// BÀI 16: ROBOT BÁM LINE BẰNG PID
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const uint8_t LINE_PIN[5] = {A1, A2, A3, A4, A5};
const int TRONG_SO[5] = {-200, -100, 0, 100, 200};
int nguong[5] = {500, 500, 500, 500, 500}; // Hiệu chỉnh thực tế.

const uint8_t MOTOR_TRAI_A = 5, MOTOR_TRAI_B = 9;
const uint8_t MOTOR_PHAI_A = 6, MOTOR_PHAI_B = 10;

const int TOC_DO_CO_BAN = 105;
const float KP = 0.55;
const float KI = 0.000;
const float KD = 0.90;
const float GIOI_HAN_TICH_PHAN = 500.0;

Adafruit_SSD1306 oled(128, 64, &Wire, -1);
float tongSaiSo = 0.0;
float saiSoTruoc = 0.0;
float saiSoCuoiCoLine = 0.0;
unsigned long mocUs = 0;

void datMotDongCo(uint8_t a, uint8_t b, int pwm, bool dao) {
  pwm = constrain(pwm, -255, 255);
  if (dao) pwm = -pwm;
  if (pwm > 0) { analogWrite(a, pwm); analogWrite(b, 0); }
  else if (pwm < 0) { analogWrite(a, 0); analogWrite(b, -pwm); }
  else { analogWrite(a, 0); analogWrite(b, 0); }
}
void datHaiDongCo(int trai, int phai) {
  datMotDongCo(MOTOR_TRAI_A, MOTOR_TRAI_B, trai, false);
  datMotDongCo(MOTOR_PHAI_A, MOTOR_PHAI_B, phai, true);
}

bool docViTriLine(float &viTri, int &tongCuongDo) {
  long tongTrongSo = 0;
  tongCuongDo = 0;
  for (uint8_t i = 0; i < 5; i++) {
    int raw = analogRead(LINE_PIN[i]);
    int cuongDo = nguong[i] - raw; // Vạch tối cho giá trị thấp.
    if (cuongDo < 0) cuongDo = 0;
    tongTrongSo += (long)cuongDo * TRONG_SO[i];
    tongCuongDo += cuongDo;
  }
  if (tongCuongDo < 80) return false;
  viTri = (float)tongTrongSo / tongCuongDo;
  return true;
}

void setup() {
  Serial.begin(115200);
  for (uint8_t i = 0; i < 5; i++) pinMode(LINE_PIN[i], INPUT);
  pinMode(MOTOR_TRAI_A, OUTPUT); pinMode(MOTOR_TRAI_B, OUTPUT);
  pinMode(MOTOR_PHAI_A, OUTPUT); pinMode(MOTOR_PHAI_B, OUTPUT);
  datHaiDongCo(0, 0);

  Wire.begin();
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("LOI OLED");
    while (true) delay(1000);
  }
  delay(2000);
  mocUs = micros();
}

void loop() {
  unsigned long bayGioUs = micros();
  float dt = (bayGioUs - mocUs) / 1000000.0;
  mocUs = bayGioUs;
  if (dt <= 0.0 || dt > 0.1) dt = 0.01;

  float saiSo = 0.0;
  int tongCuongDo = 0;
  bool coLine = docViTriLine(saiSo, tongCuongDo);
  float dieuKhien = 0.0;
  int pwmTrai = 0, pwmPhai = 0;

  if (coLine) {
    saiSoCuoiCoLine = saiSo;
    tongSaiSo += saiSo * dt;
    tongSaiSo = constrain(tongSaiSo,
                          -GIOI_HAN_TICH_PHAN,
                           GIOI_HAN_TICH_PHAN);
    float daoHam = (saiSo - saiSoTruoc) / dt;
    saiSoTruoc = saiSo;

    dieuKhien = KP * saiSo + KI * tongSaiSo + KD * daoHam;
    dieuKhien = constrain(dieuKhien, -140.0, 140.0);

    // Sai số dương: line ở bên phải -> tăng bánh trái, giảm bánh phải.
    pwmTrai = constrain((int)(TOC_DO_CO_BAN + dieuKhien), -160, 200);
    pwmPhai = constrain((int)(TOC_DO_CO_BAN - dieuKhien), -160, 200);
  } else {
    // Tìm lại line theo hướng nhìn thấy gần nhất.
    tongSaiSo = 0.0;
    if (saiSoCuoiCoLine < 0) {
      pwmTrai = -75; pwmPhai = 75;
    } else {
      pwmTrai = 75; pwmPhai = -75;
    }
  }

  datHaiDongCo(pwmTrai, pwmPhai);

  static unsigned long mocHienThi = 0;
  if (millis() - mocHienThi >= 200) {
    mocHienThi = millis();
    Serial.print("LINE="); Serial.print(coLine);
    Serial.print(" E="); Serial.print(saiSo, 1);
    Serial.print(" U="); Serial.print(dieuKhien, 1);
    Serial.print(" PWM="); Serial.print(pwmTrai);
    Serial.print('/'); Serial.println(pwmPhai);

    oled.clearDisplay();
    oled.setTextColor(SSD1306_WHITE);
    oled.setTextSize(1);
    oled.setCursor(0, 0);  oled.println("BAM LINE PID");
    oled.setCursor(0, 17); oled.print("Line: "); oled.println(coLine ? "CO" : "MAT");
    oled.setCursor(0, 30); oled.print("Sai so: "); oled.println(saiSo, 1);
    oled.setCursor(0, 43); oled.print("PID: "); oled.println(dieuKhien, 1);
    oled.setCursor(0, 56); oled.print("PWM "); oled.print(pwmTrai); oled.print('/'); oled.print(pwmPhai);
    oled.display();
  }
}
