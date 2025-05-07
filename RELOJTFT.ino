#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TimeLib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

const char* ssid = "TU_CONEXION_EDWIN_SUQUI";
const char* password = "1900470053";

#define TFT_CS     33
#define TFT_RST    27
#define TFT_DC     14
#define TFT_SCLK   17
#define TFT_MOSI   16

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -5 * 3600, 60000);

uint16_t color_dibujo = ST77XX_WHITE;
uint16_t color_fondo = ST77XX_BLACK;

const int numData[10][7][4] = {
  {{0,0,12,60},{12,0,16,4},{28,0,4,60},{12,56,16,4}},
  {{10,0,16,4},{16,4,10,52},{10,56,18,4}},
  {{0,0,32,4},{0,4,4,14},{24,4,8,26},{0,30,32,4},{0,34,12,22},{28,48,4,12},{0,56,32,4}},
  {{0,0,32,4},{0,4,4,14},{24,4,8,52},{8,30,20,4},{0,48,4,12},{0,56,32,4}},
  {{0,0,4,30},{0,30,24,4},{24,0,8,60}},
  {{0,0,32,4},{0,4,12,26},{28,4,4,12},{0,30,32,4},{24,34,8,26},{0,48,4,12},{0,56,32,4}},
  {{0,0,32,4},{0,4,12,52},{28,4,4,12},{12,30,20,4},{28,34,4,26},{0,56,32,4}},
  {{0,0,32,4},{0,4,4,14},{24,4,8,56}},
  {{0,0,12,60},{12,0,20,4},{12,30,20,4},{32,0,4,60},{12,56,20,4}},
  {{0,0,32,4},{0,4,4,26},{24,4,8,52},{0,30,24,4},{0,48,4,12},{0,56,32,4}}
};

void drawDigit(uint8_t number, int x, int y, bool clearBackground = true) {
  if (clearBackground) tft.fillRect(x, y, 36, 60, color_fondo);
  for (int i = 0; i < 7; i++) {
    int w = numData[number][i][2];
    int h = numData[number][i][3];
    if (w == 0 || h == 0) continue;
    tft.fillRect(x + numData[number][i][0], y + numData[number][i][1], w, h, color_dibujo);
  }
}

void drawColon(int x, int y) {
  tft.fillRect(x, y + 18, 5, 5, color_dibujo);
  tft.fillRect(x, y + 40, 5, 5, color_dibujo);
}

void drawTime() {
  int h = hour();
  int m = minute();

  int h1 = h / 10, h2 = h % 10;
  int m1 = m / 10, m2 = m % 10;

  tft.fillRect(0, 0, 160, 80, color_fondo);

  drawDigit(h1, 6, 10);
  drawDigit(h2, 42, 10);
  drawColon(78, 10);
  drawDigit(m1, 90, 10);
  drawDigit(m2, 126, 10);
}

void drawDate() {
  tft.fillRect(0, 85, 160, 45, color_fondo);

  char fecha[20];
  sprintf(fecha, "%02d-%02d-%02d", day(), month(), year() % 100);

  const char* dias[] = {"DOMINGO", "LUNES", "MARTES", "MIÉRCOLES", "JUEVES", "VIERNES", "SÁBADO"};
  const char* dia = dias[weekday() - 1];

  tft.setTextColor(color_dibujo);
  tft.setTextSize(1);

  int anchoFecha = strlen(fecha) * 6;
  int anchoDia = strlen(dia) * 6;

  tft.setCursor((160 - anchoFecha) / 2, 95);
  tft.print(fecha);

  tft.setCursor((160 - anchoDia) / 2, 105);
  tft.print(dia);

  // Indicador de conexión WiFi
  if (WiFi.status() == WL_CONNECTED) {
    tft.fillCircle(150, 94, 3, ST77XX_GREEN); // Conectado
  } else {
    tft.fillCircle(150, 94, 3, ST77XX_RED);   // Desconectado
  }
}

void setup() {
  Serial.begin(115200);
  SPI.begin(TFT_SCLK, -1, TFT_MOSI);
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(color_fondo);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  timeClient.begin();
  timeClient.update();
  setTime(timeClient.getEpochTime());
}

unsigned long anterior = 0;
int prevHour = -1;
int prevMinute = -1;

void loop() {
  if (millis() - anterior > 60000) {
    timeClient.update();
    setTime(timeClient.getEpochTime());
    anterior = millis();
  }

  int currHour = hour();
  int currMinute = minute();

  if (currHour != prevHour || currMinute != prevMinute) {
    drawTime();
    drawDate();
    prevHour = currHour;
    prevMinute = currMinute;
  }

  delay(200);
}
