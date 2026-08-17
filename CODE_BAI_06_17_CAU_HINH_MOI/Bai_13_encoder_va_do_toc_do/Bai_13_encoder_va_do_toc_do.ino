// BÀI 13: ĐỌC ENCODER VÀ TÍNH RPM
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const uint8_t ENC_TRAI_A = 2;
const uint8_t ENC_TRAI_B = 12;
const uint8_t ENC_PHAI_A = 3;
const uint8_t ENC_PHAI_B = 14;

// Hiệu chỉnh theo encoder và phương pháp đếm thực tế.
const float XUNG_MOI_VONG = 20.0;
const int8_t HE_SO_CHIEU_TRAI = 1;
const int8_t HE_SO_CHIEU_PHAI = -1;

volatile long xungTrai = 0;
volatile long xungPhai = 0;

Adafruit_SSD1306 oled(128, 64, &Wire, -1);

void ngatEncoderTrai() {
  int huong = digitalRead(ENC_TRAI_B) ? 1 : -1;
  xungTrai += huong * HE_SO_CHIEU_TRAI;
}

void ngatEncoderPhai() {
  int huong = digitalRead(ENC_PHAI_B) ? 1 : -1;
  xungPhai += huong * HE_SO_CHIEU_PHAI;
}

void chupSoXung(long &trai, long &phai) {
  noInterrupts();
  trai = xungTrai;
  phai = xungPhai;
  interrupts();
}

void setup() {
  Serial.begin(115200);
  pinMode(ENC_TRAI_A, INPUT_PULLUP);
  pinMode(ENC_TRAI_B, INPUT_PULLUP);
  pinMode(ENC_PHAI_A, INPUT_PULLUP);
  pinMode(ENC_PHAI_B, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENC_TRAI_A),
                  ngatEncoderTrai, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_PHAI_A),
                  ngatEncoderPhai, RISING);

  Wire.begin();
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("LOI OLED");
    while (true) delay(1000);
  }
  Serial.println("QUAY TUNG BANH BANG TAY");
}

void loop() {
  static unsigned long mocMs = millis();
  static long xungTraiCu = 0;
  static long xungPhaiCu = 0;

  if (millis() - mocMs >= 500) {
    unsigned long bayGio = millis();
    unsigned long dt = bayGio - mocMs;
    mocMs = bayGio;

    long trai, phai;
    chupSoXung(trai, phai);
    long deltaTrai = trai - xungTraiCu;
    long deltaPhai = phai - xungPhaiCu;
    xungTraiCu = trai;
    xungPhaiCu = phai;

    float rpmTrai = deltaTrai * 60000.0 / (XUNG_MOI_VONG * dt);
    float rpmPhai = deltaPhai * 60000.0 / (XUNG_MOI_VONG * dt);

    Serial.print("XUNG T/P: ");
    Serial.print(trai); Serial.print(" / "); Serial.print(phai);
    Serial.print("  RPM T/P: ");
    Serial.print(rpmTrai, 1); Serial.print(" / "); Serial.println(rpmPhai, 1);

    oled.clearDisplay();
    oled.setTextColor(SSD1306_WHITE);
    oled.setTextSize(1);
    oled.setCursor(0, 0);  oled.println("ENCODER QUAY TAY");
    oled.setCursor(0, 18); oled.print("Xung T: "); oled.println(trai);
    oled.setCursor(0, 30); oled.print("Xung P: "); oled.println(phai);
    oled.setCursor(0, 44); oled.print("RPM T: "); oled.println(rpmTrai, 1);
    oled.setCursor(0, 56); oled.print("RPM P: "); oled.println(rpmPhai, 1);
    oled.display();
  }
}
