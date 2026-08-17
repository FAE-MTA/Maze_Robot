// BÀI 2: LED NEOPIXEL CƠ BẢN TRÊN ROBOT
#include <Adafruit_NeoPixel.h>

const uint8_t LED_PIN = 13;
const uint8_t SO_LED = 12;
Adafruit_NeoPixel vongLed(SO_LED, LED_PIN, NEO_GRB + NEO_KHZ800);

void datMau(uint8_t r, uint8_t g, uint8_t b) {
  uint32_t mau = vongLed.Color(r, g, b);
  for (uint8_t i = 0; i < SO_LED; i++) {
    vongLed.setPixelColor(i, mau);
  }
  vongLed.show();
}

void setup() {
  Serial.begin(115200);
  vongLed.begin();
  vongLed.setBrightness(100);
  vongLed.clear();
  vongLed.show();
}

void loop() {
  Serial.println(F("DO - BAO LOI"));
  datMau(255, 0, 0); delay(1000);
  Serial.println(F("XANH LA - SAN SANG"));
  datMau(0, 255, 0); delay(1000);
  Serial.println(F("XANH DUONG - DANG CHAY"));
  datMau(0, 0, 255); delay(1000);
  Serial.println(F("TAT"));
  datMau(0, 0, 0); delay(1000);
}
