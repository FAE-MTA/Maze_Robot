#include "Robot.h"

#include <Wire.h>
#include <Adafruit_VL53L0X.h>

namespace {
  Adafruit_VL53L0X laserTrai;
  Adafruit_VL53L0X laserPhai;
  bool laserTraiReady = false;
  bool laserPhaiReady = false;
  bool laserDaKhoiTao = false;
  bool mauReady = false;

  bool docMotLaser(Adafruit_VL53L0X &laser, bool ready,
                   uint16_t &khoangCachMm) {
    if (!ready) {
      khoangCachMm = 0;
      return false;
    }

    VL53L0X_RangingMeasurementData_t ketQua;
    laser.rangingTest(&ketQua, false);
    if (ketQua.RangeStatus != 0) {
      khoangCachMm = 0;
      return false;
    }

    khoangCachMm = ketQua.RangeMilliMeter;
    return true;
  }

  unsigned long docXungMau(uint8_t s2, uint8_t s3,
                           unsigned long timeoutNhanhUs,
                           unsigned long timeoutChamUs) {
    digitalWrite(RobotPins::COLOR_S2, s2);
    digitalWrite(RobotPins::COLOR_S3, s3);
    delay(3);

    unsigned long xung = pulseIn(RobotPins::COLOR_OUT, LOW,
                                 timeoutNhanhUs);
    if (xung == 0) {
      xung = pulseIn(RobotPins::COLOR_OUT, LOW, timeoutChamUs);
    }
    return xung;
  }
}

void sieuAmBatDau() {
  pinMode(RobotPins::ULTRASONIC_TRIG, OUTPUT);
  pinMode(RobotPins::ULTRASONIC_ECHO, INPUT);
  digitalWrite(RobotPins::ULTRASONIC_TRIG, LOW);
}

