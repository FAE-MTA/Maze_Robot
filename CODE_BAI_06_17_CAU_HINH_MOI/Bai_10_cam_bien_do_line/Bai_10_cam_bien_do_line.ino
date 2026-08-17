// BÀI 10: ĐỌC 5 CẢM BIẾN DÒ LINE
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const uint8_t LINE_PIN[5] = {A1, A2, A3, A4, A5};
const int TRONG_SO[5] = {-200, -100, 0, 100, 200};

// Ngưỡng phải được thay bằng kết quả hiệu chỉnh của từng mắt.
int nguong[5] = {500, 500, 500, 500, 500};
int giaTri[5];

Adafruit_SSD1306 oled(128, 64, &Wire, -1);

bool docViTriLine(int &viTri) {
  long tongTrongSo = 0;
  long tongCuongDo = 0;

  for (uint8_t i = 0; i < 5; i++) {
    giaTri[i] = analogRead(LINE_PIN[i]);

    // Vạch tối làm giá trị giảm: cường độ = ngưỡng - giá trị.
    int cuongDo = nguong[i] - giaTri[i];
    if (cuongDo < 0) cuongDo = 0;

    tongTrongSo += (long)cuongDo * TRONG_SO[i];
    tongCuongDo += cuongDo;
  }

  if (tongCuongDo < 80) {
    return false; // Không đủ tín hiệu để kết luận có line.
  }

  viTri = tongTrongSo / tongCuongDo;
  return true;
}

void hienThi(bool coLine, int viTri) {
  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE);
  oled.setTextSize(1);
  oled.setCursor(0, 0);
  oled.println("LINE A1 A2 A3 A4 A5");
  oled.setCursor(0, 16);
  for (uint8_t i = 0; i < 5; i++) {
    oled.print(giaTri[i]);
    if (i < 4) oled.print(' ');
  }
  oled.setCursor(0, 34);
  oled.print("Vi tri: ");
  coLine ? oled.println(viTri) : oled.println("MAT LINE");
  oled.setCursor(0, 50);
  if (!coLine) oled.println("Dung / tim lai vach");
  else if (viTri < -30) oled.println("Vach ben TRAI");
  else if (viTri > 30) oled.println("Vach ben PHAI");
  else oled.println("Vach O GIUA");
  oled.display();
}

void setup() {
  Serial.begin(115200);
  for (uint8_t i = 0; i < 5; i++) pinMode(LINE_PIN[i], INPUT);

  Wire.begin();
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("LOI OLED");
    while (true) delay(1000);
  }
}

void loop() {
  int viTri = 0;
  bool coLine = docViTriLine(viTri);

  for (uint8_t i = 0; i < 5; i++) {
    Serial.print(giaTri[i]); Serial.print(i < 4 ? ',' : ' ');
  }
  Serial.print("VI_TRI=");
  coLine ? Serial.println(viTri) : Serial.println("MAT_LINE");

  hienThi(coLine, viTri);
  delay(100);
}
