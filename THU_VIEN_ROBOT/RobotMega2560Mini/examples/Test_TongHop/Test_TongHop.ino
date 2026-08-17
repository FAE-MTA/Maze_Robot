#include <Robot.h>

// Kiểm tra khởi tạo toàn robot nhưng không tự chạy động cơ.
// Vòng LED báo xanh nếu ít nhất siêu âm hoạt động, đỏ nếu mất Echo.
void setup() {
  batDauSerial();
  oledBatDau();
  dongCoBatDau();
  encoderBatDau();
  servoBatDau(90);
  neoPixelBatDau(60);
  sieuAmBatDau();
  laserBatDau();
  camBienMauBatDau();
  dungRobot();
}

void loop() {
  DuLieuKhoangCach kc = docBaHuong();
  DuLieuLine line = docLine();
  DuLieuMau mau = docCamBienMau();

  if (kc.sieuAmHopLe) datMauNeoPixel(0, 60, 0);
  else datMauNeoPixel(60, 0, 0);

  oledXoa();
  oledTieuDe("TEST TOAN ROBOT");
  oledInChu(0, 17, "T:"); oledInSoThuc(14, 17, kc.truocCm);
  oledInChu(64, 17, "L/R:"); oledInSo(94, 17, kc.traiMm);
  oledInSo(0, 32, kc.phaiMm);
  oledInChu(42, 32, "LINE:"); oledInSo(77, 32, line.viTri);
  oledInChu(0, 49, "MAU:");
  oledInChu(34, 49, tenMau(nhanDangMau(mau)));
  oledCapNhat();

  Serial.print(F("KC T/L/R=")); Serial.print(kc.truocCm);
  Serial.print('/'); Serial.print(kc.traiMm); Serial.print('/');
  Serial.print(kc.phaiMm); Serial.print(F("  Line="));
  Serial.print(line.viTri); Serial.print(F("  Mau="));
  Serial.println(tenMau(nhanDangMau(mau)));
  delay(250);
}
