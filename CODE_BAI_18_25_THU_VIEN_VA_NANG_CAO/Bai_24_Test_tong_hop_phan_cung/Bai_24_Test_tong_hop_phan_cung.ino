#include <Robot.h>

// BÀI 24: KIỂM TRA TỔNG HỢP CÁC PHẦN CỨNG ĐÃ ĐẤU NỐI
// Trình tự: OLED -> servo/NeoPixel -> động cơ/encoder -> khoảng cách
// -> line -> màu. Bluetooth và nút nhấn KHÔNG kiểm tra vì chưa đấu nối.
// CẢNH BÁO: kê hai bánh khỏi mặt đất trước lần thử đầu tiên.

const int PWM_THU_DONG_CO = 100;

void hienThiTrang(const char *tieuDe, const char *dong1,
                  const char *dong2 = "", const char *dong3 = "") {
  oledThongBao(tieuDe, dong1, dong2, dong3);
  Serial.print(F("[")); Serial.print(tieuDe); Serial.print(F("] "));
  Serial.print(dong1); Serial.print(F(" | "));
  Serial.print(dong2); Serial.print(F(" | ")); Serial.println(dong3);
}

void thuServoVaNeoPixel() {
  servoBatDau(90); // Chỉ phát xung servo trong giai đoạn thử.
  Serial.println(F("=== 1. SERVO + NEOPIXEL ==="));
  datGocServo(45); datMauNeoPixel(255, 0, 0);
  hienThiTrang("23 - SERVO + LED", "SERVO 45 DO", "NEOPIXEL DO", "");
  delay(900);
  datGocServo(90); datMauNeoPixel(0, 255, 0);
  hienThiTrang("23 - SERVO + LED", "SERVO 90 DO", "NEOPIXEL XANH LA", "");
  delay(900);
  datGocServo(135); datMauNeoPixel(0, 0, 255);
  hienThiTrang("23 - SERVO + LED", "SERVO 135 DO", "NEOPIXEL XANH", "");
  delay(900);
  datGocServo(90);
  delay(250);
  servoNgatTinHieu(); // Ngắt xung để servo không rung khi thử phần khác.
  tatNeoPixel();
}

void hienThiMotor(const char *trangThai, int pwm) {
  long trai, phai;
  docHaiEncoder(trai, phai);
  oledXoa();
  oledTieuDe("23 - MOTOR + ENCODER");
  oledInChu(4, 18, trangThai);
  oledInChu(4, 34, "E1:"); oledInSo(27, 34, trai);
  oledInChu(68, 34, "E2:"); oledInSo(91, 34, phai);
  oledInChu(4, 50, "PWM:"); oledInSo(34, 50, pwm);
  oledCapNhat();
  Serial.print(trangThai); Serial.print(F(" E1/E2="));
  Serial.print(trai); Serial.print('/'); Serial.println(phai);
}

void thuMotPhaDongCo(const char *trangThai, int pwm) {
  xoaEncoder();
  dieuKhienDongCo(pwm, pwm);
  unsigned long batDauMs = millis();
  while (millis() - batDauMs < 2000UL) {
    hienThiMotor(trangThai, pwm);
    delay(250);
  }
  dungRobot();
  hienThiMotor("DUNG", 0);
  delay(700);
}

void thuDongCoVaEncoder() {
  Serial.println(F("=== 2. DONG CO + ENCODER ==="));
  thuMotPhaDongCo("CHAY TIEN", PWM_THU_DONG_CO);
  thuMotPhaDongCo("CHAY LUI", -PWM_THU_DONG_CO);
}

