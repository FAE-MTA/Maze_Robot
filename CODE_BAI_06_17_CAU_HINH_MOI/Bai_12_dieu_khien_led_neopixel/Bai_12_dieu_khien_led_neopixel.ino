// BÀI 12: ĐIỀU KHIỂN VÒNG LED NEOPIXEL
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>

const uint8_t PIXEL_PIN = 13;
const uint8_t PIXEL_COUNT = 12;
const uint8_t DO_SANG = 150;

Adafruit_NeoPixel pixels(PIXEL_COUNT, PIXEL_PIN,
                         NEO_GRB + NEO_KHZ800);
Adafruit_SSD1306 oled(128, 64, &Wire, -1);

void datTatCaLed(uint8_t red, uint8_t green, uint8_t blue) {
  uint32_t mau = pixels.Color(red, green, blue);
  for (uint8_t i = 0; i < PIXEL_COUNT; i++) {
    pixels.setPixelColor(i, mau);
  }
  pixels.show();
}

void hienThiMau(const __FlashStringHelper *ten,
                uint8_t r, uint8_t g, uint8_t b) {
  oled.clearDisplay();
  oled.drawRect(0, 0, 128, 64, SSD1306_WHITE);
  oled.setTextColor(SSD1306_WHITE);
  oled.setTextSize(1);
  oled.setCursor(8, 4); oled.println(F("VONG LED NEOPIXEL"));
  oled.drawLine(0, 16, 127, 16, SSD1306_WHITE);
  oled.setCursor(4, 23); oled.print(F("MAU: ")); oled.println(ten);
  oled.setCursor(4, 38); oled.print(F("R:")); oled.print(r);
  oled.print(F(" G:")); oled.print(g);
  oled.print(F(" B:")); oled.println(b);
  oled.setCursor(4, 53); oled.print(F("DATA D13 - 12 LED"));
  oled.display();
}

void chayMau(const __FlashStringHelper *ten,
             uint8_t r, uint8_t g, uint8_t b) {
  datTatCaLed(r, g, b);
  hienThiMau(ten, r, g, b);
  Serial.print(F("Mau: ")); Serial.println(ten);
  delay(1000);
}

void quetTungLed() {
  for (uint8_t i = 0; i < PIXEL_COUNT; i++) {
    pixels.clear();
    pixels.setPixelColor(i, pixels.Color(255, 0, 0));
    pixels.show();

    oled.clearDisplay();
    oled.setTextColor(SSD1306_WHITE);
    oled.setTextSize(1);
    oled.setCursor(8, 5); oled.println(F("KIEM TRA TUNG LED"));
    oled.setTextSize(2);
    oled.setCursor(20, 27); oled.print(F("LED "));
    oled.print(i + 1); oled.print('/'); oled.print(PIXEL_COUNT);
    oled.display();
    delay(120);
  }
  pixels.clear();
  pixels.show();
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("LOI OLED 0x3C"));
    while (true) delay(1000);
  }

  pixels.begin();
  pixels.setBrightness(DO_SANG);
  pixels.clear();
  pixels.show();
  Serial.println(F("NeoPixel D13, 12 LED"));
  quetTungLed();
}

void loop() {
  chayMau(F("DO"), 255, 0, 0);
  chayMau(F("XANH LA"), 0, 255, 0);
  chayMau(F("XANH DUONG"), 0, 0, 255);
  chayMau(F("TRANG THAP"), 60, 60, 60);
  chayMau(F("TAT"), 0, 0, 0);
  quetTungLed();
}
