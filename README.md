# esp32_static_maps
Display geographical maps, assembled from downloaded tiles, on ESP32 + TFT display

<p align = "center"><iframe width="560" height="315" src="https://www.youtube.com/embed/p1Tn0YmqP0o" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe></p>

Demo of a simple static map alternative for OpenStreetMap (apparently stopped their static map service) or Google Maps (wants your credit card data). Should run on any ESP32 board + TFT_eSPI supported display with sizes up to 480x320 pixels. WiFi credentials, display size, tile provider, map center and zoom level can be set in "settings.h", where you can also indicate the use of PSRAM for storing the map. Don't forget to select the correct setup file for your display in the TFT_eSPI library.

Relies on free map tile services found on https://wiki.openstreetmap.org/wiki/Tile_servers.
Please note the following quotes from the OpenStreetMap wiki:

**"It is highly recommended to consult and follow usage policies of a given server! Respect their terms of use/tile usage policy."**

**"Please don't forget to credit OpenStreetMap. See the Legal FAQ for details. Some tile services require their own attribution in addition. Ask their operator for details."**

Also note that any tile service, or its backing from OpenStreetMap, may change/stop at any time. 

----------------------------------------------------------------------------------------------------

Credits:

Display driver by Bodmer: https://github.com/Bodmer/TFT_eSPI  
PNG decoder by bitbank2 (Larry Bank): https://github.com/bitbank2/PNGdec  
Thanks to OpenStreetMap and the tile service providers for sharing map tiles. Donations will help OpenStreetMaps to stay Open!
