// BÀI 9: HAI CẢM BIẾN LASER VL53L0X
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_VL53L0X.h>

const uint8_t XSHUT_TRAI = 39;
const uint8_t XSHUT_PHAI = 37;
const uint8_t DIA_CHI_TRAI = 0x30;
const uint8_t DIA_CHI_PHAI = 0x31;

Adafruit_SSD1306 oled(128, 64, &Wire, -1);
Adafruit_VL53L0X laserTrai;
Adafruit_VL53L0X laserPhai;

void dungBaoLoi(const __FlashStringHelper *loi) {
  Serial.println(loi);
  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE);
  oled.setTextSize(1);
  oled.setCursor(0, 10);
  oled.println(F("LOI KHOI TAO:"));
  oled.println(loi);
  oled.display();
  while (true) delay(1000);
}

void khoiTaoHaiLaser() {
  pinMode(XSHUT_TRAI, OUTPUT);
  pinMode(XSHUT_PHAI, OUTPUT);
  digitalWrite(XSHUT_TRAI, LOW);
  digitalWrite(XSHUT_PHAI, LOW);
  delay(10);

  digitalWrite(XSHUT_TRAI, HIGH);
  delay(10);
  if (!laserTrai.begin(DIA_CHI_TRAI)) {
    dungBaoLoi(F("LASER TRAI D39"));
  }

  digitalWrite(XSHUT_PHAI, HIGH);
  delay(10);
  if (!laserPhai.begin(DIA_CHI_PHAI)) {
    dungBaoLoi(F("LASER PHAI D37"));
  }
}

bool docLaser(Adafruit_VL53L0X &laser, uint16_t &mm) {
  VL53L0X_RangingMeasurementData_t ketQua;
  laser.rangingTest(&ketQua, false);
  if (ketQua.RangeStatus != 0) {
    mm = 0;
    return false;
  }
  mm = ketQua.RangeMilliMeter;
  return true;
}

void hienThi(uint16_t trai, bool okTrai,
             uint16_t phai, bool okPhai) {
  oled.clearDisplay();
  oled.drawRect(0, 0, 128, 64, SSD1306_WHITE);
  oled.setTextColor(SSD1306_WHITE);
  oled.setTextSize(1);
  oled.setCursor(8, 4); oled.println(F("LASER TRAI / PHAI"));
  oled.drawLine(0, 16, 127, 16, SSD1306_WHITE);
  oled.setTextSize(2);
  oled.setCursor(4, 23); oled.print(F("L:"));
  okTrai ? oled.print(trai) : oled.print(F("--"));
  oled.setCursor(4, 44); oled.print(F("R:"));
  okPhai ? oled.print(phai) : oled.print(F("--"));
  oled.setTextSize(1);
  oled.setCursor(103, 50); oled.print(F("mm"));
  oled.display();
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("LOI OLED 0x3C"));
    while (true) delay(1000);
  }
  khoiTaoHaiLaser();
}

void loop() {
  uint16_t trai, phai;
  bool okTrai = docLaser(laserTrai, trai);
  bool okPhai = docLaser(laserPhai, phai);

  Serial.print(F("Trai="));
  okTrai ? Serial.print(trai) : Serial.print(F("LOI"));
  Serial.print(F(" mm  Phai="));
  okPhai ? Serial.print(phai) : Serial.print(F("LOI"));
  Serial.println(F(" mm"));

  hienThi(trai, okTrai, phai, okPhai);
  delay(100);
}
