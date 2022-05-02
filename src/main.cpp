#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include "Led.h"
#include "SSD1306Wire.h"
#include "TgtgApi.h"

void connect_to_wifi();

const char *ssid = "---";
const char *password = "---";
const char *fingerprint = "---";

Led ledBuiltin = Led(LED_BUILTIN);
SSD1306Wire display(0x3c, SDA, SCL);
TgtgApi tgtgApi = TgtgApi();

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  Serial.println("\nHello world\n");

  ledBuiltin.init(true);

  display.init();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  display.drawString(64, 32, "Too Good To Go\nnotifier\nv1.0");
  display.display();
  delay(1000);
  display.clear();
  display.display();

  connect_to_wifi();

  if (WiFi.status() == WL_CONNECTED)
  {
    tgtgApi.init(fingerprint, &display);
  }
}

void loop()
{
  tgtgApi.update();
  ledBuiltin.dim(1000); // lasts 1s
  Serial.print(".");
}

void connect_to_wifi()
{
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  display.clear();

  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  display.drawString(64, 32, "Connecting\nto Wi-Fi...");
  display.display();

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(200);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  display.drawString(64, 32, "Connected!\n" + WiFi.localIP().toString());
  display.display();
  delay(1000);
  display.clear();
  display.display();
}
