// BÀI 6: ĐIỀU KHIỂN HAI ĐỘNG CƠ DC
// Bo mạch: Arduino Mega 2560 Mini

const uint8_t MOTOR_TRAI_A = 5;
const uint8_t MOTOR_TRAI_B = 9;
const uint8_t MOTOR_PHAI_A = 6;
const uint8_t MOTOR_PHAI_B = 10;

// Điều khiển một động cơ bằng tốc độ có dấu.
// tocDo > 0: quay thuận; tocDo < 0: quay ngược; tocDo = 0: dừng.
void datMotDongCo(uint8_t chanA, uint8_t chanB,
                 int tocDo, bool daoChieu) {
  tocDo = constrain(tocDo, -255, 255);
  if (daoChieu) tocDo = -tocDo;

  if (tocDo > 0) {
    analogWrite(chanA, tocDo);
    analogWrite(chanB, 0);
  } else if (tocDo < 0) {
    analogWrite(chanA, 0);
    analogWrite(chanB, -tocDo);
  } else {
    analogWrite(chanA, 0);
    analogWrite(chanB, 0);
  }
}

void datHaiDongCo(int tocDoTrai, int tocDoPhai) {
  datMotDongCo(MOTOR_TRAI_A, MOTOR_TRAI_B, tocDoTrai, false);
  datMotDongCo(MOTOR_PHAI_A, MOTOR_PHAI_B, tocDoPhai, true);
}

void diThang(int pwm)  { datHaiDongCo( pwm,  pwm); }
void diLui(int pwm)    { datHaiDongCo(-pwm, -pwm); }
void quayTrai(int pwm) { datHaiDongCo(-pwm,  pwm); }
void quayPhai(int pwm) { datHaiDongCo( pwm, -pwm); }
void dungRobot()       { datHaiDongCo(0, 0); }

void chayTrongThoiGian(const char *tenLenh,
                       void (*hamChuyenDong)(int),
                       int pwm, unsigned long thoiGianMs) {
  Serial.println(tenLenh);
  hamChuyenDong(pwm);
  delay(thoiGianMs);
  dungRobot();
  delay(800);
}

void setup() {
  Serial.begin(115200);
  pinMode(MOTOR_TRAI_A, OUTPUT);
  pinMode(MOTOR_TRAI_B, OUTPUT);
  pinMode(MOTOR_PHAI_A, OUTPUT);
  pinMode(MOTOR_PHAI_B, OUTPUT);
  dungRobot();
  Serial.println("BAT DAU BAI 6");
  delay(2000);
}

void loop() {
  chayTrongThoiGian("TIEN",  diThang,  120, 1500);
  chayTrongThoiGian("LUI",   diLui,    120, 1500);
  chayTrongThoiGian("TRAI",  quayTrai, 110, 700);
  chayTrongThoiGian("PHAI",  quayPhai, 110, 700);

  Serial.println("KET THUC CHU KY\n");
  delay(2500);
}
