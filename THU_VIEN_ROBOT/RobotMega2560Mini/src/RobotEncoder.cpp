#include "Robot.h"

namespace {
  volatile long xungTrai = 0;
  volatile long xungPhai = 0;
  bool encoderReady = false;

  void ngatEncoderTrai() {
    xungTrai += (digitalRead(RobotPins::ENC1_B) == LOW) ? 1 : -1;
  }

  void ngatEncoderPhai() {
    xungPhai += (digitalRead(RobotPins::ENC2_B) == LOW) ? 1 : -1;
  }
}

void encoderBatDau() {
  pinMode(RobotPins::ENC1_A, INPUT_PULLUP);
  pinMode(RobotPins::ENC1_B, INPUT_PULLUP);
  pinMode(RobotPins::ENC2_A, INPUT_PULLUP);
  pinMode(RobotPins::ENC2_B, INPUT_PULLUP);

  if (!encoderReady) {
    attachInterrupt(digitalPinToInterrupt(RobotPins::ENC1_A),
                    ngatEncoderTrai, RISING);
    attachInterrupt(digitalPinToInterrupt(RobotPins::ENC2_A),
                    ngatEncoderPhai, RISING);
    encoderReady = true;
  }
  xoaEncoder();
}

void xoaEncoder() {
  noInterrupts();
  xungTrai = 0;
  xungPhai = 0;
  interrupts();
}

long docEncoderTrai() {
  noInterrupts();
  long giaTri = xungTrai;
  interrupts();
  return giaTri;
}

long docEncoderPhai() {
  noInterrupts();
  long giaTri = xungPhai;
  interrupts();
  return giaTri;
}

void docHaiEncoder(long &trai, long &phai) {
  noInterrupts();
  trai = xungTrai;
  phai = xungPhai;
  interrupts();
}

float tinhRPM(long soXungMoi, float xungMoiVong,
             unsigned long thoiGianMs) {
  if (xungMoiVong <= 0.0f || thoiGianMs == 0) return 0.0f;
  return soXungMoi * 60000.0f / (xungMoiVong * thoiGianMs);
}
