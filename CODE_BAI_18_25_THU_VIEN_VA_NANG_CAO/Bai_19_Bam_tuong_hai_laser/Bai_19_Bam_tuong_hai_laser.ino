#include <Robot.h>

// BÀI 19: ROBOT BÁM TƯỜNG BẰNG LASER
// Hai laser đặt bên hông; siêu âm phía trước dùng để tránh va chạm.

const bool BAM_TUONG_TRAI = true;  // Đổi false để bám tường phải.
const uint16_t KHOANG_CACH_DAT_MM = 180;
const int PWM_CO_BAN = 120;
const int PWM_QUAY = 125;
const float KP_TUONG = 0.35f;
const float NGUONG_TRUOC_CM = 23.0f;

void hienThiBamTuong(const char *trangThai, const DuLieuKhoangCach &d) {
  oledXoa();
  oledTieuDe("BAI 19 - BAM TUONG");
  oledInChu(4, 16, trangThai);
  oledInChu(4, 30, "F:");
  if (d.sieuAmHopLe) oledInSoThuc(18, 30, d.truocCm); else oledInChu(18, 30, "NO ECHO");
  oledInChu(4, 46, "L:");
  if (d.laserTraiHopLe) oledInSo(18, 46, d.traiMm); else oledInChu(18, 46, "LOI");
  oledInChu(65, 46, "R:");
  if (d.laserPhaiHopLe) oledInSo(79, 46, d.phaiMm); else oledInChu(79, 46, "LOI");
  oledCapNhat();

  Serial.print(trangThai); Serial.print(F(" F/L/R="));
  Serial.print(d.truocCm); Serial.print('/'); Serial.print(d.traiMm);
  Serial.print('/'); Serial.println(d.phaiMm);
}

void tranhVatCanTruoc(const DuLieuKhoangCach &d) {
  dungRobot();
  hienThiBamTuong("VAT CAN PHIA TRUOC", d);
  delay(120);
  diLui(105); delay(300); dungRobot();
  if (BAM_TUONG_TRAI) quayPhaiTheoThoiGian(PWM_QUAY, 480);
  else quayTraiTheoThoiGian(PWM_QUAY, 480);
}

void setup() {
  batDauSerial();
  oledBatDau();
  dongCoBatDau();
  sieuAmBatDau();
  laserBatDau();
  oledThongBao("BAI 19 - BAM TUONG",
               BAM_TUONG_TRAI ? "BAM BEN TRAI" : "BAM BEN PHAI",
               "CHO 1 GIAY", "");
  delay(1000);
}

void loop() {
  DuLieuKhoangCach d = docBaHuong();
  if (!d.sieuAmHopLe) {
    dungRobot();
    hienThiBamTuong("DUNG - MAT ECHO", d);
  } else if (d.truocCm <= NGUONG_TRUOC_CM) {
    tranhVatCanTruoc(d);
  } else {
    bool thayTuong = bamTuongMotBuoc(BAM_TUONG_TRAI,
                                     KHOANG_CACH_DAT_MM,
                                     PWM_CO_BAN, KP_TUONG);
    hienThiBamTuong(thayTuong ? "DANG BAM TUONG" : "DANG TIM TUONG", d);
  }
  delay(80);
}
