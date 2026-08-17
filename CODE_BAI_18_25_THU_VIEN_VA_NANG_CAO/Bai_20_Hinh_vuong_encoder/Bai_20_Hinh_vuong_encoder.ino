#include <Robot.h>

// BÀI 20: ROBOT DI CHUYỂN HÌNH VUÔNG BẰNG ENCODER
// Robot đi bốn cạnh bằng số xung encoder, sau mỗi cạnh quay phải 90 độ.
// Hai hằng SO_XUNG_MOT_CANH và SO_XUNG_QUAY_90 phải đo lại trên robot thật.

const long SO_XUNG_MOT_CANH = 300;
const long SO_XUNG_QUAY_90 = 82;
const int PWM_DI = 105;
const int PWM_QUAY = 105;
const unsigned long TIMEOUT_DI_MS = 7000UL;
const unsigned long TIMEOUT_QUAY_MS = 3500UL;

bool daChayXong = false;
bool coLoi = false;

void hienThiHinhVuong(const char *trangThai, uint8_t canh) {
  long trai, phai;
  docHaiEncoder(trai, phai);

  oledXoa();
  oledTieuDe("BAI 20 - HINH VUONG");
  oledInChu(4, 17, trangThai);
  oledInChu(4, 31, "CANH:"); oledInSo(39, 31, canh);
  oledInChu(55, 31, "/4");
  oledInChu(4, 46, "E T/P:");
  oledInSo(45, 46, trai); oledInSo(86, 46, phai);
  oledCapNhat();

  Serial.print(F("Canh=")); Serial.print(canh); Serial.print(F("/4  "));
  Serial.print(trangThai); Serial.print(F("  Encoder T/P="));
  Serial.print(trai); Serial.print('/'); Serial.println(phai);
}

void baoLoi(const char *noiDung, uint8_t canh) {
  dungRobot();
  coLoi = true;
  hienThiHinhVuong(noiDung, canh);
  Serial.println(F("Robot dung an toan. Kiem tra encoder hoac vat can."));
}

void setup() {
  batDauSerial();
  oledBatDau();
  dongCoBatDau();
  encoderBatDau();
  oledThongBao("BAI 20", "HINH VUONG ENCODER",
               "DAT ROBOT LEN SAN", "CHO 2 GIAY");
  delay(2000);
}

void loop() {
  if (daChayXong || coLoi) {
    dungRobot();
    return;
  }

  for (uint8_t canh = 1; canh <= 4; canh++) {
    hienThiHinhVuong("DI MOT CANH", canh);
    if (!diTheoSoXung(SO_XUNG_MOT_CANH, PWM_DI, TIMEOUT_DI_MS)) {
      baoLoi("LOI DI THANG", canh);
      return;
    }
    hienThiHinhVuong("DUNG GIAM TRUOT", canh);
    delay(350);

    hienThiHinhVuong("QUAY PHAI 90", canh);
    if (!quayPhaiTheoSoXung(SO_XUNG_QUAY_90, PWM_QUAY,
                            TIMEOUT_QUAY_MS)) {
      baoLoi("LOI QUAY 90", canh);
      return;
    }
    delay(350);
  }

  dungRobot();
  daChayXong = true;
  hienThiHinhVuong("HOAN THANH", 4);
  Serial.println(F("Da di xong hinh vuong. Nhan RESET de chay lai."));
}
