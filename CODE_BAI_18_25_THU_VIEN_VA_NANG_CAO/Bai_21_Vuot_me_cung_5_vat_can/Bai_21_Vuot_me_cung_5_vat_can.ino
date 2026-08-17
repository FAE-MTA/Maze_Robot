#include <Robot.h>

// BÀI 21: VƯỢT MÊ CUNG BẰNG SIÊU ÂM VÀ HAI LASER
// Thuật toán được lấy từ bài 11_Kết hợp siêu âm và laser:
// 1. Phía trước trống: robot đi thẳng.
// 2. Siêu âm thấy vật cản: dừng và đọc khoảng cách hai bên.
// 3. Robot quay về bên có khoảng trống lớn hơn.
// 4. Hai bên đều gần: lùi rồi quay 180 độ để thoát ngõ cụt.
// 5. Sau 5 lần siêu âm phát hiện vật cản, coi như đã ra khỏi mê cung.

const float NGUONG_TRUOC_CM = 25.0f;
const uint16_t NGUONG_NGO_CUT_MM = 180;
const uint16_t KHOANG_CACH_MO_MM = 2000;
const uint8_t SO_LAN_VAT_CAN_DE_THOAT = 5;
const int PWM_DI = 110;
const int PWM_QUAY = 120;
const int PWM_LUI = 100;
const unsigned long QUAY_90_MS = 520;   // Hiệu chỉnh trên robot thật.
const unsigned long QUAY_180_MS = 1050; // Hiệu chỉnh trên robot thật.
const bool MAT_ECHO_XEM_LA_TRONG = true;

uint8_t soLanGapVatCan = 0;
bool lanTruocQuayPhai = false;
bool daRaKhoiMeCung = false;

void hienThiMeCung(const char *trangThai, const DuLieuKhoangCach &d) {
  oledXoa();
  oledTieuDe("BAI 21 - ME CUNG");
  oledInChu(4, 16, trangThai);

  oledInChu(4, 30, "F:");
  if (d.sieuAmHopLe) oledInSoThuc(18, 30, d.truocCm);
  else oledInChu(18, 30, "NO ECHO");

  oledInChu(4, 43, "L:");
  if (d.laserTraiHopLe) oledInSo(18, 43, d.traiMm);
  else oledInChu(18, 43, "LOI");
  oledInChu(64, 43, "R:");
  if (d.laserPhaiHopLe) oledInSo(78, 43, d.phaiMm);
  else oledInChu(78, 43, "LOI");

  oledInChu(4, 55, "VAT CAN:");
  oledInSo(58, 55, soLanGapVatCan);
  oledInChu(69, 55, "/5");
  oledCapNhat();

  Serial.print(trangThai);
  Serial.print(F("  F="));
  if (d.sieuAmHopLe) Serial.print(d.truocCm);
  else Serial.print(F("NO_ECHO"));
  Serial.print(F("cm L=")); Serial.print(d.traiMm);
  Serial.print(F("mm R=")); Serial.print(d.phaiMm);
  Serial.print(F("mm  So lan vat can="));
  Serial.print(soLanGapVatCan);
  Serial.print('/'); Serial.println(SO_LAN_VAT_CAN_DE_THOAT);
}

void quayTrai90(const DuLieuKhoangCach &d) {
  hienThiMeCung("QUAY TRAI", d);
  quayTraiTheoThoiGian(PWM_QUAY, QUAY_90_MS);
  lanTruocQuayPhai = false;
}

void quayPhai90(const DuLieuKhoangCach &d) {
  hienThiMeCung("QUAY PHAI", d);
  quayPhaiTheoThoiGian(PWM_QUAY, QUAY_90_MS);
  lanTruocQuayPhai = true;
}

void thoatNgoCut(const DuLieuKhoangCach &d) {
  hienThiMeCung("NGO CUT - DANG LUI", d);
  diLui(PWM_LUI);
  delay(300);
  dungRobot();
  delay(100);

  hienThiMeCung("NGO CUT - QUAY 180", d);
  quayPhaiTheoThoiGian(PWM_QUAY, QUAY_180_MS);
  lanTruocQuayPhai = true;
}

void xuLyMotLanVatCan(const DuLieuKhoangCach &d) {
  // Mỗi lần vào hàm này là một lần siêu âm xác nhận vật cản mới
  // và robot thực hiện một thao tác đổi hướng.
  soLanGapVatCan++;
  dungRobot();
  hienThiMeCung("VAT CAN PHIA TRUOC", d);
  delay(150);

  // Laser đã khởi tạo nhưng báo ngoài tầm đo được xem là phía rất thoáng,
  // giống quy ước 2000 mm trong bài 11 gốc.
  bool dungDuocLaserTrai = d.laserTraiHopLe || laserTraiDaSanSang();
  bool dungDuocLaserPhai = d.laserPhaiHopLe || laserPhaiDaSanSang();
  uint16_t traiDeChon = d.laserTraiHopLe ? d.traiMm : KHOANG_CACH_MO_MM;
  uint16_t phaiDeChon = d.laserPhaiHopLe ? d.phaiMm : KHOANG_CACH_MO_MM;

  if (dungDuocLaserTrai && dungDuocLaserPhai &&
      traiDeChon < NGUONG_NGO_CUT_MM && phaiDeChon < NGUONG_NGO_CUT_MM) {
    thoatNgoCut(d);
  } else if (dungDuocLaserTrai && dungDuocLaserPhai) {
    if (traiDeChon > phaiDeChon + 40) quayTrai90(d);
    else if (phaiDeChon > traiDeChon + 40) quayPhai90(d);
    else if (lanTruocQuayPhai) quayTrai90(d);
    else quayPhai90(d);
  } else if (dungDuocLaserTrai) {
    quayTrai90(d);
  } else if (dungDuocLaserPhai) {
    quayPhai90(d);
  } else {
    dungRobot();
    hienThiMeCung("LOI CA HAI LASER", d);
  }

  if (soLanGapVatCan >= SO_LAN_VAT_CAN_DE_THOAT) {
    dungRobot();
    daRaKhoiMeCung = true;
    oledThongBao("BAI 21 - ME CUNG", "DA GAP 5 VAT CAN",
                 "COI NHU DA THOAT", "DUNG ROBOT");
    Serial.println(F("Da du 5 lan sieu am thay vat can: ket thuc me cung."));
  }
}

void setup() {
  batDauSerial();
  oledBatDau();
  dongCoBatDau();
  sieuAmBatDau();
  laserBatDau();
  oledThongBao("BAI 21 - ME CUNG", "DAT ROBOT DAU LOI",
               "THOAT SAU 5 VAT CAN", "CHO 2 GIAY");
  delay(2000);
}

void loop() {
  if (daRaKhoiMeCung) {
    dungRobot();
    return;
  }

  DuLieuKhoangCach d = docBaHuong();
  bool coVatCan = d.sieuAmHopLe && d.truocCm <= NGUONG_TRUOC_CM;

  if (coVatCan) {
    xuLyMotLanVatCan(d);
  } else if (!d.sieuAmHopLe && !MAT_ECHO_XEM_LA_TRONG) {
    dungRobot();
    hienThiMeCung("DUNG - MAT ECHO", d);
  } else {
    diThang(PWM_DI);
    hienThiMeCung("DI THANG", d);
  }
  delay(60);
}
