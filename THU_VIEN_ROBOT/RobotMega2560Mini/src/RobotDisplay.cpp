#include "Robot.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

namespace {
  Adafruit_SSD1306 manHinh(128, 64, &Wire, -1);
  bool oledReady = false;
}

void batDauSerial(unsigned long baud) {
  Serial.begin(baud);
}

bool oledBatDau(uint8_t diaChi) {
  Wire.begin();
  oledReady = manHinh.begin(SSD1306_SWITCHCAPVCC, diaChi);
  if (oledReady) {
    manHinh.setTextColor(SSD1306_WHITE);
    manHinh.clearDisplay();
    manHinh.display();
  }
  return oledReady;
}

bool oledDaSanSang() {
  return oledReady;
}

void oledXoa() {
  if (!oledReady) return;
  manHinh.clearDisplay();
}

void oledTieuDe(const char *tieuDe) {
  if (!oledReady) return;
  manHinh.drawRect(0, 0, 128, 64, SSD1306_WHITE);
  manHinh.setTextSize(1);
  manHinh.setCursor(4, 3);
  manHinh.print(tieuDe);
  manHinh.drawLine(0, 13, 127, 13, SSD1306_WHITE);
}

void oledInChu(int16_t x, int16_t y, const char *noiDung, uint8_t coChu) {
  if (!oledReady) return;
  manHinh.setTextSize(coChu);
  manHinh.setCursor(x, y);
  manHinh.print(noiDung);
}

void oledInSo(int16_t x, int16_t y, long giaTri, uint8_t coChu) {
  if (!oledReady) return;
  manHinh.setTextSize(coChu);
  manHinh.setCursor(x, y);
  manHinh.print(giaTri);
}

void oledInSoThuc(int16_t x, int16_t y, float giaTri,
                 uint8_t soChuSoThapPhan, uint8_t coChu) {
  if (!oledReady) return;
  manHinh.setTextSize(coChu);
  manHinh.setCursor(x, y);
  manHinh.print(giaTri, soChuSoThapPhan);
}

void oledCapNhat() {
  if (!oledReady) return;
  manHinh.display();
}

void oledThongBao(const char *tieuDe, const char *dong1,
                 const char *dong2, const char *dong3) {
  if (!oledReady) return;
  oledXoa();
  oledTieuDe(tieuDe);
  oledInChu(4, 18, dong1);
  oledInChu(4, 33, dong2);
  oledInChu(4, 48, dong3);
  oledCapNhat();
}

