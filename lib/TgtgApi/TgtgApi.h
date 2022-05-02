#ifndef TGTG_H
#define TGTG_H

#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "SSD1306Wire.h"

#define REFRESH_URL "https://apptoogoodtogo.com/api/auth/v3/token/refresh"
#define ITEMS_URL "https://apptoogoodtogo.com/api/item/v7/"

#define USER_AGENT "TGTG/21.12.1 Dalvik/2.1.0 (Linux; U; Android 7.0; SM-G935F Build/NRD90M)"
#define BEARER_FORMAT "Bearer %s"
#define REFRESH_TOKEN_FORMAT "{\"refresh_token\":\"%s\"}"
#define REFRESH_ITEMS_FORMAT "{\"user_id\":\"%s\",\"origin\":{\"latitude\":%s,\"longitude\":%s},\"radius\":%s,\"page_size\":1,\"page\":%d,\"with_stock_only\":\"true\"}"

#define ACCESS_TOKEN_LIFETIME 3600000 // 1h
#define ITEMS_REFRESH_INTERVAL 30000  // 30s

#define LAT "52.2297" // Warsaw
#define LON "21.0122"
#define RADIUS "3.0"

class TgtgApi
{
public:
    TgtgApi();

    void init(const char *fingerprint, SSD1306Wire *display);
    void update();

private:
    HTTPClient _httpClient;
    WiFiClientSecure _wifiClient;
    SSD1306Wire *_display;

    long _last_refresh_token_time = 0L;
    long _last_refresh_items_time = 0L;

    const char *user_id = "---";
    const char *refresh_token = "---";
    char *access_token = "---";

    void refresh_api_token();
    void refresh_items();
    String get_item_name(String item_json);
};

#endif