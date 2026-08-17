#include <Robot.h>

// Nâng hai bánh khỏi mặt bàn trước khi chạy ví dụ này.
void chayMotDongTac(const char *ten, int trai, int phai) {
  oledThongBao("TEST DONG CO", ten, "CHAY 1 GIAY", "");
  Serial.println(ten);
  dieuKhienDongCo(trai, phai);
  delay(1000);
  dungRobot();
  delay(500);
}

void setup() {
  batDauSerial();
  oledBatDau();
  dongCoBatDau();
  oledThongBao("TEST DONG CO", "NANG BANH XE LEN", "BAT DAU SAU 2 GIAY", "");
  delay(2000);
}

void loop() {
  chayMotDongTac("DI THANG", 100, 100);
  chayMotDongTac("DI LUI", -100, -100);
  chayMotDongTac("QUAY TRAI", -100, 100);
  chayMotDongTac("QUAY PHAI", 100, -100);
}
