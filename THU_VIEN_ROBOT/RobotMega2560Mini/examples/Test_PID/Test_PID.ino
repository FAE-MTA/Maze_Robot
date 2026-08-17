#include <Robot.h>

// Đặt robot lên đường line trước khi chạy. Bánh xe sẽ tự quay.
BoDieuKhienPID pidLine;
unsigned long lanHienThiMs = 0;

void setup() {
  batDauSerial();
  oledBatDau();
  dongCoBatDau();
  pidCauHinh(pidLine, 0.45f, 0.02f, 0.003f, 500.0f);
  oledThongBao("TEST PID", "DAT ROBOT LEN LINE", "CHO 2 GIAY", "");
  delay(2000);
}

void loop() {
  bool thayVach = bamLinePIDMotBuoc(pidLine, 110, 190, 75);

  if (millis() - lanHienThiMs >= 200) {
    DuLieuLine line = docLine();
    oledXoa();
    oledTieuDe("TEST BAM LINE PID");
    oledInChu(4, 24, thayVach ? "DANG BAM VACH" : "DANG TIM VACH");
    oledInChu(4, 44, "SAI SO:"); oledInSo(58, 44, line.viTri);
    oledCapNhat();
    Serial.print(F("Thay vach=")); Serial.print(thayVach);
    Serial.print(F("  Sai so=")); Serial.println(line.viTri);
    lanHienThiMs = millis();
  }
  delay(10);
}