void thuKhoangCach() {
  Serial.println(F("=== 3. SIEU AM + HAI LASER ==="));
  unsigned long batDauMs = millis();
  while (millis() - batDauMs < 5000UL) {
    DuLieuKhoangCach d = docBaHuong();
    oledXoa();
    oledTieuDe("23 - KHOANG CACH");
    oledInChu(4, 18, "TRUOC:");
    if (d.sieuAmHopLe) oledInSoThuc(48, 18, d.truocCm);
    else oledInChu(48, 18, "NO ECHO");
    oledInChu(4, 34, "TRAI:");
    if (d.laserTraiHopLe) oledInSo(45, 34, d.traiMm); else oledInChu(45, 34, "LOI");
    oledInChu(4, 50, "PHAI:");
    if (d.laserPhaiHopLe) oledInSo(45, 50, d.phaiMm); else oledInChu(45, 50, "LOI");
    oledCapNhat();

    Serial.print(F("F/L/R=")); Serial.print(d.truocCm); Serial.print('/');
    Serial.print(d.traiMm); Serial.print('/'); Serial.println(d.phaiMm);
    delay(250);
  }
}

void thuCamBienLine() {
  Serial.println(F("=== 4. NAM CAM BIEN LINE ==="));
  unsigned long batDauMs = millis();
  while (millis() - batDauMs < 5000UL) {
    DuLieuLine line = docLine();
    oledXoa();
    oledTieuDe("23 - CAM BIEN LINE");
    oledInChu(0, 18, "A1 A2 A3 A4 A5");
    for (uint8_t i = 0; i < 5; i++) oledInSo(i * 25, 33, line.giaTri[i]);
    oledInChu(4, 50, "VI TRI:"); oledInSo(52, 50, line.viTri);
    oledCapNhat();

    Serial.print(F("A1..A5="));
    for (uint8_t i = 0; i < 5; i++) {
      Serial.print(line.giaTri[i]); Serial.print(i < 4 ? ',' : ' ');
    }
    Serial.print(F("Vi tri=")); Serial.println(line.viTri);
    delay(250);
  }
}

void thuCamBienMau() {
  Serial.println(F("=== 5. CAM BIEN MAU TCS3200 ==="));
  unsigned long batDauMs = millis();
  while (millis() - batDauMs < 5000UL) {
    DuLieuMau d = docCamBienMau();
    MauSac mau = nhanDangMau(d);
    oledXoa();
    oledTieuDe("23 - CAM BIEN MAU");
    oledInChu(4, 18, "R:"); oledInSo(18, 18, d.doPulse);
    oledInChu(66, 18, "G:"); oledInSo(80, 18, d.xanhLaPulse);
    oledInChu(4, 33, "B:"); oledInSo(18, 33, d.xanhDuongPulse);
    oledInChu(66, 33, "C:"); oledInSo(80, 33, d.clearPulse);
    oledInChu(4, 50, d.hopLe ? tenMau(mau) : "NO PULSE");
    oledCapNhat();

    Serial.print(F("R/G/B/C=")); Serial.print(d.doPulse); Serial.print('/');
    Serial.print(d.xanhLaPulse); Serial.print('/');
    Serial.print(d.xanhDuongPulse); Serial.print('/');
    Serial.print(d.clearPulse); Serial.print(F(" -> "));
    Serial.println(d.hopLe ? tenMau(mau) : "NO PULSE");
    delay(250);
  }
}

void setup() {
  batDauSerial();
  oledBatDau();
  dongCoBatDau();
  encoderBatDau();
  servoNgatTinHieu();
  neoPixelBatDau(100);
  sieuAmBatDau();
  laserBatDau();
  camBienMauBatDau();

  hienThiTrang("BAI 24 - TONG HOP", "KE HAI BANH XE",
               "BT + NUT: CHUA NOI", "CHO 3 GIAY");
  Serial.println(F("Bluetooth va nut nhan CHUA DAU NOI, KHONG KIEM TRA."));
  delay(3000);
}

void loop() {
  static unsigned int soChuKy = 1;
  Serial.print(F("========== CHU KY ")); Serial.print(soChuKy);
  Serial.println(F(" =========="));

  thuServoVaNeoPixel();
  thuDongCoVaEncoder();
  thuKhoangCach();
  thuCamBienLine();
  thuCamBienMau();

  dungRobot();
  servoNgatTinHieu();
  tatNeoPixel();
  hienThiTrang("HOAN THANH BAI 24", "BT + NUT: CHUA NOI",
               "XEM SERIAL NEU LOI", "LAP LAI SAU 5 GIAY");
  delay(5000);
  soChuKy++;
}
