#include "TgtgApi.h"

TgtgApi::TgtgApi() {}

void TgtgApi::init(const char *fingerprint, SSD1306Wire *_display)
{
    _wifiClient.setFingerprint(fingerprint);
    this->_display = _display;
    this->refresh_api_token();
    this->refresh_items();
}

void TgtgApi::update()
{
    if ((millis() - _last_refresh_token_time) > ACCESS_TOKEN_LIFETIME)
    {
        refresh_api_token();
    }

    if ((millis() - _last_refresh_items_time) > ITEMS_REFRESH_INTERVAL)
    {
        refresh_items();
    }
}

void TgtgApi::refresh_api_token()
{
    Serial.println("\n> refresh_api_token");

    _display->clear();
    _display->setFont(ArialMT_Plain_16);
    _display->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    _display->drawString(64, 32, "Refreshing API\naccess token...");
    _display->display();

    _httpClient.begin(_wifiClient, REFRESH_URL);
    _httpClient.setReuse(true);
    _httpClient.setUserAgent(USER_AGENT);

    char authBearer[strlen(BEARER_FORMAT) + strlen(access_token) + 1];
    sprintf(authBearer, BEARER_FORMAT, access_token);

    _httpClient.addHeader("content-type", "application/json");
    _httpClient.addHeader("accept-language", "pl-PL");
    _httpClient.addHeader("Accept-Encoding", "gzip");
    _httpClient.addHeader("authorization", authBearer, false, false);

    const char *headerkeys[] = {"Set-Cookie", "Cookie"};
    size_t headerkeyssize = sizeof(headerkeys) / sizeof(char *);
    _httpClient.collectHeaders(headerkeys, headerkeyssize);

    char payload[strlen(REFRESH_TOKEN_FORMAT) + strlen(refresh_token) + 1];
    sprintf(payload, REFRESH_TOKEN_FORMAT, refresh_token);
    Serial.print("payload = ");
    Serial.println(payload);

    int httpCode = _httpClient.POST(payload);
    Serial.print("httpCode = ");
    Serial.println(httpCode);

    if (httpCode != 200)
    {
        _display->clear();
        _display->setFont(ArialMT_Plain_16);
        _display->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
        _display->drawString(64, 32, "refresh_api_token error\nCode: " + httpCode);
        _display->display();
        return;
    }

    Serial.print("http response = ");
    Serial.println(_httpClient.getString());

    DynamicJsonDocument doc(512);
    deserializeJson(doc, _httpClient.getString());

    String new_access_token = doc["access_token"];
    Serial.print("new access_token = ");
    Serial.println(new_access_token);

    strcpy(access_token, new_access_token.c_str());

    _last_refresh_token_time = millis();

    _display->clear();
    _display->setFont(ArialMT_Plain_16);
    _display->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    _display->drawString(64, 32, "Done!");
    _display->display();
    delay(1000);
    _display->clear();
    _display->display();
}

void TgtgApi::refresh_items()
{
    Serial.println("\n> refresh_items");

    _display->clear();
    _display->setFont(ArialMT_Plain_16);
    _display->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    _display->drawString(64, 32, "Loading items...");
    _display->display();

    if (_httpClient.hasHeader("Set-Cookie") == false)
    {
        Serial.println("Can't find cookie");
        _last_refresh_items_time = millis();

        _display->clear();
        _display->setFont(ArialMT_Plain_16);
        _display->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
        _display->drawString(64, 32, "Error\nCan't find cookie");
        _display->display();
        return;
    }

    String cookie = _httpClient.header("Set-Cookie");
    Serial.print("Found cookie: ");
    Serial.println(cookie);

    char authBearer[strlen(BEARER_FORMAT) + strlen(access_token) + 1];
    sprintf(authBearer, BEARER_FORMAT, access_token);

    int8_t items_count = 0;
    int8_t items_page = 1;

    while (true)
    {
        _httpClient.begin(_wifiClient, ITEMS_URL);
        _httpClient.setReuse(true);
        _httpClient.setUserAgent(USER_AGENT);
        _httpClient.addHeader("content-type", "application/json");
        _httpClient.addHeader("accept-language", "pl-PL");
        _httpClient.addHeader("Accept-Encoding", "gzip");
        _httpClient.addHeader("Cookie", cookie);
        _httpClient.addHeader("authorization", authBearer, false, false);

        char payload[256];
        sprintf(payload, REFRESH_ITEMS_FORMAT, user_id, LAT, LON, RADIUS, items_page);
        Serial.print("payload = ");
        Serial.println(payload);

        int httpCode = _httpClient.POST(payload);
        Serial.print("httpCode = ");
        Serial.println(httpCode);

        if (httpCode != 200)
        {
            _display->clear();
            _display->setFont(ArialMT_Plain_16);
            _display->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
            _display->drawString(64, 32, "refresh_items error\nCode: " + httpCode);
            _display->display();
            break;
        }

        String response = _httpClient.getString();
        Serial.print("http response = ");
        Serial.println(response);

        if (response.length() > 12) // {"items":[]}
        {
            String item_name = get_item_name(response);
            Serial.print("item_name = ");
            Serial.println(item_name);

            if (items_count == 0)
            {
                _display->clear();
            }
            _display->setFont(ArialMT_Plain_10);
            _display->setTextAlignment(TEXT_ALIGN_LEFT);
            _display->drawString(0, 12 * items_count, "- " + item_name);
            _display->display();

            items_count++;
            items_page++;
        }
        else
        {
            break;
        }
    }

    if (items_count == 0)
    {
        _display->clear();
        _display->setFont(ArialMT_Plain_16);
        _display->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
        _display->drawString(64, 32, "No items :(");
        _display->display();
    }

    _last_refresh_items_time = millis();
}

String TgtgApi::get_item_name(String item_json)
{
    Serial.println("\n> get_item_name");

    // Ex. input:
    // [...],"uses_ecommerce_model":false},"display_name":"This is shop name","pickup_interval"[...]

    // 1. Find `display_name` in JSON string and get 32 chars after it
    char needle[] = "\"display_name\":";
    char *ret = strstr(item_json.c_str(), needle);
    char display_name[32];
    strncpy(display_name, ret + sizeof(needle), sizeof(display_name) - 2);
    display_name[sizeof(display_name) - 1] = '\0';

    // 2. Take text until first `"` character
    // At this point `display_name` = `This is shop name","pickup_inter\0`
    char *pch = strtok(display_name, "\"");
    if (pch != NULL)
    {
        return pch; // `This is shop name`
    }
    else
    {
        return "???";
    }
}
