// BÀI 3: NÚT A0 - HIỆN CHƯA ĐẤU TRÊN ROBOT
const uint8_t NUT_PIN = A0;

void setup() {
  Serial.begin(115200);
  pinMode(NUT_PIN, INPUT_PULLUP);
  Serial.println(F("NUT A0: BAI THAM KHAO, CHUA DAU NOI"));
}

void loop() {
  bool dangNhan = digitalRead(NUT_PIN) == LOW;
  Serial.println(dangNhan ? F("NUT: NHAN") : F("NUT: THA"));
  delay(100);
}
