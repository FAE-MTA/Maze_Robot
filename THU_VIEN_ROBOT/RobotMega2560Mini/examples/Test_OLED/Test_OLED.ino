#include <Robot.h>

// Kiểm tra riêng màn hình OLED và cổng Serial.
unsigned long boDem = 0;

void setup() {
  batDauSerial();
  oledBatDau();
  oledThongBao("TEST OLED", "KHOI TAO THANH CONG", "DIA CHI: 0x3C", "");
}

void loop() {
  oledXoa();
  oledTieuDe("TEST OLED DOC LAP");
  oledInChu(4, 22, "BO DEM:");
  oledInSo(52, 22, boDem);
  oledInChu(4, 42, "SDA=20  SCL=21");
  oledCapNhat();

  Serial.print(F("Bo dem OLED: "));
  Serial.println(boDem++);
  delay(500);
}
