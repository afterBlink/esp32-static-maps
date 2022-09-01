#include "settings.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <TFT_eSPI.h>
#include <PNGdec.h>

TFT_eSPI tft = TFT_eSPI();
PNG png;

#ifdef HAS_PSRAM
unsigned long screen_buf_size = pixHor * pixVer;
uint16_t* screen = NULL;
#endif

unsigned long png_buf_size = 60000;
uint8_t* pngbuffer = NULL;  // buffer for 8bpp png file
unsigned long jp = 0;

typedef struct my_private_struct
{
  int xoff, yoff; // corner offset
} PRIVATE;
PRIVATE priv;

int rc;
char url[100];
uint8_t si;   // index of baseurl array
long tx, ty;  // tile coordinates (within map tile system) of tile that contains center location
int ctx;      // x-coordinate for posioning center-tile on display
int cty;      // x-coordinate for posioning center-tile on display

#include "functions.h"

void setup() {
  pngbuffer = (uint8_t*)malloc(png_buf_size);
  
#ifdef HAS_PSRAM
  screen = (uint16_t*)ps_malloc(screen_buf_size * sizeof(uint16_t));  // for storing map in PSRAM, e.g. on ESP32 WROVER boards
#endif
#ifndef M5STACK
  Serial.begin(115200);     // NEVER include Serial.begin() in your code when using M5Stack or M5Core2 library 
#endif
  tft.begin();
  tft.setRotation(orientation);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
#ifdef M5STACK
  tft.invertDisplay(true);  // may be necessary on other displays as well
#endif
#ifdef ILI9481_DRIVER       // e.g. LILY Pi with ili9481 IPS display
  tft.setSwapBytes(true);   // may be necessary on other displays as well
#endif

  tft.fillScreen(0);
  do_map(4,lat, lon, z);    // first parameter determines tile service (see index in baseurl[] array)
  print_mapinfo();

#ifdef HAS_PSRAM
  delay(1000);
  tft.fillScreen(0);
  tft.setCursor(20, 20);
  tft.print("Pushing stored map from PSRAM");
  delay(2000);
  tft.startWrite();
  tft.pushImage(0, 0, pixHor, pixVer, screen);
  tft.endWrite();
#endif

  // optional:
  WiFi.disconnect(true);
}

void loop() {

}
