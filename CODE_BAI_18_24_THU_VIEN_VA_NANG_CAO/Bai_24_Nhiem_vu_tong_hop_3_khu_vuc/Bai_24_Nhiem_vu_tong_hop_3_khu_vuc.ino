#include <Robot.h>

// BÀI 24: NHIỆM VỤ TỔNG HỢP BA KHU VỰC
// Khu vực 1: đi qua mê cung bằng siêu âm và hai cảm biến laser.
// Khu vực 2: bám line PID, tránh vật cản rồi tìm lại line.
// Khu vực 3: tìm đúng khối màu yêu cầu và đẩy khối ra khỏi vị trí.
//
// Quy ước sa hình:
// - Sau 5 lần siêu âm phát hiện vật cản, coi như đã ra khỏi mê cung.
// - Khu vực line có 2 vật cản; vượt xong vật cản thứ 2 thì hết khu vực.
// - Các khối màu đặt tại những vị trí kiểm tra xác định.

enum KhuVuc {
  KHOI_DONG,
  KHU_VUC_ME_CUNG,
  KHU_VUC_LINE,
  KHU_VUC_KHOI_MAU,
  NHIEM_VU_HOAN_THANH,
  NHIEM_VU_LOI
};

enum TrangThaiKhoiMau {
  TIM_KHOI,
  TIEP_CAN_KHOI,
  DOC_MAU_KHOI,
  BO_QUA_KHOI,
  DAY_KHOI
};

// Các thông số dưới đây cần được hiệu chỉnh theo mặt sân và điện áp pin.
const float NGUONG_VAT_CAN_ME_CUNG_CM = 25.0f;
const float NGUONG_VAT_CAN_LINE_CM = 22.0f;
const uint8_t SO_VAT_CAN_LINE_DE_THOAT = 2;
const uint16_t NGUONG_NGO_CUT_MM = 180;
const uint16_t KHOANG_CACH_MO_MM = 2000;
const uint8_t SO_LAN_VAT_CAN_DE_THOAT = 5;
const int PWM_ME_CUNG = 110;
const int PWM_LINE = 120;
const int PWM_QUAY = 120;
const int PWM_LUI_ME_CUNG = 100;
const unsigned long THOI_GIAN_QUAY_90_MS = 520;
const unsigned long THOI_GIAN_QUAY_180_MS = 1050;
const bool MAT_ECHO_XEM_LA_TRONG = true;
const MauSac MAU_YEU_CAU = MAU_XANH_DUONG;

KhuVuc khuVuc = KHOI_DONG;
TrangThaiKhoiMau trangThaiKhoi = TIM_KHOI;
BoDieuKhienPID pidLine;

DuLieuKhoangCach khoangCach = {-1.0f, 0, 0, false, false, false};
float khoangCachKhoiCm = -1.0f;
MauSac mauVuaDoc = MAU_KHONG_XAC_DINH;

unsigned long lanDocSieuAmMs = 0;
unsigned long lanHienThiMs = 0;
uint8_t soLanGapVatCanMeCung = 0;
uint8_t soVatCanLineDaVuot = 0;
bool lanTruocQuayPhai = false;

const char *tenKhuVuc() {
  switch (khuVuc) {
    case KHOI_DONG: return "KHOI DONG";
    case KHU_VUC_ME_CUNG: return "1: ME CUNG";
    case KHU_VUC_LINE: return "2: LINE";
    case KHU_VUC_KHOI_MAU: return "3: KHOI MAU";
    case NHIEM_VU_HOAN_THANH: return "HOAN THANH";
    default: return "BAO LOI";
  }
}

