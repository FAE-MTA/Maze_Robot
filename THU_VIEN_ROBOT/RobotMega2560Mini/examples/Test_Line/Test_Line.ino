#include <Robot.h>

void setup() {
  batDauSerial();
  oledBatDau();
}

void loop() {
  DuLieuLine line = docLine();

  oledXoa();
  oledTieuDe("TEST 5 MAT LINE");
  oledInChu(0, 20, "A1 A2 A3 A4 A5");
  for (uint8_t i = 0; i < 5; i++) oledInSo(i * 25, 34, line.giaTri[i]);
  oledInChu(0, 51, "VT:"); oledInSo(20, 51, line.viTri);
  oledInChu(63, 51, line.thayVach ? "CO VACH" : "MAT VACH");
  oledCapNhat();

  Serial.print(F("A1..A5: "));
  for (uint8_t i = 0; i < 5; i++) {
    Serial.print(line.giaTri[i]); Serial.print(i < 4 ? ',' : ' ');
  }
  Serial.print(F("Vi tri=")); Serial.println(line.viTri);
  delay(150);
}
