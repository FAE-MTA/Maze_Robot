#include <Robot.h>

// BÀI 22: NHẬN DẠNG VÀ ĐẨY KHỐI MÀU
// Giả thiết sa hình: các khối nằm tại những vị trí kiểm tra xác định.
// Robot quay tìm khối, dùng siêu âm để tiếp cận, đọc TCS3200 ở cự ly gần,
// sau đó chỉ đẩy khối có màu đúng yêu cầu.

enum TrangThaiKhoiMau {
  TIM_KHOI,
  TIEP_CAN_KHOI,
  DOC_MAU_KHOI,
  BO_QUA_KHOI,
  DAY_KHOI,
  DAY_XONG
};

const MauSac MAU_YEU_CAU = MAU_XANH_DUONG;
TrangThaiKhoiMau trangThai = TIM_KHOI;
float khoangCachKhoi = -1.0f;
MauSac mauVuaDoc = MAU_KHONG_XAC_DINH;

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

const char *tenTrangThai() {
  switch (trangThai) {
    case TIM_KHOI: return "TIM KHOI";
    case TIEP_CAN_KHOI: return "TIEP CAN";
    case DOC_MAU_KHOI: return "DOC MAU";
    case BO_QUA_KHOI: return "BO QUA KHOI";
    case DAY_KHOI: return "DAY KHOI";
    default: return "HOAN THANH";
  }
}

void hienThiKhoiMau() {
  oledXoa();
  oledTieuDe("BAI 22 - KHOI MAU");
  oledInChu(4, 18, tenTrangThai());
  oledInChu(4, 33, "KC:");
  if (khoangCachKhoi > 0) oledInSoThuc(25, 33, khoangCachKhoi);
  else oledInChu(25, 33, "NO ECHO");
  oledInChu(4, 47, "MAU:"); oledInChu(34, 47, tenMau(mauVuaDoc));
  oledCapNhat();

  Serial.print(tenTrangThai());
  Serial.print(F("  Khoang cach=")); Serial.print(khoangCachKhoi);
  Serial.print(F("  Mau=")); Serial.println(tenMau(mauVuaDoc));
}

void setup() {
  batDauSerial();
  oledBatDau();
  dongCoBatDau();
  sieuAmBatDau();
  camBienMauBatDau();
  oledThongBao("BAI 22", "MAU CAN DAY:", tenMau(MAU_YEU_CAU),
               "CHO 2 GIAY");
  delay(2000);
}

void loop() {
  switch (trangThai) {
    case TIM_KHOI:
      if (docSieuAm(khoangCachKhoi) && khoangCachKhoi < 60.0f) {
        dungRobot();
        trangThai = TIEP_CAN_KHOI;
      } else {
        quayPhai(65); // Quay chậm để quét tìm khối phía trước.
      }
      break;

    case TIEP_CAN_KHOI:
      if (!docSieuAm(khoangCachKhoi) || khoangCachKhoi > 70.0f) {
        trangThai = TIM_KHOI;
      } else if (khoangCachKhoi > 7.0f) {
        diThang(70);
      } else {
        // Tiến rất ngắn để cảm biến màu cách bề mặt khối khoảng 1–2 cm.
        diThang(55); delay(180); dungRobot();
        trangThai = DOC_MAU_KHOI;
      }
      break;

    case DOC_MAU_KHOI: {
      DuLieuMau duLieu = docMauTrungBinh(2);
      mauVuaDoc = nhanDangMau(duLieu);
      trangThai = (mauVuaDoc == MAU_YEU_CAU) ? DAY_KHOI : BO_QUA_KHOI;
      break;
    }

    case BO_QUA_KHOI:
      diLui(80); delay(450); dungRobot();
      quayPhaiTheoThoiGian(90, 550);
      mauVuaDoc = MAU_KHONG_XAC_DINH;
      trangThai = TIM_KHOI;
      break;

    case DAY_KHOI:
      diThang(115); delay(1500); dungRobot();
      trangThai = DAY_XONG;
      break;

    case DAY_XONG:
      dungRobot();
      break;
  }

  hienThiKhoiMau();
  delay(50);
}
