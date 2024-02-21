#define WIFI_SSID "<WiFi_SSID>"
#define PASSWORD "<WiFi_password>"

const uint16_t pixHor = 480;
const uint16_t pixVer = 320;
const uint8_t orientation = 1; // landscape

// uncomment the following line if your board has PSRAM and you want the displayed map to be stored in PSRAM as well
//#define HAS_PSRAM

// A map tile provider is identified by its index, passed as first parameter to function do_map()
const char *baseurl[] = {                                         // INDEX
  "https://c.tile.opentopomap.org",                               // 0  well suited for GPS apps; quite fast
  "https://tile.openstreetmap.org",                               // 1  familiar OSM map
  "https://a.tile.openstreetmap.fr/hot",                          // 2  nice 'humanitarean' OSM style
  "https://a.tile.openstreetmap.fr/osmfr",                        // 3  clear and fast
  "https://cartodb-basemaps-1.global.ssl.fastly.net/dark_all"     // 4  night impression; very fast
};
const uint8_t n_urls = sizeof(baseurl) / sizeof(baseurl[0]); 

// desired map center location and zoom level
double lon = -1.82613;   
double lat = 51.18698;  
uint8_t z = 10;
