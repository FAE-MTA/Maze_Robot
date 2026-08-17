#pragma once

#include <Arduino.h>
#include "RobotConfig.h"
#include "RobotTypes.h"

// ==================== KHỞI TẠO VÀ OLED ====================
void batDauSerial(unsigned long baud = RobotDefaults::SERIAL_BAUD);
bool oledBatDau(uint8_t diaChi = RobotPins::OLED_ADDRESS);
bool oledDaSanSang();
void oledXoa();
void oledTieuDe(const char *tieuDe);
void oledInChu(int16_t x, int16_t y, const char *noiDung,
              uint8_t coChu = 1);
void oledInSo(int16_t x, int16_t y, long giaTri, uint8_t coChu = 1);
void oledInSoThuc(int16_t x, int16_t y, float giaTri,
                 uint8_t soChuSoThapPhan = 1, uint8_t coChu = 1);
void oledCapNhat();
void oledThongBao(const char *tieuDe, const char *dong1,
                 const char *dong2 = "", const char *dong3 = "");

// ==================== ĐỘNG CƠ, SERVO, NEOPIXEL ====================
void dongCoBatDau();
void dieuKhienDongCo(int pwmTrai, int pwmPhai);
void diThang(int pwm);
void diLui(int pwm);
void quayTrai(int pwm);
void quayPhai(int pwm);
void dungRobot();

void servoBatDau(int gocBanDau = 90);
void datGocServo(int goc);
void servoNgatTinHieu();
void neoPixelBatDau(uint8_t doSang = 100);
void datMauNeoPixel(uint8_t red, uint8_t green, uint8_t blue);
void datMauMotNeoPixel(uint8_t viTri, uint8_t red,
                      uint8_t green, uint8_t blue);
void tatNeoPixel();

// ==================== ENCODER ====================
void encoderBatDau();
void xoaEncoder();
long docEncoderTrai();
long docEncoderPhai();
void docHaiEncoder(long &trai, long &phai);
float tinhRPM(long soXungMoi, float xungMoiVong,
             unsigned long thoiGianMs);

// ==================== CẢM BIẾN ====================
void sieuAmBatDau();
bool docSieuAm(float &khoangCachCm, unsigned long timeoutUs = 25000UL);

bool laserBatDau();
bool laserTraiDaSanSang();
bool laserPhaiDaSanSang();
bool docLaserTrai(uint16_t &khoangCachMm);
bool docLaserPhai(uint16_t &khoangCachMm);
DuLieuKhoangCach docBaHuong();

void docLineRaw(int giaTri[5]);
DuLieuLine docLine(const int nguong[5] = RobotDefaults::LINE_THRESHOLD,
                  bool vachDenChoGiaTriThap = RobotDefaults::LINE_DARK_IS_LOW);
bool tatCaMatTrenVach(const int nguong[5] = RobotDefaults::LINE_THRESHOLD,
                     bool vachDenChoGiaTriThap = RobotDefaults::LINE_DARK_IS_LOW);

void camBienMauBatDau();
DuLieuMau docCamBienMau(unsigned long timeoutNhanhUs = 50000UL,
                       unsigned long timeoutChamUs = 800000UL);
MauSac nhanDangMau(const DuLieuMau &duLieu);
const char *tenMau(MauSac mau);

// ==================== PID VÀ CHUYỂN ĐỘNG ====================
void pidCauHinh(BoDieuKhienPID &pid, float kp, float ki, float kd,
                float gioiHanTichPhan = 3000.0f);
void pidXoa(BoDieuKhienPID &pid);
float pidTinh(BoDieuKhienPID &pid, float saiSo);

bool diTheoSoXung(long soXung, int pwm,
                  unsigned long timeoutMs = 5000UL);
bool quayTraiTheoSoXung(long soXung, int pwm,
                       unsigned long timeoutMs = 3000UL);
bool quayPhaiTheoSoXung(long soXung, int pwm,
                       unsigned long timeoutMs = 3000UL);
void quayTraiTheoThoiGian(int pwm, unsigned long thoiGianMs);
void quayPhaiTheoThoiGian(int pwm, unsigned long thoiGianMs);
bool diThangCanBangEncoder(int pwmCoBan, unsigned long thoiGianMs,
                          float kpCanBang = 5.0f);

// ==================== HÀNH VI DÙNG LẠI ====================
bool bamLinePIDMotBuoc(BoDieuKhienPID &pid, int pwmCoBan,
                      int pwmToiDa, int pwmTimVach,
                      const int nguong[5] = RobotDefaults::LINE_THRESHOLD,
                      bool vachDenChoGiaTriThap = RobotDefaults::LINE_DARK_IS_LOW);

bool bamTuongMotBuoc(bool bamBenTrai, uint16_t khoangCachDatMm,
                     int pwmCoBan, float kp,
                     uint16_t khoangCachToiDaMm = 1500);

bool timLaiVach(int huongCuoi, int pwmTim,
                unsigned long timeoutMs,
                const int nguong[5] = RobotDefaults::LINE_THRESHOLD,
                bool vachDenChoGiaTriThap = RobotDefaults::LINE_DARK_IS_LOW);
