#include <Robot.h>

void setup() {
  batDauSerial();
  oledBatDau();
  sieuAmBatDau();
}

void loop() {
  float khoangCachCm;
  bool hopLe = docSieuAm(khoangCachCm);

  oledXoa();
  oledTieuDe("SIEU AM HC-SR04");
  oledInChu(4, 24, "TRIG=4  ECHO=7");
  oledInChu(4, 44, "KC:");
  if (hopLe) oledInSoThuc(28, 44, khoangCachCm);
  else oledInChu(28, 44, "NO ECHO");
  oledCapNhat();

  Serial.print(F("Khoang cach truoc: "));
  if (hopLe) { Serial.print(khoangCachCm); Serial.println(F(" cm")); }
  else Serial.println(F("NO ECHO"));
  delay(200);
}
