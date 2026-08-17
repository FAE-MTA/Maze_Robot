// BÀI 14: ĐI QUÃNG ĐƯỜNG ĐẶT TRƯỚC BẰNG ENCODER
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const uint8_t MOTOR_TRAI_A = 5, MOTOR_TRAI_B = 9;
const uint8_t MOTOR_PHAI_A = 6, MOTOR_PHAI_B = 10;
const uint8_t ENC_TRAI_A = 2, ENC_TRAI_B = 12;
const uint8_t ENC_PHAI_A = 3, ENC_PHAI_B = 14;

const float XUNG_MOI_VONG = 20.0;   // Hiệu chỉnh thực tế.
const float DUONG_KINH_BANH_CM = 6.5;
const float QUANG_DUONG_MUC_TIEU_CM = 50.0;
const int PWM_CHAY = 105;
const unsigned long TIMEOUT_MS = 10000UL;

volatile long xungTrai = 0;
volatile long xungPhai = 0;
Adafruit_SSD1306 oled(128, 64, &Wire, -1);

void ngatTrai() {
  xungTrai += digitalRead(ENC_TRAI_B) ? 1 : -1;
}
void ngatPhai() {
  // Đảo dấu vì encoder phải được lắp đối xứng.
  xungPhai += digitalRead(ENC_PHAI_B) ? -1 : 1;
}

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

void datLaiEncoder() {
  noInterrupts();
  xungTrai = 0;
  xungPhai = 0;
  interrupts();
}

void chupEncoder(long &trai, long &phai) {
  noInterrupts();
  trai = xungTrai;
  phai = xungPhai;
  interrupts();
}

float xungSangCm(long soXung) {
  return abs(soXung) * PI * DUONG_KINH_BANH_CM / XUNG_MOI_VONG;
}

bool diQuangDuong(float mucTieuCm) {
  long xungMucTieu = lround(mucTieuCm * XUNG_MOI_VONG /
                            (PI * DUONG_KINH_BANH_CM));
  datLaiEncoder();
  unsigned long batDau = millis();
  unsigned long capNhatOled = 0;

  while (millis() - batDau < TIMEOUT_MS) {
    long trai, phai;
    chupEncoder(trai, phai);
    long daDiTrai = abs(trai);
    long daDiPhai = abs(phai);

    bool traiDat = daDiTrai >= xungMucTieu;
    bool phaiDat = daDiPhai >= xungMucTieu;
    if (traiDat && phaiDat) {
      dungRobot();
      return true;
    }

    // Bánh nào đạt trước sẽ dừng, bánh còn lại tiếp tục đến mục tiêu.
    datHaiDongCo(traiDat ? 0 : PWM_CHAY,
                 phaiDat ? 0 : PWM_CHAY);

    if (millis() - capNhatOled >= 150) {
      capNhatOled = millis();
      float cmTrai = xungSangCm(trai);
      float cmPhai = xungSangCm(phai);
      Serial.print("T/P cm: ");
      Serial.print(cmTrai, 1); Serial.print(" / "); Serial.println(cmPhai, 1);

      oled.clearDisplay();
      oled.setTextColor(SSD1306_WHITE);
      oled.setTextSize(1);
      oled.setCursor(0, 0);  oled.println("DI 50 cm ENCODER");
      oled.setCursor(0, 20); oled.print("Trai: "); oled.print(cmTrai, 1); oled.println(" cm");
      oled.setCursor(0, 34); oled.print("Phai: "); oled.print(cmPhai, 1); oled.println(" cm");
      oled.setCursor(0, 50); oled.print("Muc tieu: "); oled.println(mucTieuCm, 1);
      oled.display();
    }
  }

  dungRobot(); // Dừng an toàn nếu encoder hỏng hoặc robot bị kẹt.
  return false;
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
  bool thanhCong = diQuangDuong(QUANG_DUONG_MUC_TIEU_CM);
  Serial.println(thanhCong ? "DA DAT MUC TIEU" : "LOI / TIMEOUT");
}

void loop() {
  dungRobot(); // Chỉ thực hiện một lần sau khi khởi động.
}
