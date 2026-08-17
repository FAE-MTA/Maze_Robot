#include <Robot.h>

// BÀI 18: LÀM QUEN VỚI THƯ VIỆN ROBOTMEGA2560MINI
// Chương trình nhận lệnh từ Serial Monitor để minh họa cách gọi hàm.
// Kê hai bánh khỏi mặt đất trong lần thử đầu tiên.
// Lệnh: T = tiến, L = lùi, A = quay trái, D = quay phải, S = dừng.

const int PWM_THU = 90;
const unsigned long THOI_GIAN_THU_MS = 500UL;

void hienThiLenh(const char *trangThai, int pwmTrai, int pwmPhai) {
  oledXoa();
  oledTieuDe("BAI 18 - THU VIEN");
  oledInChu(4, 18, trangThai);
  oledInChu(4, 34, "PWM T/P:");
  oledInSo(58, 34, pwmTrai);
  oledInSo(92, 34, pwmPhai);
  oledInChu(4, 50, "T L A D S");
  oledCapNhat();

  Serial.print(F("Trang thai=")); Serial.print(trangThai);
  Serial.print(F("  PWM trai/phai="));
  Serial.print(pwmTrai); Serial.print('/'); Serial.println(pwmPhai);
}

void chayThu(const char *trangThai, int pwmTrai, int pwmPhai) {
  dieuKhienDongCo(pwmTrai, pwmPhai);
  hienThiLenh(trangThai, pwmTrai, pwmPhai);
  delay(THOI_GIAN_THU_MS);
  dungRobot();
  hienThiLenh("DA DUNG", 0, 0);
}

void inHuongDan() {
  Serial.println(F("=== LAM QUEN THU VIEN ROBOT ==="));
  Serial.println(F("Gui T: tien  | L: lui | A: trai | D: phai | S: dung"));
}

void setup() {
  // Chỉ khởi tạo những phần cứng chương trình đang dùng.
  batDauSerial();
  oledBatDau();
  dongCoBatDau();

  // Bài này không dùng servo: giữ chân D15 ở LOW để servo không rung.
  servoNgatTinHieu();
  dungRobot();
  hienThiLenh("THU VIEN SAN SANG", 0, 0);
  inHuongDan();
}

void loop() {
  if (Serial.available() == 0) return;

  char lenh = Serial.read();
  if (lenh >= 'a' && lenh <= 'z') lenh -= 32;

  switch (lenh) {
    case 'T': chayThu("DI THANG", PWM_THU, PWM_THU); break;
    case 'L': chayThu("DI LUI", -PWM_THU, -PWM_THU); break;
    case 'A': chayThu("QUAY TRAI", -PWM_THU, PWM_THU); break;
    case 'D': chayThu("QUAY PHAI", PWM_THU, -PWM_THU); break;
    case 'S':
      dungRobot();
      hienThiLenh("DUNG THEO LENH", 0, 0);
      break;
    case '\n':
    case '\r':
      break; // Bỏ qua ký tự xuống dòng của Serial Monitor.
    default:
      Serial.println(F("Lenh khong hop le. Hay dung T, L, A, D hoac S."));
      break;
  }
}
