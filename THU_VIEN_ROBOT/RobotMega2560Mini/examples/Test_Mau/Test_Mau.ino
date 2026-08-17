#include <Robot.h>

// Đặt bề mặt màu cách cảm biến khoảng 1–2 cm.
void setup() {
  batDauSerial();
  oledBatDau();
  camBienMauBatDau();
  oledThongBao("TEST TCS3200", "DAT MAU CACH", "CAM BIEN 1-2 CM", "");
  delay(1000);
}

void loop() {
  DuLieuMau d = docCamBienMau();
  MauSac mau = nhanDangMau(d);

  oledXoa();
  oledTieuDe("CAM BIEN MAU");
  oledInChu(0, 17, "R:"); oledInSo(15, 17, d.doPulse);
  oledInChu(65, 17, "G:"); oledInSo(80, 17, d.xanhLaPulse);
  oledInChu(0, 33, "B:"); oledInSo(15, 33, d.xanhDuongPulse);
  oledInChu(65, 33, "C:"); oledInSo(80, 33, d.clearPulse);
  oledInChu(0, 51, d.hopLe ? tenMau(mau) : "NO PULSE");
  oledCapNhat();

  Serial.print(F("R=")); Serial.print(d.doPulse);
  Serial.print(F(" G=")); Serial.print(d.xanhLaPulse);
  Serial.print(F(" B=")); Serial.print(d.xanhDuongPulse);
  Serial.print(F(" C=")); Serial.print(d.clearPulse);
  Serial.print(F(" -> ")); Serial.println(tenMau(mau));
  delay(200);
}