void hienThiTongHop(const char *trangThai) {
  oledXoa();
  oledTieuDe("BAI 24 - TONG HOP");
  oledInChu(4, 17, tenKhuVuc());
  oledInChu(4, 30, trangThai);

  if (khuVuc == KHU_VUC_ME_CUNG) {
    oledInChu(4, 42, "F:");
    if (khoangCach.sieuAmHopLe) oledInSoThuc(18, 42, khoangCach.truocCm);
    else oledInChu(18, 42, "NO ECHO");
    oledInChu(62, 42, "L:"); oledInSo(76, 42, khoangCach.traiMm);
    oledInChu(4, 54, "R:"); oledInSo(18, 54, khoangCach.phaiMm);
    oledInChu(63, 54, "LAN:"); oledInSo(91, 54, soLanGapVatCanMeCung);
    oledInChu(103, 54, "/5");
  } else if (khuVuc == KHU_VUC_LINE) {
    DuLieuLine line = docLine();
    oledInChu(4, 43, "LINE:"); oledInSo(39, 43, line.viTri);
    oledInChu(72, 43, "D:"); oledInSoThuc(86, 43, khoangCachKhoiCm);
    oledInChu(4, 55, "VAT CAN:"); oledInSo(58, 55, soVatCanLineDaVuot);
    oledInChu(69, 55, "/2");
  } else if (khuVuc == KHU_VUC_KHOI_MAU) {
    oledInChu(4, 46, "D:"); oledInSoThuc(18, 46, khoangCachKhoiCm);
    oledInChu(62, 46, "M:"); oledInChu(76, 46, tenMau(mauVuaDoc));
  }
  oledCapNhat();

  Serial.print(F("Khu vuc=")); Serial.print(tenKhuVuc());
  Serial.print(F("  Trang thai=")); Serial.println(trangThai);
}

void chuyenKhuVuc(KhuVuc khuVucMoi, const char *thongBao) {
  dungRobot();
  khuVuc = khuVucMoi;

  // Với trạng thái cuối, chỉ đặt màu LED và ngắt servo đúng một lần.
  // Không gửi lại NeoPixel liên tục vì thư viện NeoPixel tạm khóa ngắt,
  // có thể làm xung servo bị nhiễu và khiến servo rung giật.
  if (khuVucMoi == NHIEM_VU_LOI) {
    servoNgatTinHieu();
    datMauNeoPixel(100, 0, 0);
  } else if (khuVucMoi == NHIEM_VU_HOAN_THANH) {
    servoNgatTinHieu();
    datMauNeoPixel(0, 100, 0);
  }

  Serial.print(F("Chuyen sang "));
  Serial.print(tenKhuVuc());
  Serial.print(F(": "));
  Serial.println(thongBao);
  oledThongBao("BAI 24 - CHUYEN VUNG", tenKhuVuc(), thongBao, "");
  delay(800);
}

void quayTraiMeCung() {
  hienThiTongHop("QUAY TRAI");
  quayTraiTheoThoiGian(PWM_QUAY, THOI_GIAN_QUAY_90_MS);
  lanTruocQuayPhai = false;
}

void quayPhaiMeCung() {
  hienThiTongHop("QUAY PHAI");
  quayPhaiTheoThoiGian(PWM_QUAY, THOI_GIAN_QUAY_90_MS);
  lanTruocQuayPhai = true;
}

void thoatNgoCutMeCung() {
  hienThiTongHop("NGO CUT - DANG LUI");
  diLui(PWM_LUI_ME_CUNG);
  delay(300);
  dungRobot();
  delay(100);
  hienThiTongHop("NGO CUT - QUAY 180");
  quayPhaiTheoThoiGian(PWM_QUAY, THOI_GIAN_QUAY_180_MS);
  lanTruocQuayPhai = true;
}