bool docSieuAm(float &khoangCachCm, unsigned long timeoutUs) {
  digitalWrite(RobotPins::ULTRASONIC_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(RobotPins::ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(RobotPins::ULTRASONIC_TRIG, LOW);

  unsigned long thoiGianEcho = pulseIn(RobotPins::ULTRASONIC_ECHO,
                                       HIGH, timeoutUs);
  if (thoiGianEcho == 0) {
    khoangCachCm = -1.0f;
    return false;
  }

  khoangCachCm = thoiGianEcho * 0.0343f / 2.0f;
  return true;
}

bool laserBatDau() {
  if (laserDaKhoiTao) return laserTraiReady || laserPhaiReady;

  Wire.begin();
  pinMode(RobotPins::LASER_LEFT_XSHUT, OUTPUT);
  pinMode(RobotPins::LASER_RIGHT_XSHUT, OUTPUT);
  digitalWrite(RobotPins::LASER_LEFT_XSHUT, LOW);
  digitalWrite(RobotPins::LASER_RIGHT_XSHUT, LOW);
  delay(10);

  digitalWrite(RobotPins::LASER_LEFT_XSHUT, HIGH);
  delay(10);
  laserTraiReady = laserTrai.begin(RobotPins::LASER_LEFT_ADDRESS);
  if (!laserTraiReady) {
    // Tắt cảm biến lỗi để không giữ địa chỉ I2C mặc định 0x29.
    digitalWrite(RobotPins::LASER_LEFT_XSHUT, LOW);
  }

  digitalWrite(RobotPins::LASER_RIGHT_XSHUT, HIGH);
  delay(10);
  laserPhaiReady = laserPhai.begin(RobotPins::LASER_RIGHT_ADDRESS);
  if (!laserPhaiReady) {
    digitalWrite(RobotPins::LASER_RIGHT_XSHUT, LOW);
  }

  laserDaKhoiTao = true;
  return laserTraiReady || laserPhaiReady;
}

bool laserTraiDaSanSang() {
  return laserTraiReady;
}

bool laserPhaiDaSanSang() {
  return laserPhaiReady;
}

bool docLaserTrai(uint16_t &khoangCachMm) {
  return docMotLaser(laserTrai, laserTraiReady, khoangCachMm);
}

bool docLaserPhai(uint16_t &khoangCachMm) {
  return docMotLaser(laserPhai, laserPhaiReady, khoangCachMm);
}

DuLieuKhoangCach docBaHuong() {
  DuLieuKhoangCach duLieu;
  duLieu.sieuAmHopLe = docSieuAm(duLieu.truocCm);
  duLieu.laserTraiHopLe = docLaserTrai(duLieu.traiMm);
  duLieu.laserPhaiHopLe = docLaserPhai(duLieu.phaiMm);
  return duLieu;
}

void docLineRaw(int giaTri[5]) {
  for (uint8_t i = 0; i < 5; i++) {
    giaTri[i] = analogRead(RobotPins::LINE[i]);
  }
}

DuLieuLine docLine(const int nguong[5], bool vachDenChoGiaTriThap) {
  DuLieuLine duLieu;
  docLineRaw(duLieu.giaTri);

  long tongTrongSo = 0;
  duLieu.tongDoManh = 0;
  for (uint8_t i = 0; i < 5; i++) {
    int doManh = vachDenChoGiaTriThap
        ? nguong[i] - duLieu.giaTri[i]
        : duLieu.giaTri[i] - nguong[i];
    if (doManh < 0) doManh = 0;
    tongTrongSo += (long)doManh * RobotDefaults::LINE_WEIGHT[i];
    duLieu.tongDoManh += doManh;
  }

  duLieu.thayVach = duLieu.tongDoManh >= RobotDefaults::LINE_MIN_STRENGTH;
  duLieu.viTri = duLieu.thayVach
      ? (int)(tongTrongSo / duLieu.tongDoManh)
      : 0;
  return duLieu;
}

bool tatCaMatTrenVach(const int nguong[5], bool vachDenChoGiaTriThap) {
  int giaTri[5];
  docLineRaw(giaTri);
  for (uint8_t i = 0; i < 5; i++) {
    bool trenVach = vachDenChoGiaTriThap
        ? giaTri[i] < nguong[i]
        : giaTri[i] > nguong[i];
    if (!trenVach) return false;
  }
  return true;
}

void camBienMauBatDau() {
  pinMode(RobotPins::COLOR_S0, OUTPUT);
  pinMode(RobotPins::COLOR_S1, OUTPUT);
  pinMode(RobotPins::COLOR_S2, OUTPUT);
  pinMode(RobotPins::COLOR_S3, OUTPUT);
  pinMode(RobotPins::COLOR_LED, OUTPUT);
  pinMode(RobotPins::COLOR_OUT, INPUT);

  // Tần số đầu ra 100% giúp phát hiện được cả kênh màu yếu.
  digitalWrite(RobotPins::COLOR_S0, HIGH);
  digitalWrite(RobotPins::COLOR_S1, HIGH);
  digitalWrite(RobotPins::COLOR_S2, LOW);
  digitalWrite(RobotPins::COLOR_S3, LOW);
  digitalWrite(RobotPins::COLOR_LED, HIGH);
  mauReady = true;
}

DuLieuMau docCamBienMau(unsigned long timeoutNhanhUs,
                       unsigned long timeoutChamUs) {
  DuLieuMau duLieu;
  if (!mauReady) camBienMauBatDau();

  // LOW/LOW=đỏ, LOW/HIGH=xanh dương,
  // HIGH/LOW=không lọc, HIGH/HIGH=xanh lá.
  duLieu.doPulse = docXungMau(LOW, LOW, timeoutNhanhUs, timeoutChamUs);
  duLieu.xanhDuongPulse = docXungMau(LOW, HIGH,
                                    timeoutNhanhUs, timeoutChamUs);
  duLieu.clearPulse = docXungMau(HIGH, LOW,
                                timeoutNhanhUs, timeoutChamUs);
  duLieu.xanhLaPulse = docXungMau(HIGH, HIGH,
                                 timeoutNhanhUs, timeoutChamUs);
  duLieu.hopLe = duLieu.doPulse > 0 && duLieu.xanhLaPulse > 0
              && duLieu.xanhDuongPulse > 0 && duLieu.clearPulse > 0;
  return duLieu;
}

MauSac nhanDangMau(const DuLieuMau &duLieu) {
  if (!duLieu.hopLe) return MAU_KHONG_XAC_DINH;
  if (duLieu.doPulse <= duLieu.xanhLaPulse
      && duLieu.doPulse <= duLieu.xanhDuongPulse) return MAU_DO;
  if (duLieu.xanhLaPulse <= duLieu.doPulse
      && duLieu.xanhLaPulse <= duLieu.xanhDuongPulse) return MAU_XANH_LA;
  return MAU_XANH_DUONG;
}

const char *tenMau(MauSac mau) {
  switch (mau) {
    case MAU_DO: return "DO";
    case MAU_XANH_LA: return "XANH LA";
    case MAU_XANH_DUONG: return "XANH DUONG";
    default: return "KHONG XAC DINH";
  }
}

