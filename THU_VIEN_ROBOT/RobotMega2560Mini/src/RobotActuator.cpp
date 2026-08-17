#include "Robot.h"

#include <Servo.h>
#include <Adafruit_NeoPixel.h>

namespace {
  Servo servoRobot;
  bool servoReady = false;
  Adafruit_NeoPixel vongLed(RobotPins::NEOPIXEL_COUNT,
                            RobotPins::NEOPIXEL,
                            NEO_GRB + NEO_KHZ800);
  bool neoPixelReady = false;

  void dieuKhienMotDongCo(uint8_t pinA, uint8_t pinB,
                         int pwm, bool daoChieu) {
    pwm = constrain(pwm, -255, 255);
    if (daoChieu) pwm = -pwm;

    if (pwm > 0) {
      analogWrite(pinA, pwm);
      analogWrite(pinB, 0);
    } else if (pwm < 0) {
      analogWrite(pinA, 0);
      analogWrite(pinB, -pwm);
    } else {
      analogWrite(pinA, 0);
      analogWrite(pinB, 0);
    }
  }
}

void dongCoBatDau() {
  pinMode(RobotPins::M1_A, OUTPUT);
  pinMode(RobotPins::M1_B, OUTPUT);
  pinMode(RobotPins::M2_A, OUTPUT);
  pinMode(RobotPins::M2_B, OUTPUT);
  dungRobot();
}

void dieuKhienDongCo(int pwmTrai, int pwmPhai) {
  dieuKhienMotDongCo(RobotPins::M1_A, RobotPins::M1_B,
                    pwmTrai, RobotPins::MOTOR1_INVERT);
  dieuKhienMotDongCo(RobotPins::M2_A, RobotPins::M2_B,
                    pwmPhai, RobotPins::MOTOR2_INVERT);
}

void diThang(int pwm) {
  pwm = abs(pwm);
  dieuKhienDongCo(pwm, pwm);
}

void diLui(int pwm) {
  pwm = abs(pwm);
  dieuKhienDongCo(-pwm, -pwm);
}

void quayTrai(int pwm) {
  pwm = abs(pwm);
  dieuKhienDongCo(-pwm, pwm);
}

void quayPhai(int pwm) {
  pwm = abs(pwm);
  dieuKhienDongCo(pwm, -pwm);
}

void dungRobot() {
  dieuKhienDongCo(0, 0);
}

void servoBatDau(int gocBanDau) {
  if (!servoReady) {
    servoRobot.attach(RobotPins::SERVO);
    servoReady = true;
  }
  datGocServo(gocBanDau);
}

void datGocServo(int goc) {
  if (!servoReady) return;
  servoRobot.write(constrain(goc, 0, 180));
}

void servoNgatTinHieu() {
  // Ngắt xung PWM servo để servo không tiếp tục giữ lực hoặc rung giật.
  // Kéo chân tín hiệu xuống LOW thay vì để chân bị trôi do nhiễu.
  if (servoReady) servoRobot.detach();
  servoReady = false;
  pinMode(RobotPins::SERVO, OUTPUT);
  digitalWrite(RobotPins::SERVO, LOW);
}

void neoPixelBatDau(uint8_t doSang) {
  if (!neoPixelReady) {
    vongLed.begin();
    neoPixelReady = true;
  }
  vongLed.setBrightness(doSang);
  tatNeoPixel();
}

void datMauNeoPixel(uint8_t red, uint8_t green, uint8_t blue) {
  if (!neoPixelReady) return;
  uint32_t mau = vongLed.Color(red, green, blue);
  for (uint8_t i = 0; i < RobotPins::NEOPIXEL_COUNT; i++) {
    vongLed.setPixelColor(i, mau);
  }
  vongLed.show();
}

void datMauMotNeoPixel(uint8_t viTri, uint8_t red,
                      uint8_t green, uint8_t blue) {
  if (!neoPixelReady) return;
  vongLed.clear();
  if (viTri < RobotPins::NEOPIXEL_COUNT) {
    vongLed.setPixelColor(viTri, vongLed.Color(red, green, blue));
  }
  vongLed.show();
}

void tatNeoPixel() {
  datMauNeoPixel(0, 0, 0);
}
