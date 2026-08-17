// BÀI 11: CẢM BIẾN MÀU TCS3200
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const uint8_t S0_PIN = 25;
const uint8_t S1_PIN = 24;
const uint8_t LED_PIN = 26;
const uint8_t OUT_PIN = 29;
const uint8_t S2_PIN = 28;
const uint8_t S3_PIN = 27;

const unsigned long TIMEOUT_NHANH_US = 30000UL;
const unsigned long TIMEOUT_CHAM_US = 100000UL;

Adafruit_SSD1306 oled(128, 64, &Wire, -1);

unsigned long docMotKenh(uint8_t s2, uint8_t s3) {
  digitalWrite(S2_PIN, s2);
  digitalWrite(S3_PIN, s3);
  delay(3); // Chờ bộ lọc quang ổn định.

  unsigned long xung = pulseIn(OUT_PIN, LOW, TIMEOUT_NHANH_US);
  if (xung == 0) {
    // Thử lại với thời gian chờ dài hơn khi bề mặt tối.
    xung = pulseIn(OUT_PIN, LOW, TIMEOUT_CHAM_US);
  }
  return xung;
}

const char *nhanDangMau(unsigned long r,
                        unsigned long g,
                        unsigned long b) {
  if (r == 0 || g == 0 || b == 0) return "NO PULSE";

  // Độ rộng xung nhỏ hơn nghĩa là kênh màu mạnh hơn.
  if (r * 100UL < g * 85UL && r * 100UL < b * 85UL) return "DO";
  if (g * 100UL < r * 85UL && g * 100UL < b * 85UL) return "XANH LA";
  if (b * 100UL < r * 85UL && b * 100UL < g * 85UL) return "XANH DUONG";
  return "CHUA RO";
}

void hienThi(unsigned long r, unsigned long g,
             unsigned long b, const char *mau) {
  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE);
  oled.setTextSize(1);
  oled.setCursor(0, 0);
  oled.println("TCS3200 - XUNG us");
  oled.setCursor(0, 16); oled.print("R: "); r ? oled.println(r) : oled.println("--");
  oled.setCursor(0, 28); oled.print("G: "); g ? oled.println(g) : oled.println("--");
  oled.setCursor(0, 40); oled.print("B: "); b ? oled.println(b) : oled.println("--");
  oled.setCursor(0, 53); oled.print("MAU: "); oled.println(mau);
  oled.display();
}

void setup() {
  Serial.begin(115200);
  pinMode(S0_PIN, OUTPUT);
  pinMode(S1_PIN, OUTPUT);
  pinMode(S2_PIN, OUTPUT);
  pinMode(S3_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(OUT_PIN, INPUT);

  // Chọn tỉ lệ tần số đầu ra 20%.
  digitalWrite(S0_PIN, HIGH);
  digitalWrite(S1_PIN, LOW);
  digitalWrite(LED_PIN, HIGH);

  Wire.begin();
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("LOI OLED");
    while (true) delay(1000);
  }
}

void loop() {
  unsigned long r = docMotKenh(LOW, LOW);   // Bộ lọc đỏ
  unsigned long b = docMotKenh(LOW, HIGH);  // Bộ lọc lam
  unsigned long g = docMotKenh(HIGH, HIGH); // Bộ lọc lục
  const char *mau = nhanDangMau(r, g, b);

  Serial.print("R="); Serial.print(r);
  Serial.print(" G="); Serial.print(g);
  Serial.print(" B="); Serial.print(b);
  Serial.print(" MAU="); Serial.println(mau);

  hienThi(r, g, b, mau);
  delay(150);
}
