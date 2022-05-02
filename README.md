# too_good_too_go_esp8266			[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

##### An ESP8266 based project to display current available items from [Too Good Too Go](https://toogoodtogo.com/) (unofficial) API on OLED display.
___

### Software
To use, import project in [PlatformIO](https://platformio.org/).
Then you have to fill some data:

- `user_id`, `refresh_token`, `access_token` ← Your TGTG account credentials. You can retrieve them using [this library](https://github.com/ahivert/tgtg-python)
- `ssid`, `password` ← Your WiFi credentials
- `fingerprint` ← current TGTG API fingerprint
- `LAT`, `LON`, `RADIUS` ← The latitude, longitude and radius (in km) of area in which you want to search for parcels

You can also configure refresh time by changing `ITEMS_REFRESH_INTERVAL`. Default is 30s.
___

### Hardware
- ESP8266 - I used Wemos d1 mini v3.0
- SSD1306 OLED 128x64 Display

<p align="left">
<img src="assets/gif1.gif" height="400">
</p>
