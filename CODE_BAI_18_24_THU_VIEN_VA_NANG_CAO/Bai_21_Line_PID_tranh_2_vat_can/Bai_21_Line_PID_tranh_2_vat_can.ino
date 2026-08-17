#include <Robot.h>

// BÀI 21: BÁM LINE PID VÀ TRÁNH HAI VẬT CẢN
// Robot bám line, đi vòng khi HC-SR04 phát hiện vật cản trên vạch.
// Sau khi vượt thành công vật cản thứ hai, robot coi như hết khu vực line.

BoDieuKhienPID pidLine;
const float NGUONG_VAT_CAN_CM = 22.0f;
const uint8_t SO_VAT_CAN_DE_KET_THUC = 2;
const int PWM_BAM_LINE = 115;
const int PWM_TOI_DA = 200;
const int PWM_TIM_LINE = 80;

float khoangCachTruocCm = -1.0f;
uint8_t soVatCanDaVuot = 0;
bool daHoanThanh = false;
bool coLoi = false;
unsigned long lanDocSieuAmMs = 0;
unsigned long boQuaVatCanDenMs = 0;
unsigned long lanHienThiMs = 0;

void hienThiLine(const char *trangThai) {
  DuLieuLine line = docLine();
  oledXoa();
  oledTieuDe("BAI 21 - LINE + VAT");
  oledInChu(4, 17, trangThai);
  oledInChu(4, 31, "LINE:"); oledInSo(38, 31, line.viTri);
  oledInChu(4, 44, "KC:");
  if (khoangCachTruocCm > 0) oledInSoThuc(27, 44, khoangCachTruocCm);
  else oledInChu(27, 44, "NO ECHO");
  oledInChu(73, 44, "VAT:"); oledInSo(99, 44, soVatCanDaVuot);
  oledInChu(112, 44, "/2");
  oledCapNhat();

  Serial.print(trangThai); Serial.print(F("  line="));
  Serial.print(line.viTri); Serial.print(F("  truoc="));
  Serial.print(khoangCachTruocCm); Serial.print(F("cm  vat="));
  Serial.print(soVatCanDaVuot); Serial.println(F("/2"));
}

bool tranhMotVatCan() {
  dungRobot();
  hienThiLine("BAT DAU DI VONG");
  delay(150);

  // Quỹ đạo hình chữ nhật đi vòng về bên phải vật cản.
  diLui(90); delay(250); dungRobot();
  quayPhaiTheoThoiGian(110, 450);
  diThang(100); delay(650); dungRobot();
  quayTraiTheoThoiGian(110, 450);
  diThang(100); delay(900); dungRobot();
  quayTraiTheoThoiGian(110, 450);
  diThang(90); delay(600); dungRobot();
  quayPhaiTheoThoiGian(110, 450);

  // Quay chậm có giới hạn thời gian để tìm lại vạch.
  bool thayLine = timLaiVach(1, PWM_TIM_LINE, 3500UL);
  pidXoa(pidLine);
  return thayLine;
}

void setup() {
  batDauSerial();
  oledBatDau();
  dongCoBatDau();
  sieuAmBatDau();
  pidCauHinh(pidLine, 0.45f, 0.02f, 0.003f, 500.0f);
  oledThongBao("BAI 21", "LINE PID + 2 VAT",
               "DAT ROBOT DAU LINE", "CHO 2 GIAY");
  delay(2000);
}

void loop() {
  if (daHoanThanh || coLoi) {
    dungRobot();
    return;
  }

  unsigned long hienTaiMs = millis();
  if (hienTaiMs - lanDocSieuAmMs >= 80) {
    docSieuAm(khoangCachTruocCm);
    lanDocSieuAmMs = hienTaiMs;
  }

  bool duocPhepPhatHien = hienTaiMs >= boQuaVatCanDenMs;
  bool coVatCan = duocPhepPhatHien && khoangCachTruocCm > 0 &&
                  khoangCachTruocCm <= NGUONG_VAT_CAN_CM;

  if (coVatCan) {
    if (!tranhMotVatCan()) {
      coLoi = true;
      hienThiLine("LOI - MAT LINE");
      Serial.println(F("Dung an toan vi khong tim lai duoc line."));
      return;
    }

    soVatCanDaVuot++;
    boQuaVatCanDenMs = millis() + 1200UL;
    hienThiLine("DA VUOT VAT CAN");
    delay(400);

    if (soVatCanDaVuot >= SO_VAT_CAN_DE_KET_THUC) {
      daHoanThanh = true;
      dungRobot();
      oledThongBao("BAI 21", "DA VUOT VAT CAN 2",
                   "HET KHU VUC LINE", "DUNG ROBOT");
      Serial.println(F("Da vuot vat can thu hai: ket thuc khu vuc line."));
      return;
    }
  } else {
    bamLinePIDMotBuoc(pidLine, PWM_BAM_LINE, PWM_TOI_DA, PWM_TIM_LINE);
  }

  if (millis() - lanHienThiMs >= 200) {
    hienThiLine("DANG BAM LINE");
    lanHienThiMs = millis();
  }
  delay(10);
}
