#include <Robot.h>

// Bảng theo dõi đồng thời ba hướng và năm mắt line, không chạy động cơ.
void setup() {
  batDauSerial();
  oledBatDau();
  sieuAmBatDau();
  laserBatDau();
}

void loop() {
  DuLieuKhoangCach d = docBaHuong();
  DuLieuLine line = docLine();

  oledXoa();
  oledTieuDe("TEST CAM BIEN");
  oledInChu(0, 17, "T:"); oledInSoThuc(14, 17, d.truocCm);
  oledInChu(65, 17, "L:"); oledInSo(79, 17, d.traiMm);
  oledInChu(0, 33, "R:"); oledInSo(14, 33, d.phaiMm);
  oledInChu(65, 33, "LINE:"); oledInSo(100, 33, line.viTri);
  oledInChu(0, 50, line.thayVach ? "CO VACH" : "MAT VACH");
  oledCapNhat();

  Serial.print(F("Truoc/Trai/Phai=")); Serial.print(d.truocCm);
  Serial.print('/'); Serial.print(d.traiMm); Serial.print('/');
  Serial.print(d.phaiMm); Serial.print(F("  Line="));
  Serial.println(line.viTri);
  delay(200);
}
