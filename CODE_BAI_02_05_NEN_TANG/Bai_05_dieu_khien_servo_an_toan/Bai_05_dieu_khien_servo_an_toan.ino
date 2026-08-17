// BÀI 5: RC SERVO D15 - NGẮT XUNG KHI KHÔNG DÙNG
#include <Wire.h>
#include <Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const uint8_t SERVO_PIN = 15;
Servo servoRobot;
Adafruit_SSD1306 oled(128, 64, &Wire, -1);

void ngatServo() {
  if (servoRobot.attached()) servoRobot.detach();
  pinMode(SERVO_PIN, OUTPUT);
  digitalWrite(SERVO_PIN, LOW);
}

void datGoc(int goc) {
  servoRobot.attach(SERVO_PIN);
  servoRobot.write(constrain(goc, 0, 180));

  Serial.print(F("Servo = ")); Serial.print(goc); Serial.println(F(" do"));
  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE);
  oled.setTextSize(1);
  oled.setCursor(12, 5); oled.println(F("RC SERVO D15"));
  oled.setTextSize(3);
  oled.setCursor(28, 27); oled.print(goc);
  oled.setTextSize(1); oled.print(F(" do"));
  oled.display();

  delay(700);  // Chờ servo tới vị trí.
  ngatServo(); // Ngừng giữ lực và tránh rung khi chờ.
  delay(500);
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("LOI OLED"));
    ngatServo();
    while (true) delay(1000);
  }
  ngatServo();
  delay(1000);
}

void loop() {
  datGoc(30);
  datGoc(90);
  datGoc(150);
  datGoc(90);
  delay(1500);
}
