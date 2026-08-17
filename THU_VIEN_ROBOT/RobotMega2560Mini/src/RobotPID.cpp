#include "Robot.h"

void pidCauHinh(BoDieuKhienPID &pid, float kp, float ki, float kd,
                float gioiHanTichPhan) {
  pid.kp = kp;
  pid.ki = ki;
  pid.kd = kd;
  pid.gioiHanTichPhan = abs(gioiHanTichPhan);
  pidXoa(pid);
}

void pidXoa(BoDieuKhienPID &pid) {
  pid.tichPhan = 0.0f;
  pid.saiSoTruoc = 0.0f;
  pid.thoiDiemTruocUs = micros();
  pid.daCoMauTruoc = false;
}

float pidTinh(BoDieuKhienPID &pid, float saiSo) {
  unsigned long hienTaiUs = micros();
  unsigned long deltaUs = hienTaiUs - pid.thoiDiemTruocUs;
  float dt = deltaUs > 0 ? deltaUs / 1000000.0f : 0.001f;

  float daoHam = 0.0f;
  if (pid.daCoMauTruoc) {
    daoHam = (saiSo - pid.saiSoTruoc) / dt;
  }

  pid.tichPhan += saiSo * dt;
  pid.tichPhan = constrain(pid.tichPhan,
                          -pid.gioiHanTichPhan,
                          pid.gioiHanTichPhan);

  float dauRa = pid.kp * saiSo
              + pid.ki * pid.tichPhan
              + pid.kd * daoHam;

  pid.saiSoTruoc = saiSo;
  pid.thoiDiemTruocUs = hienTaiUs;
  pid.daCoMauTruoc = true;
  return dauRa;
}
