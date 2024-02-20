#include <math.h>

typedef struct {
  float border_n;   // latitude border North
  float border_e;   // longitude border East
  float border_s;   // latitude border South
  float border_w;   // longitude border West
  double mpp;       // meters per pixel
  double degpp_hor; // horizontal degrees per pixel, usefull for positioning markers (sprites) and/or get longitude of touch point
  double degpp_ver; // vertical degrees per pixel, usefull for positioning markers (sprites) and/or get latitude of touch point
  float max_dist;   // distance from map center to corners (km)
} Area;

Area myArea;

// Map tile system related functions
int long2tilex(double lon, int z) {
  return (int)(floor((lon + 180.0) / 360.0 * (1 << z)));
}

int lat2tiley(double lat, int z) {
  double latrad = lat * M_PI / 180.0;
  return (int)(floor((1.0 - asinh(tan(latrad)) / M_PI) / 2.0 * (1 << z)));
}

double tilex2long(int x, int z) {
  return x / (double)(1 << z) * 360.0 - 180;
}

double tiley2lat(int y, int z) {
  double n = M_PI - 2.0 * M_PI * y / (double)(1 << z);
  return 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
}

bool getFile(String url, uint8_t* pngbuffer) {
  Serial.println("Downloading tile from " + url);
  if ((WiFi.status() == WL_CONNECTED)) {
    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
        int total = http.getSize();
        int len = total;
        uint8_t buff[128] = { 0 };
        WiFiClient * stream = http.getStreamPtr();
        while (http.connected() && (len > 0 || len == -1)) {
          size_t size = stream->available();
          if (size) {
            int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
            memcpy (pngbuffer + jp, buff, c);
            jp = jp + c;
            if (len > 0) {
              len -= c;
            }
          }
          yield();
        }
      }
    }
    else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
  return 1;
}

void PNGDraw(PNGDRAW *pDraw)
{
  if ((priv.yoff + pDraw->y) >= 0 && (priv.yoff + pDraw->y) < pixVer) {
    uint16_t usPixels[png.getWidth()];  // must be large enough to hold a horizontal line of tile
    PRIVATE *pPriv = (PRIVATE *)pDraw->pUser;
    png.getLineAsRGB565(pDraw, usPixels, PNG_RGB565_LITTLE_ENDIAN, 0xffffffff); // don't do alpha color blending
    int len = pDraw->iWidth;
    int p_offset = 0;  // offset in usPixels
    int new_xoff = priv.xoff;
    if (priv.xoff < 0) {
      len = _min(pDraw->iWidth + priv.xoff, pixHor);
      p_offset = -priv.xoff;
      new_xoff = 0;
    } else {
      len = _min(pixHor - priv.xoff, pDraw->iWidth);
    }
    tft.pushImage(new_xoff, priv.yoff + pDraw->y, len, 1, usPixels + p_offset);
#ifdef HAS_PSRAM
    memcpy((uint8_t*)(screen + (new_xoff + pixHor * (priv.yoff + pDraw->y))), (uint8_t*)(usPixels + p_offset), 2 * len);
#endif

  }
}

void do_tile(int tx_offset, int ty_offset, uint8_t zoom) {
  n_tiles++;  // tile counter
  sprintf(url, "%s/%d/%d/%d.png", baseurl[si], zoom, tx + tx_offset, ty + ty_offset);
  jp = 0;  // reset global pointer in pngbuffer array before downloading a tile
  bool loaded_ok = getFile(url, pngbuffer);
  rc = png.openRAM((uint8_t *)pngbuffer, jp, PNGDraw);
  if (rc == PNG_SUCCESS) {
    priv.xoff = ctx + 256 * tx_offset;
    priv.yoff = cty + 256 * ty_offset;
    rc = png.decode((void *)&priv, PNG_FAST_PALETTE);
    png.close();
  }
}

