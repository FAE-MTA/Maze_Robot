#pragma once

#include <Arduino.h>

// Các kiểu dữ liệu dùng chung giúp hàm có đầu vào và đầu ra rõ ràng.
enum MauSac : uint8_t {
  MAU_KHONG_XAC_DINH,
  MAU_DO,
  MAU_XANH_LA,
  MAU_XANH_DUONG
};

enum KetQuaTacVu : uint8_t {
  TAC_VU_DANG_CHAY,
  TAC_VU_HOAN_THANH,
  TAC_VU_LOI
};

struct DuLieuMau {
  unsigned long doPulse;
  unsigned long xanhLaPulse;
  unsigned long xanhDuongPulse;
  unsigned long clearPulse;
  bool hopLe;
};

struct DuLieuKhoangCach {
  float truocCm;
  uint16_t traiMm;
  uint16_t phaiMm;
  bool sieuAmHopLe;
  bool laserTraiHopLe;
  bool laserPhaiHopLe;
};

struct DuLieuLine {
  int giaTri[5];
  int viTri;
  long tongDoManh;
  bool thayVach;
};

struct BoDieuKhienPID {
  float kp;
  float ki;
  float kd;
  float tichPhan;
  float saiSoTruoc;
  float gioiHanTichPhan;
  unsigned long thoiDiemTruocUs;
  bool daCoMauTruoc;
};

