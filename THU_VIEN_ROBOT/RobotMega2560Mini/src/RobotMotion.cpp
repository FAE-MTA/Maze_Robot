#include "Robot.h"

namespace {
  long soXungTrungBinh() {
    long trai;
    long phai;
    docHaiEncoder(trai, phai);
    return (labs(trai) + labs(phai)) / 2;
  }
}

bool diTheoSoXung(long soXung, int pwm, unsigned long timeoutMs) {
  soXung = labs(soXung);
  if (soXung == 0 || pwm == 0) {
    dungRobot();
    return true;
  }

  xoaEncoder();
  dieuKhienDongCo(pwm, pwm);
  unsigned long batDauMs = millis();

  while (soXungTrungBinh() < soXung) {
    if (millis() - batDauMs >= timeoutMs) {
      dungRobot();
      return false;
    }
    delay(1);
  }

  dungRobot();
  return true;
}

bool quayTraiTheoSoXung(long soXung, int pwm, unsigned long timeoutMs) {
  soXung = labs(soXung);
  xoaEncoder();
  quayTrai(pwm);
  unsigned long batDauMs = millis();

  while (soXungTrungBinh() < soXung) {
    if (millis() - batDauMs >= timeoutMs) {
      dungRobot();
      return false;
    }
    delay(1);
  }

  dungRobot();
  return true;
}

bool quayPhaiTheoSoXung(long soXung, int pwm, unsigned long timeoutMs) {
  soXung = labs(soXung);
  xoaEncoder();
  quayPhai(pwm);
  unsigned long batDauMs = millis();

  while (soXungTrungBinh() < soXung) {
    if (millis() - batDauMs >= timeoutMs) {
      dungRobot();
      return false;
    }
    delay(1);
  }

  dungRobot();
  return true;
}

void quayTraiTheoThoiGian(int pwm, unsigned long thoiGianMs) {
  quayTrai(pwm);
  delay(thoiGianMs);
  dungRobot();
}

void quayPhaiTheoThoiGian(int pwm, unsigned long thoiGianMs) {
  quayPhai(pwm);
  delay(thoiGianMs);
  dungRobot();
}

bool diThangCanBangEncoder(int pwmCoBan, unsigned long thoiGianMs,
                          float kpCanBang) {
  pwmCoBan = constrain(abs(pwmCoBan), 0, 255);
  if (pwmCoBan == 0 || thoiGianMs == 0) {
    dungRobot();
    return true;
  }

  xoaEncoder();
  long xungTraiTruoc = 0;
  long xungPhaiTruoc = 0;
  unsigned long batDauMs = millis();
  unsigned long lanDieuKhienMs = batDauMs;

  while (millis() - batDauMs < thoiGianMs) {
    unsigned long hienTaiMs = millis();
    if (hienTaiMs - lanDieuKhienMs < 20) continue;

    long xungTrai;
    long xungPhai;
    docHaiEncoder(xungTrai, xungPhai);
    long deltaTrai = labs(xungTrai - xungTraiTruoc);
    long deltaPhai = labs(xungPhai - xungPhaiTruoc);
    long saiSo = deltaTrai - deltaPhai;
    int hieuChinh = constrain((int)(kpCanBang * saiSo), -50, 50);

    int pwmTrai = constrain(pwmCoBan - hieuChinh, 0, 255);
    int pwmPhai = constrain(pwmCoBan + hieuChinh, 0, 255);
    dieuKhienDongCo(pwmTrai, pwmPhai);

    xungTraiTruoc = xungTrai;
    xungPhaiTruoc = xungPhai;
    lanDieuKhienMs = hienTaiMs;
  }

  dungRobot();
  return true;
}

