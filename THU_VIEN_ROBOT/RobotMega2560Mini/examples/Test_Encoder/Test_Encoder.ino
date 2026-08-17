#include <Robot.h>

// Động cơ tự quay thuận rồi quay ngược; không cần xoay bánh bằng tay.
// Nâng hai bánh khỏi mặt bàn trước khi thử.
long xungTraiTruoc = 0;
long xungPhaiTruoc = 0;
unsigned long lanDoMs = 0;
bool dangQuayThuan = true;
unsigned long batDauChieuMs = 0;

void setup() {
  batDauSerial();
  oledBatDau();
  dongCoBatDau();
  encoderBatDau();
  oledThongBao("TEST ENCODER", "NANG BANH XE LEN", "TU QUAY 2 CHIEU", "");
  delay(2000);
  batDauChieuMs = millis();
  lanDoMs = millis();
}

void loop() {
  if (millis() - batDauChieuMs >= 3000) {
    dangQuayThuan = !dangQuayThuan;
    batDauChieuMs = millis();
    xoaEncoder();
    xungTraiTruoc = 0;
    xungPhaiTruoc = 0;
  }

  if (dangQuayThuan) diThang(90);
  else diLui(90);

  if (millis() - lanDoMs >= 500) {
    unsigned long khoangThoiGianMs = millis() - lanDoMs;
    long trai, phai;
    docHaiEncoder(trai, phai);
    float rpmTrai = tinhRPM(trai - xungTraiTruoc,
                            RobotDefaults::TICKS_PER_REV, khoangThoiGianMs);
    float rpmPhai = tinhRPM(phai - xungPhaiTruoc,
                            RobotDefaults::TICKS_PER_REV, khoangThoiGianMs);
    xungTraiTruoc = trai;
    xungPhaiTruoc = phai;
    lanDoMs = millis();

    oledXoa();
    oledTieuDe(dangQuayThuan ? "ENCODER - TIEN" : "ENCODER - LUI");
    oledInChu(4, 23, "XUNG L/R:");
    oledInSo(66, 23, trai); oledInSo(96, 23, phai);
    oledInChu(4, 43, "RPM L/R:");
    oledInSoThuc(60, 43, rpmTrai); oledInSoThuc(94, 43, rpmPhai);
    oledCapNhat();

    Serial.print(F("Xung L/R=")); Serial.print(trai); Serial.print('/');
    Serial.print(phai); Serial.print(F("  RPM L/R="));
    Serial.print(rpmTrai); Serial.print('/'); Serial.println(rpmPhai);
  }
}
