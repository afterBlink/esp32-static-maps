#define WIFI_SSID "<WiFi_SSID>"
#define PASSWORD "<WiFi_password>"

const uint16_t pixHor = 480;
const uint16_t pixVer = 320;
const uint8_t orientation = 1; // landscape

// uncomment this line if your board has PSRAM and you want the displayed map to be stored in PSRAM as well
//#define HAS_PSRAM

// A map tile provider is identified by its index, passed as first parameter to function do_map()
const char *baseurl[] = {                                       // INDEX
  "https://tiles.wmflabs.org/osm-no-labels",                      // 0  no texts; rather slow (occasionally a tile may fail) 
  "https://tiles.wmflabs.org/hikebike",                           // 1  clear map; text has white background 
  "https://tiles.wmflabs.org/bw-mapnik",                          // 2  nice map; text has white background
  "https://c.tile.opentopomap.org",                               // 3  well suited for GPS apps; quite fast
  "https://tile.openstreetmap.org",                               // 4  familiar OSM map
  "https://a.tile.openstreetmap.fr/hot",                          // 5  nice 'humanitarean' OSM style
  "https://a.tile.openstreetmap.fr/osmfr",                        // 6  clear and fast
  "https://tile.memomaps.de/tilegen",                             // 7  clear and reasonably fast
  "https://cartodb-basemaps-1.global.ssl.fastly.net/dark_all",    // 8  night impression; very fast
  "http://a.tile.stamen.com/toner"                                // 9  just black & white; super fast (small png files)
};
const uint8_t n_urls = sizeof(baseurl) / sizeof(baseurl[0]); 

// desired map center location and zoom level
double lon = 6.183;   
double lat = 51.354;  
uint8_t z = 10;
