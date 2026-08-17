// BÀI 8: CẢM BIẾN SIÊU ÂM PHÍA TRƯỚC
// Cấu hình đã kiểm tra: TRIG D4, ECHO D7.
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const uint8_t TRIG_PIN = 4;
const uint8_t ECHO_PIN = 7;
const unsigned long TIMEOUT_US = 30000UL;

Adafruit_SSD1306 oled(128, 64, &Wire, -1);

bool docKhoangCach(float &khoangCachCm) {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long thoiGianUs = pulseIn(ECHO_PIN, HIGH, TIMEOUT_US);
  if (thoiGianUs == 0) {
    khoangCachCm = -1.0f;
    return false;
  }

  khoangCachCm = thoiGianUs * 0.0343f / 2.0f;
  return khoangCachCm >= 2.0f && khoangCachCm <= 400.0f;
}

void hienThi(bool hopLe, float cm) {
  oled.clearDisplay();
  oled.drawRect(0, 0, 128, 64, SSD1306_WHITE);
  oled.setTextColor(SSD1306_WHITE);
  oled.setTextSize(1);
  oled.setCursor(8, 5);
  oled.println(F("SIEU AM PHIA TRUOC"));
  oled.drawLine(0, 17, 127, 17, SSD1306_WHITE);

  if (hopLe) {
    oled.setTextSize(3);
    oled.setCursor(5, 28);
    oled.print(cm, 1);
    oled.setTextSize(1);
    oled.print(F(" cm"));
  } else {
    oled.setTextSize(2);
    oled.setCursor(18, 31);
    oled.println(F("NO ECHO"));
  }
  oled.display();
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  Wire.begin();
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("LOI OLED 0x3C"));
    while (true) delay(1000);
  }
}

void loop() {
  float khoangCachCm;
  bool hopLe = docKhoangCach(khoangCachCm);

  Serial.print(F("Truoc: "));
  if (hopLe) {
    Serial.print(khoangCachCm, 1);
    Serial.println(F(" cm"));
  } else {
    Serial.println(F("NO ECHO"));
  }

  hienThi(hopLe, khoangCachCm);
  delay(200);
}
