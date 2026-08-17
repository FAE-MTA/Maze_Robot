// BÀI 4: GIAO TIẾP SERIAL - CHƯA ĐIỀU KHIỂN ĐỘNG CƠ
void setup() {
  Serial.begin(115200);
  Serial.println(F("Nhap F/B/L/R/S roi nhan Send"));
  Serial.println(F("Chi giai ma lenh, robot KHONG chay"));
}

void loop() {
  static unsigned long mocBaoCao = 0;

  if (Serial.available() > 0) {
    char lenh = toupper(Serial.read());
    if (lenh == '\n' || lenh == '\r') return;

    Serial.print(F("Nhan lenh: "));
    switch (lenh) {
      case 'F': Serial.println(F("TIEN")); break;
      case 'B': Serial.println(F("LUI")); break;
      case 'L': Serial.println(F("TRAI")); break;
      case 'R': Serial.println(F("PHAI")); break;
      case 'S': Serial.println(F("DUNG")); break;
      default:  Serial.println(F("KHONG HOP LE")); break;
    }
  }

  if (millis() - mocBaoCao >= 1000) {
    mocBaoCao = millis();
    Serial.print(F("Uptime: "));
    Serial.print(millis() / 1000);
    Serial.println(F(" s"));
  }
}