void xuLyMotLanVatCanMeCung() {
  // Mỗi lần gọi là một lần siêu âm xác nhận vật cản phía trước.
  soLanGapVatCanMeCung++;
  dungRobot();
  hienThiTongHop("VAT CAN PHIA TRUOC");
  delay(150);

  // Laser đã khởi tạo nhưng ngoài tầm đo được quy ước là 2000 mm,
  // tức phía đó đang rất thoáng như trong bài 11 gốc.
  bool dungDuocLaserTrai = khoangCach.laserTraiHopLe || laserTraiDaSanSang();
  bool dungDuocLaserPhai = khoangCach.laserPhaiHopLe || laserPhaiDaSanSang();
  uint16_t traiDeChon = khoangCach.laserTraiHopLe
      ? khoangCach.traiMm : KHOANG_CACH_MO_MM;
  uint16_t phaiDeChon = khoangCach.laserPhaiHopLe
      ? khoangCach.phaiMm : KHOANG_CACH_MO_MM;

  if (dungDuocLaserTrai && dungDuocLaserPhai &&
      traiDeChon < NGUONG_NGO_CUT_MM && phaiDeChon < NGUONG_NGO_CUT_MM) {
    thoatNgoCutMeCung();
  } else if (dungDuocLaserTrai && dungDuocLaserPhai) {
    if (traiDeChon > phaiDeChon + 40) quayTraiMeCung();
    else if (phaiDeChon > traiDeChon + 40) quayPhaiMeCung();
    else if (lanTruocQuayPhai) quayTraiMeCung();
    else quayPhaiMeCung();
  } else if (dungDuocLaserTrai) {
    quayTraiMeCung();
  } else if (dungDuocLaserPhai) {
    quayPhaiMeCung();
  } else {
    chuyenKhuVuc(NHIEM_VU_LOI, "LOI HAI LASER");
    return;
  }

  if (soLanGapVatCanMeCung >= SO_LAN_VAT_CAN_DE_THOAT) {
    chuyenKhuVuc(KHU_VUC_LINE, "DU 5 VAT CAN - BAM LINE");
  }
}

void xuLyMeCungMotBuoc() {
  khoangCach = docBaHuong();
  bool coVatCan = khoangCach.sieuAmHopLe &&
                  khoangCach.truocCm <= NGUONG_VAT_CAN_ME_CUNG_CM;

  if (coVatCan) {
    xuLyMotLanVatCanMeCung();
  } else if (!khoangCach.sieuAmHopLe && !MAT_ECHO_XEM_LA_TRONG) {
    dungRobot();
    hienThiTongHop("MAT ECHO SIEU AM");
  } else {
    diThang(PWM_ME_CUNG);
    hienThiTongHop("DI THANG");
  }
}

bool tranhVatCanVaTimLaiLine() {
  dungRobot();
  hienThiTongHop("TRANH VAT CAN");
  delay(150);

  diLui(90); delay(300); dungRobot();
  quayPhaiTheoThoiGian(110, 450);
  diThang(100); delay(650); dungRobot();
  quayTraiTheoThoiGian(110, 450);
  diThang(90); delay(450); dungRobot();

  bool thayLaiVach = timLaiVach(1, 80, 3500);
  pidXoa(pidLine);
  return thayLaiVach;
}

void xuLyLineMotBuoc() {
  unsigned long hienTaiMs = millis();
  if (hienTaiMs - lanDocSieuAmMs >= 80) {
    docSieuAm(khoangCachKhoiCm);
    lanDocSieuAmMs = hienTaiMs;
  }

  if (khoangCachKhoiCm > 0 && khoangCachKhoiCm < NGUONG_VAT_CAN_LINE_CM) {
    if (!tranhVatCanVaTimLaiLine()) {
      chuyenKhuVuc(NHIEM_VU_LOI, "MAT LINE SAU VAT CAN");
      return;
    }

    // Chỉ tính khi robot đã đi vòng và tìm lại được line thành công.
    soVatCanLineDaVuot++;
    Serial.print(F("Da vuot vat can line so "));
    Serial.print(soVatCanLineDaVuot);
    Serial.print('/'); Serial.println(SO_VAT_CAN_LINE_DE_THOAT);
    hienThiTongHop("DA VUOT VAT CAN LINE");
    delay(300);

    if (soVatCanLineDaVuot >= SO_VAT_CAN_LINE_DE_THOAT) {
      trangThaiKhoi = TIM_KHOI;
      mauVuaDoc = MAU_KHONG_XAC_DINH;
      chuyenKhuVuc(KHU_VUC_KHOI_MAU, "DA VUOT 2 VAT CAN");
    }
  } else {
    bamLinePIDMotBuoc(pidLine, PWM_LINE, 200, 80);
  }
}

