#include <Robot.h>

void hienThiCoCau(const char *mau, int goc) {
  oledXoa();
  oledTieuDe("SERVO + NEOPIXEL");
  oledInChu(4, 23, "MAU:"); oledInChu(38, 23, mau);
  oledInChu(4, 42, "GOC:"); oledInSo(38, 42, goc);
  oledCapNhat();
  Serial.print(F("Mau=")); Serial.print(mau);
  Serial.print(F("  Goc servo=")); Serial.println(goc);
}

void setup() {
  batDauSerial();
  oledBatDau();
  servoBatDau(90);
  neoPixelBatDau(100);
}

void loop() {
  datGocServo(30); datMauNeoPixel(100, 0, 0);
  hienThiCoCau("DO", 30); delay(1000);

  datGocServo(90); datMauNeoPixel(0, 100, 0);
  hienThiCoCau("XANH LA", 90); delay(1000);

  datGocServo(150); datMauNeoPixel(0, 0, 100);
  hienThiCoCau("XANH DUONG", 150); delay(1000);
}
