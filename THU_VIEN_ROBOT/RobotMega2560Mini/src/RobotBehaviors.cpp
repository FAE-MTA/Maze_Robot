#include "Robot.h"

bool bamLinePIDMotBuoc(BoDieuKhienPID &pid, int pwmCoBan,
                      int pwmToiDa, int pwmTimVach,
                      const int nguong[5], bool vachDenChoGiaTriThap) {
  static int huongVachCuoi = 1;
  DuLieuLine line = docLine(nguong, vachDenChoGiaTriThap);

  if (!line.thayVach) {
    pidXoa(pid);
    if (huongVachCuoi >= 0) quayPhai(pwmTimVach);
    else quayTrai(pwmTimVach);
    return false;
  }

  huongVachCuoi = line.viTri;
  int hieuChinh = (int)pidTinh(pid, line.viTri);
  hieuChinh = constrain(hieuChinh, -pwmToiDa, pwmToiDa);

  // Vạch ở bên phải: tăng bánh trái và giảm bánh phải.
  int pwmTrai = constrain(pwmCoBan + hieuChinh, 0, pwmToiDa);
  int pwmPhai = constrain(pwmCoBan - hieuChinh, 0, pwmToiDa);
  dieuKhienDongCo(pwmTrai, pwmPhai);
  return true;
}

bool bamTuongMotBuoc(bool bamBenTrai, uint16_t khoangCachDatMm,
                     int pwmCoBan, float kp,
                     uint16_t khoangCachToiDaMm) {
  uint16_t khoangCachMm = 0;
  bool hopLe = bamBenTrai
      ? docLaserTrai(khoangCachMm)
      : docLaserPhai(khoangCachMm);

  if (!hopLe || khoangCachMm > khoangCachToiDaMm) {
    // Không thấy tường: quay nhẹ về phía cần tìm tường.
    int hieuChinhTim = 30;
    if (bamBenTrai) {
      dieuKhienDongCo(pwmCoBan - hieuChinhTim,
                      pwmCoBan + hieuChinhTim);
    } else {
      dieuKhienDongCo(pwmCoBan + hieuChinhTim,
                      pwmCoBan - hieuChinhTim);
    }
    return false;
  }

  int saiSo = (int)khoangCachMm - (int)khoangCachDatMm;
  int hieuChinh = constrain((int)(kp * saiSo), -50, 50);
  int pwmTrai;
  int pwmPhai;

  if (bamBenTrai) {
    pwmTrai = pwmCoBan - hieuChinh;
    pwmPhai = pwmCoBan + hieuChinh;
  } else {
    pwmTrai = pwmCoBan + hieuChinh;
    pwmPhai = pwmCoBan - hieuChinh;
  }

  dieuKhienDongCo(constrain(pwmTrai, 0, 255),
                  constrain(pwmPhai, 0, 255));
  return true;
}

bool timLaiVach(int huongCuoi, int pwmTim,
                unsigned long timeoutMs,
                const int nguong[5], bool vachDenChoGiaTriThap) {
  unsigned long batDauMs = millis();
  while (millis() - batDauMs < timeoutMs) {
    DuLieuLine line = docLine(nguong, vachDenChoGiaTriThap);
    if (line.thayVach) {
      dungRobot();
      return true;
    }

    if (huongCuoi >= 0) quayPhai(pwmTim);
    else quayTrai(pwmTim);
    delay(5);
  }

  dungRobot();
  return false;
}
