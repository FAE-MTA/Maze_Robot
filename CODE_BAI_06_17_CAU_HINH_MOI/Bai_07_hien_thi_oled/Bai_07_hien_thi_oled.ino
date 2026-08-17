// BÀI 7: HIỂN THỊ OLED 128x64 QUA I2C
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const uint8_t DIA_CHI_OLED = 0x3C;
const int CHIEU_RONG = 128;
const int CHIEU_CAO = 64;

Adafruit_SSD1306 oled(CHIEU_RONG, CHIEU_CAO, &Wire, -1);
unsigned long lanCapNhatCuoi = 0;
unsigned long boDem = 0;

void hienThiTrangThai(const char *trangThai) {
  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE);

  oled.setTextSize(1);
  oled.setCursor(0, 0);
  oled.println("ROBOT MEGA 2560");
  oled.drawLine(0, 11, 127, 11, SSD1306_WHITE);

  oled.setCursor(0, 20);
  oled.print("Trang thai: ");
  oled.println(trangThai);

  oled.setCursor(0, 36);
  oled.print("Thoi gian: ");
  oled.print(millis() / 1000.0, 1);
  oled.println(" s");

  oled.setCursor(0, 50);
  oled.print("So lan: ");
  oled.println(boDem);

  // Gửi bộ đệm lên màn hình.
  oled.display();
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (!oled.begin(SSD1306_SWITCHCAPVCC, DIA_CHI_OLED)) {
    Serial.println("LOI: KHONG TIM THAY OLED");
    while (true) {
      delay(1000); // Dừng an toàn để chờ kiểm tra phần cứng.
    }
  }

  oled.clearDisplay();
  oled.display();
  hienThiTrangThai("KHOI DONG");
  Serial.println("OLED DA SAN SANG");
}

void loop() {
  // Chỉ làm mới màn hình mỗi 200 ms.
  if (millis() - lanCapNhatCuoi >= 200) {
    lanCapNhatCuoi = millis();
    boDem++;
    hienThiTrangThai("DANG CHAY");

    Serial.print("Bo dem = ");
    Serial.println(boDem);
  }
}
