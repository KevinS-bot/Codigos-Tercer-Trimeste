#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TimeLib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

const char* ssid = "TU_CONEXION_EDWIN_SUQUI";
const char* password = "1900470053";

#define TFT_CS 33
#define TFT_RST 27
#define TFT_DC 14
#define TFT_SCLK 23
#define TFT_MOSI 18

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -5 * 3600, 60000);

uint16_t fg = ST77XX_WHITE, bg = ST77XX_BLACK;

const int digits[10][10][4] = {
  {{0,0,44,4},{0,4,4,80},{11,4,4,80},{40,4,4,80},{0,84,44,4}},
  {{5,0,24,4},{15,4,4,80},{25,4,4,80},{5,84,34,4}},
  {{0,0,44,4},{0,4,4,20},{29,4,4,38},{40,4,4,38},{0,42,44,4},{0,46,4,38},{11,46,4,38},{40,64,4,20},{0,84,44,4}},
  {{0,0,44,4},{0,4,4,20},{29,4,4,80},{40,4,4,80},{9,42,20,4},{0,64,4,20},{0,84,44,4}},
  {{0,0,4,42},{29,0,15,4},{29,4,4,80},{40,4,4,80},{0,42,29,4},{29,84,15,4}},
  {{0,0,44,4},{0,4,4,38},{11,4,4,38},{40,4,4,20},{0,42,44,4},{29,46,4,38},{40,46,4,38},{0,64,4,20},{0,84,44,4}},
  {{0,0,44,4},{0,4,4,80},{11,4,4,80},{40,4,4,20},{15,42,29,4},{40,46,4,38},{0,84,44,4}},
  {{0,0,44,4},{0,4,4,20},{29,4,4,80},{40,4,4,80},{29,84,15,4}},
  {{0,0,44,4},{0,4,4,80},{11,4,4,80},{40,4,4,80},{15,42,29,4},{0,84,44,4}},
  {{0,0,44,4},{0,4,4,38},{29,4,4,80},{40,4,4,80},{0,42,29,4},{0,64,4,20},{0,84,44,4}}
};

void drawDigit(uint8_t n, int x, int y) {
  tft.fillRect(x, y, 48, 88, bg);
  for (auto& seg : digits[n]) {
    if (seg[2] && seg[3])
      tft.fillRect(x + seg[0], y + seg[1], seg[2], seg[3], fg);
  }
}

void drawColon(int x, int y) {
  tft.fillRect(x, y + 30, 5, 5, fg);
  tft.fillRect(x, y + 50, 5, 5, fg);
}

void drawTime() {
  int h = hour(), m = minute();
  drawDigit(h / 10, 4, 0);
  drawDigit(h % 10, 56, 0);
  drawColon(108, 0);
  drawDigit(m / 10, 118, 0);
  drawDigit(m % 10, 170, 0);
}

void drawDate() {
  tft.fillRect(0, 92, 160, 45, bg);
  char fecha[20]; sprintf(fecha, "%02d-%02d-%02d", day(), month(), year() % 100);
  const char* dias[] = {"DOMINGO","LUNES","MARTES","MIÉRCOLES","JUEVES","VIERNES","SÁBADO"};
  const char* d = dias[weekday() - 1];
  tft.setTextColor(fg); tft.setTextSize(1);
  tft.setCursor((160 - strlen(fecha)*6)/2, 100); tft.print(fecha);
  tft.setCursor((160 - strlen(d)*6)/2, 110); tft.print(d);
  tft.fillCircle(150, 94, 3, WiFi.status() == WL_CONNECTED ? ST77XX_GREEN : ST77XX_RED);
}

void setup() {
  Serial.begin(115200);
  SPI.begin(TFT_SCLK, -1, TFT_MOSI);
  tft.initR(INITR_BLACKTAB); tft.setRotation(1); tft.fillScreen(bg);
  WiFi.begin(ssid, password); while (WiFi.status() != WL_CONNECTED) delay(500);
  timeClient.begin(); timeClient.update(); setTime(timeClient.getEpochTime());
}

unsigned long lastSync = 0;
int hPrev = -1, mPrev = -1;

void loop() {
  if (millis() - lastSync > 60000) {
    timeClient.update(); setTime(timeClient.getEpochTime());
    lastSync = millis();
  }
  int h = hour(), m = minute();
  if (h != hPrev || m != mPrev) {
    drawTime(); drawDate(); hPrev = h; mPrev = m;
  }
  delay(500);
}
