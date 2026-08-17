// BÀI 15: ĐỒNG BỘ TỐC ĐỘ HAI BÁNH BẰNG ENCODER
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const uint8_t MOTOR_TRAI_A = 5, MOTOR_TRAI_B = 9;
const uint8_t MOTOR_PHAI_A = 6, MOTOR_PHAI_B = 10;
const uint8_t ENC_TRAI_A = 2, ENC_TRAI_B = 12;
const uint8_t ENC_PHAI_A = 3, ENC_PHAI_B = 14;

const int PWM_CO_BAN = 115;
const float KP_DONG_BO = 4.0;      // Hiệu chỉnh trên robot thật.
const unsigned long CHU_KY_MS = 50;
const unsigned long THOI_GIAN_CHAY_MS = 5000;

volatile long xungTrai = 0;
volatile long xungPhai = 0;
Adafruit_SSD1306 oled(128, 64, &Wire, -1);

void ngatTrai() { xungTrai++; }
void ngatPhai() { xungPhai++; }

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
void dungRobot() { datHaiDongCo(0, 0); }

void chupEncoder(long &trai, long &phai) {
  noInterrupts();
  trai = xungTrai;
  phai = xungPhai;
  interrupts();
}

void setup() {
  Serial.begin(115200);
  pinMode(MOTOR_TRAI_A, OUTPUT); pinMode(MOTOR_TRAI_B, OUTPUT);
  pinMode(MOTOR_PHAI_A, OUTPUT); pinMode(MOTOR_PHAI_B, OUTPUT);
  pinMode(ENC_TRAI_A, INPUT_PULLUP); pinMode(ENC_TRAI_B, INPUT_PULLUP);
  pinMode(ENC_PHAI_A, INPUT_PULLUP); pinMode(ENC_PHAI_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_TRAI_A), ngatTrai, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_PHAI_A), ngatPhai, RISING);
  dungRobot();

  Wire.begin();
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("LOI OLED");
    while (true) delay(1000);
  }
  delay(2000);
}

void loop() {
  static bool dangChay = true;
  static unsigned long batDau = millis();
  static unsigned long mocDieuKhien = millis();
  static unsigned long mocOled = 0;
  static long xungTraiCu = 0, xungPhaiCu = 0;
  static int pwmTrai = PWM_CO_BAN, pwmPhai = PWM_CO_BAN;
  static long saiSo = 0;

  if (!dangChay) {
    dungRobot();
    return;
  }

  if (millis() - batDau >= THOI_GIAN_CHAY_MS) {
    dangChay = false;
    dungRobot();
    Serial.println("DA DUNG SAU 5 GIAY");
    return;
  }

  if (millis() - mocDieuKhien >= CHU_KY_MS) {
    mocDieuKhien += CHU_KY_MS;
    long trai, phai;
    chupEncoder(trai, phai);
    long deltaTrai = trai - xungTraiCu;
    long deltaPhai = phai - xungPhaiCu;
    xungTraiCu = trai;
    xungPhaiCu = phai;

    saiSo = deltaTrai - deltaPhai;
    int hieuChinh = lround(KP_DONG_BO * saiSo);
    pwmTrai = constrain(PWM_CO_BAN - hieuChinh, 0, 200);
    pwmPhai = constrain(PWM_CO_BAN + hieuChinh, 0, 200);
    datHaiDongCo(pwmTrai, pwmPhai);
  }

  if (millis() - mocOled >= 200) {
    mocOled = millis();
    Serial.print("PWM T/P="); Serial.print(pwmTrai);
    Serial.print('/'); Serial.print(pwmPhai);
    Serial.print("  SAI_SO="); Serial.println(saiSo);

    oled.clearDisplay();
    oled.setTextColor(SSD1306_WHITE);
    oled.setTextSize(1);
    oled.setCursor(0, 0);  oled.println("DONG BO ENCODER");
    oled.setCursor(0, 20); oled.print("PWM T: "); oled.println(pwmTrai);
    oled.setCursor(0, 34); oled.print("PWM P: "); oled.println(pwmPhai);
    oled.setCursor(0, 50); oled.print("Sai so: "); oled.println(saiSo);
    oled.display();
  }
}
