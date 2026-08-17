// BÀI 17: KẾT HỢP SIÊU ÂM TRƯỚC VÀ HAI LASER BÊN
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_VL53L0X.h>

const uint8_t TRIG_PIN = 4, ECHO_PIN = 7;
const uint8_t XSHUT_TRAI = 39, XSHUT_PHAI = 37;
const uint8_t DIA_CHI_TRAI = 0x30, DIA_CHI_PHAI = 0x31;
const uint8_t M1_A = 5, M1_B = 9, M2_A = 6, M2_B = 10;
const bool DAO_M1 = false, DAO_M2 = true;

const float NGUONG_TRUOC_CM = 25.0f;
const uint16_t NGUONG_NGO_CUT_MM = 180;
const uint16_t KHOANG_CACH_MO_MM = 2000;
const int PWM_DI = 110, PWM_QUAY = 120, PWM_LUI = 100;
const unsigned long QUAY_90_MS = 520;
const unsigned long QUAY_180_MS = 1050;
const bool MAT_ECHO_XEM_LA_TRONG = true;

Adafruit_SSD1306 oled(128, 64, &Wire, -1);
Adafruit_VL53L0X laserTrai, laserPhai;
bool lanTruocQuayPhai = false;

void datMotDongCo(uint8_t a, uint8_t b, int pwm, bool dao) {
  pwm = constrain(pwm, -255, 255);
  if (dao) pwm = -pwm;
  if (pwm > 0) { analogWrite(a, pwm); analogWrite(b, 0); }
  else if (pwm < 0) { analogWrite(a, 0); analogWrite(b, -pwm); }
  else { analogWrite(a, 0); analogWrite(b, 0); }
}

void datHaiDongCo(int trai, int phai) {
  datMotDongCo(M1_A, M1_B, trai, DAO_M1);
  datMotDongCo(M2_A, M2_B, phai, DAO_M2);
}

bool docTruoc(float &cm) {
  digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  unsigned long us = pulseIn(ECHO_PIN, HIGH, 30000UL);
  if (us == 0) { cm = -1.0f; return false; }
  cm = us * 0.0343f / 2.0f;
  return true;
}

uint16_t docBen(Adafruit_VL53L0X &laser, bool &hopLe) {
  VL53L0X_RangingMeasurementData_t kq;
  laser.rangingTest(&kq, false);
  hopLe = (kq.RangeStatus == 0);
  return hopLe ? kq.RangeMilliMeter : KHOANG_CACH_MO_MM;
}

void hienThi(const __FlashStringHelper *trangThai,
             float truoc, bool okTruoc,
             uint16_t trai, uint16_t phai) {
  oled.clearDisplay();
  oled.drawRect(0, 0, 128, 64, SSD1306_WHITE);
  oled.setTextColor(SSD1306_WHITE);
  oled.setTextSize(1);
  oled.setCursor(5, 3); oled.println(F("SIEU AM + 2 LASER"));
  oled.drawLine(0, 14, 127, 14, SSD1306_WHITE);
  oled.setCursor(4, 18); oled.print(F("F:"));
  okTruoc ? oled.print(truoc, 1) : oled.print(F("NO ECHO"));
  oled.setCursor(4, 31); oled.print(F("L:")); oled.print(trai);
  oled.print(F(" R:")); oled.print(phai);
  oled.setCursor(4, 45); oled.print(F("TT:")); oled.println(trangThai);
  oled.setCursor(4, 56); oled.print(F("F cm; L/R mm"));
  oled.display();
}

void dungBaoLoi(const __FlashStringHelper *loi) {
  datHaiDongCo(0, 0);
  Serial.println(loi);
  hienThi(loi, -1, false, 0, 0);
  while (true) delay(1000);
}

void khoiTaoLaser() {
  digitalWrite(XSHUT_TRAI, LOW);
  digitalWrite(XSHUT_PHAI, LOW);
  delay(10);
  digitalWrite(XSHUT_TRAI, HIGH); delay(10);
  if (!laserTrai.begin(DIA_CHI_TRAI)) dungBaoLoi(F("LOI LASER TRAI"));
  digitalWrite(XSHUT_PHAI, HIGH); delay(10);
  if (!laserPhai.begin(DIA_CHI_PHAI)) dungBaoLoi(F("LOI LASER PHAI"));
}

void xuLyVatCan(float truoc, bool okTruoc,
                uint16_t trai, uint16_t phai) {
  datHaiDongCo(0, 0);
  hienThi(F("VAT CAN"), truoc, okTruoc, trai, phai);
  delay(150);

  if (trai < NGUONG_NGO_CUT_MM && phai < NGUONG_NGO_CUT_MM) {
    hienThi(F("NGO CUT"), truoc, okTruoc, trai, phai);
    datHaiDongCo(-PWM_LUI, -PWM_LUI); delay(300);
    datHaiDongCo(PWM_QUAY, -PWM_QUAY); delay(QUAY_180_MS);
    lanTruocQuayPhai = true;
  } else if (trai > phai + 40) {
    hienThi(F("QUAY TRAI"), truoc, okTruoc, trai, phai);
    datHaiDongCo(-PWM_QUAY, PWM_QUAY); delay(QUAY_90_MS);
    lanTruocQuayPhai = false;
  } else if (phai > trai + 40) {
    hienThi(F("QUAY PHAI"), truoc, okTruoc, trai, phai);
    datHaiDongCo(PWM_QUAY, -PWM_QUAY); delay(QUAY_90_MS);
    lanTruocQuayPhai = true;
  } else if (lanTruocQuayPhai) {
    datHaiDongCo(-PWM_QUAY, PWM_QUAY); delay(QUAY_90_MS);
    lanTruocQuayPhai = false;
  } else {
    datHaiDongCo(PWM_QUAY, -PWM_QUAY); delay(QUAY_90_MS);
    lanTruocQuayPhai = true;
  }
  datHaiDongCo(0, 0);
  delay(120);
}

void setup() {
  Serial.begin(115200);
  pinMode(M1_A, OUTPUT); pinMode(M1_B, OUTPUT);
  pinMode(M2_A, OUTPUT); pinMode(M2_B, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT); pinMode(ECHO_PIN, INPUT);
  pinMode(XSHUT_TRAI, OUTPUT); pinMode(XSHUT_PHAI, OUTPUT);
  datHaiDongCo(0, 0);

  Wire.begin();
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("LOI OLED"));
    while (true) delay(1000);
  }
  khoiTaoLaser();
  delay(1500);
}

void loop() {
  float truoc;
  bool okTruoc = docTruoc(truoc);
  bool okTrai, okPhai;
  uint16_t trai = docBen(laserTrai, okTrai);
  uint16_t phai = docBen(laserPhai, okPhai);

  Serial.print(F("F="));
  okTruoc ? Serial.print(truoc, 1) : Serial.print(F("NO_ECHO"));
  Serial.print(F("cm L=")); Serial.print(trai);
  Serial.print(F("mm R=")); Serial.print(phai); Serial.println(F("mm"));

  bool coVatCan = okTruoc && truoc <= NGUONG_TRUOC_CM;
  if (coVatCan) {
    xuLyVatCan(truoc, okTruoc, trai, phai);
  } else if (!okTruoc && !MAT_ECHO_XEM_LA_TRONG) {
    datHaiDongCo(0, 0);
    hienThi(F("DUNG - MAT ECHO"), truoc, okTruoc, trai, phai);
  } else {
    datHaiDongCo(PWM_DI, PWM_DI);
    hienThi(F("DI THANG"), truoc, okTruoc, trai, phai);
  }
  delay(60);
}
