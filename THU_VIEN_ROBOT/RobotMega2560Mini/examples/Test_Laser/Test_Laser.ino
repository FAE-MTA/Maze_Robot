#include <Robot.h>

void setup() {
  batDauSerial();
  oledBatDau();
  bool coLaser = laserBatDau();
  oledThongBao("TEST HAI LASER", coLaser ? "DA KHOI TAO" : "KHONG TIM THAY",
               "TRAI XSHUT=39", "PHAI XSHUT=37");
  delay(1000);
}

void loop() {
  uint16_t traiMm = 0, phaiMm = 0;
  bool traiTot = docLaserTrai(traiMm);
  bool phaiTot = docLaserPhai(phaiMm);

  oledXoa();
  oledTieuDe("LASER TRAI / PHAI");
  oledInChu(4, 25, "TRAI:");
  if (traiTot) oledInSo(45, 25, traiMm); else oledInChu(45, 25, "LOI");
  oledInChu(4, 45, "PHAI:");
  if (phaiTot) oledInSo(45, 45, phaiMm); else oledInChu(45, 45, "LOI");
  oledCapNhat();

  Serial.print(F("Laser trai=")); Serial.print(traiMm);
  Serial.print(F(" mm  Laser phai=")); Serial.print(phaiMm);
  Serial.println(F(" mm"));
  delay(200);
}