void do_map(uint8_t i, float lat, float lon, uint8_t zoom) {
  tx = long2tilex(lon, zoom);
  ty = lat2tiley(lat, zoom);
  si = i;  // used by do_tile() as index in basurl
  // Geographical borders of tile that contains the center location (center-tile):
  double NW_lon = tilex2long(tx, zoom);
  double NW_lat = tiley2lat(ty, zoom);
  // SE corner of tile (tx, ty) is NW corner of tile (tx+1, ty+1), so:
  double SE_lon = tilex2long(tx + 1, zoom);
  double SE_lat = tiley2lat(ty + 1, zoom);

  int cx = (int)(256 * (lon - NW_lon) / (SE_lon - NW_lon)); // x-coordinate center location within 256x256 tile
  int cy = (int)(256 * (lat - NW_lat) / (SE_lat - NW_lat)); // y-coordinate center location within 256x256 tile
  ctx = pixHor / 2 - cx;    // x-coordinate for posioning center-tile on display
  cty = pixVer / 2 - cy;    // y-coordinate for posioning center-tile on display

  WiFi.begin(WIFI_SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("WiFi OK");

  // Note: tile positions can be negative and tiles don't need to fit on the display completely
  if (cty > 0) {    // are extra tiles needed at the top?
    if (ctx > 0) {
      // Position(ctx - 256, cty - 256)
      do_tile(-1, -1, zoom);
    }

    // Position(ctx, cty - 256)
    do_tile(0, -1, zoom);

    if (ctx + 256 < pixHor ) {
      // Position(ctx + 256, cty - 256)
      do_tile(1, -1, zoom);
    }
  }

  // center tile and its neighbours:
  if (ctx > 0) {
    // Position(ctx - 256, cty)
    do_tile(-1, 0, zoom);
  }

  // Position(ctx, cty)
  do_tile(0, 0, zoom);

  if (ctx + 256 < pixHor) {
    // Position(ctx + 256, cty)
    do_tile(1, 0, zoom);
  }

  if (cty + 256 <  pixVer) {    // are extra tiles needed at the bottom?
    if (ctx > 0) {
      // Position(ctx - 256, cty + 256)
      do_tile(-1, 1, zoom);
    }

    // Position(ctx, cty + 256)
    do_tile(0, 1, zoom);

    if (ctx + 256 < pixHor ) {
      // Position(ctx + 256, cty + 256)
      do_tile(1, 1, zoom);
    }
  }
}

void calc_area(float clat, float clon, uint8_t zoom, uint16_t phor, uint16_t pver) {
  long omtrek = 40075016.686;
  double coslat = cos(clat * PI / 180.0);
  long sz = 256 * pow(2, zoom);
  myArea.degpp_ver = coslat * 360.0 / sz;
  myArea.degpp_hor = 360.0 / sz;
  float d_lat = myArea.degpp_ver * pver / 2;
  float d_lon = myArea.degpp_hor * phor / 2;
  myArea.border_n = clat + d_lat;
  myArea.border_s = clat - d_lat;
  myArea.border_w = clon - d_lon;
  myArea.border_e = clon + d_lon;
  myArea.mpp = (omtrek * coslat) / (sz);
  myArea.max_dist = 0.0005 * sqrt( pow(myArea.mpp * phor, 2) + pow(myArea.mpp * pver, 2));
}

void print_mapinfo() {
  Serial.println();
  Serial.print("Map completed; number of 256x256 tiles used for "); Serial.print(pixHor); Serial.print("x"); Serial.print(pixVer); Serial.print(" display: ");Serial.println(n_tiles);
  Serial.println();
  Serial.print("Displayed area is ");Serial.print((myArea.mpp * pixHor / 1000));Serial.print(" x ");Serial.print((myArea.mpp * pixVer / 1000));Serial.println(" km.");
  Serial.print("Latitude northern border:\t");Serial.println(myArea.border_n,5);
  Serial.print("Latitude southern border:\t");Serial.println(myArea.border_s,5);
  Serial.print("Longitude western border:\t");Serial.println(myArea.border_w,5);
  Serial.print("Longitude eastern border:\t");Serial.println(myArea.border_e,5);
  Serial.print("Meters per pixel:\t\t");Serial.println(myArea.mpp,1);
  Serial.print("Horizontal degrees per pixel:\t");Serial.println(myArea.degpp_hor,7);
  Serial.print("Vertical degrees per pixel:\t");Serial.println(myArea.degpp_ver,7);
  Serial.print("Center to corner distance (km):\t");Serial.println(myArea.max_dist,2);
}
