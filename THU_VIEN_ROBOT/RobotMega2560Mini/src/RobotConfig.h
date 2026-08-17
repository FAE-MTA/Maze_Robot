#pragma once

#include <Arduino.h>

// Toàn bộ chân được khai báo tập trung tại đây.
// Các giá trị đã được cập nhật theo kết quả kiểm tra trên robot thật.
namespace RobotPins {
  // Động cơ và encoder.
  static const uint8_t M1_A = 5;
  static const uint8_t M1_B = 9;
  static const uint8_t M2_A = 6;
  static const uint8_t M2_B = 10;
  static const bool MOTOR1_INVERT = false;
  static const bool MOTOR2_INVERT = true;

  static const uint8_t ENC1_A = 2;
  static const uint8_t ENC1_B = 12;
  static const uint8_t ENC2_A = 3;
  static const uint8_t ENC2_B = 14;

  // Cơ cấu chấp hành và hiển thị.
  static const uint8_t SERVO = 15;
  static const uint8_t NEOPIXEL = 13;
  static const uint8_t NEOPIXEL_COUNT = 12;
  static const uint8_t OLED_ADDRESS = 0x3C;

  // Siêu âm phía trước và hai laser nhìn sang hai bên.
  static const uint8_t ULTRASONIC_TRIG = 4;
  static const uint8_t ULTRASONIC_ECHO = 7;
  static const uint8_t LASER_LEFT_XSHUT = 39;
  static const uint8_t LASER_RIGHT_XSHUT = 37;
  static const uint8_t LASER_LEFT_ADDRESS = 0x30;
  static const uint8_t LASER_RIGHT_ADDRESS = 0x31;

  // Năm cảm biến dò line từ trái sang phải.
  static const uint8_t LINE[5] = {A1, A2, A3, A4, A5};

  // Cảm biến màu TCS3200.
  static const uint8_t COLOR_S0 = 25;
  static const uint8_t COLOR_S1 = 24;
  static const uint8_t COLOR_LED = 26;
  static const uint8_t COLOR_OUT = 29;
  static const uint8_t COLOR_S2 = 28;
  static const uint8_t COLOR_S3 = 27;

  // Phần cứng chưa đấu nối, chỉ lưu cấu hình dự kiến.
  static const uint8_t BLUETOOTH_RX1 = 19;
  static const uint8_t BLUETOOTH_TX1 = 18;
  static const uint8_t BUTTON = A0;
}

namespace RobotDefaults {
  static const unsigned long SERIAL_BAUD = 115200UL;
  static const int LINE_THRESHOLD[5] = {500, 500, 500, 500, 500};
  static const bool LINE_DARK_IS_LOW = true;
  static const int LINE_WEIGHT[5] = {-200, -100, 0, 100, 200};
  static const int LINE_MIN_STRENGTH = 80;
  static const float TICKS_PER_REV = 20.0f;
}