DuLieuMau docMauTrungBinh(uint8_t soMau) {
  DuLieuMau trungBinh = {0, 0, 0, 0, false};
  uint8_t soMauHopLe = 0;

  for (uint8_t i = 0; i < soMau; i++) {
    DuLieuMau d = docCamBienMau();
    if (!d.hopLe) continue;
    trungBinh.doPulse += d.doPulse;
    trungBinh.xanhLaPulse += d.xanhLaPulse;
    trungBinh.xanhDuongPulse += d.xanhDuongPulse;
    trungBinh.clearPulse += d.clearPulse;
    soMauHopLe++;
  }

  if (soMauHopLe > 0) {
    trungBinh.doPulse /= soMauHopLe;
    trungBinh.xanhLaPulse /= soMauHopLe;
    trungBinh.xanhDuongPulse /= soMauHopLe;
    trungBinh.clearPulse /= soMauHopLe;
    trungBinh.hopLe = true;
  }
  return trungBinh;
}

void xuLyKhoiMauMotBuoc() {
  switch (trangThaiKhoi) {
    case TIM_KHOI:
      if (docSieuAm(khoangCachKhoiCm) && khoangCachKhoiCm < 60.0f) {
        dungRobot();
        trangThaiKhoi = TIEP_CAN_KHOI;
      } else {
        quayPhai(65);
      }
      break;

    case TIEP_CAN_KHOI:
      if (!docSieuAm(khoangCachKhoiCm) || khoangCachKhoiCm > 70.0f) {
        trangThaiKhoi = TIM_KHOI;
      } else if (khoangCachKhoiCm > 7.0f) {
        diThang(70);
      } else {
        diThang(55); delay(180); dungRobot();
        trangThaiKhoi = DOC_MAU_KHOI;
      }
      break;

    case DOC_MAU_KHOI: {
      DuLieuMau duLieu = docMauTrungBinh(2);
      mauVuaDoc = nhanDangMau(duLieu);
      trangThaiKhoi = (mauVuaDoc == MAU_YEU_CAU) ? DAY_KHOI : BO_QUA_KHOI;
      break;
    }

    case BO_QUA_KHOI:
      diLui(80); delay(450); dungRobot();
      quayPhaiTheoThoiGian(90, 550);
      mauVuaDoc = MAU_KHONG_XAC_DINH;
      trangThaiKhoi = TIM_KHOI;
      break;

    case DAY_KHOI:
      diThang(115); delay(1500); dungRobot();
      chuyenKhuVuc(NHIEM_VU_HOAN_THANH, "DA DAY DUNG KHOI");
      break;
  }
}

void setup() {
  batDauSerial();
  oledBatDau();
  dongCoBatDau();
  encoderBatDau();
  // Bài 08 hiện chưa dùng servo, vì vậy giữ chân D15 ở LOW và không phát xung.
  servoNgatTinHieu();
  neoPixelBatDau(70);
  sieuAmBatDau();
  laserBatDau();
  camBienMauBatDau();

  pidCauHinh(pidLine, 0.45f, 0.02f, 0.003f, 500.0f);
  datMauNeoPixel(0, 0, 80);
  oledThongBao("BAI 24 - TONG HOP", "DAT ROBOT TAI DAU", "CHO 2 GIAY", "");
  delay(2000);
  chuyenKhuVuc(KHU_VUC_ME_CUNG, "BAT DAU");
}

void loop() {
  switch (khuVuc) {
    case KHU_VUC_ME_CUNG:
      xuLyMeCungMotBuoc();
      break;
    case KHU_VUC_LINE:
      xuLyLineMotBuoc();
      break;
    case KHU_VUC_KHOI_MAU:
      xuLyKhoiMauMotBuoc();
      break;
    case NHIEM_VU_HOAN_THANH:
      dungRobot();
      break;
    case NHIEM_VU_LOI:
      dungRobot();
      break;
    default:
      dungRobot();
      break;
  }

  if (millis() - lanHienThiMs >= 200) {
    if (khuVuc == NHIEM_VU_HOAN_THANH) hienThiTongHop("DA HOAN THANH");
    else if (khuVuc == NHIEM_VU_LOI) hienThiTongHop("DUNG AN TOAN");
    else hienThiTongHop("DANG THUC HIEN");
    lanHienThiMs = millis();
  }
  delay(10);
}
