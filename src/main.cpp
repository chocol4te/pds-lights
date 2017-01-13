/*
 * Written by Ferdia McKeogh, 2017
 *
 * data.bin format as follows:
 * version
 * power
 * brightness
 * mode
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <SoftwareSerial.h>

#define FINGERPRINT "***REMOVED***"
#define CURRENT_VERSION 0

WiFiClientSecure client;

const char* ssid = "***REMOVED***";
const char* password = "***REMOVED***";

const char* host = "***REMOVED***";
const int httpsPort = 443;
const char* url = "/esp8266/data.bin";

uint8_t version;
uint8_t power;
uint8_t brightness;
uint8_t mode;

void senddata() { // Sends processed data to 328P for light update

}

void updatefirmware() {

  Serial.print("Beginning update...");

  t_httpUpdate_return  ret = ESPhttpUpdate.update("https://***REMOVED***/esp8266/firmware.bin", "", FINGERPRINT, true);

  switch(ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;

    case HTTP_UPDATE_OK:
      Serial.println("HTTP_UPDATE_OK");
      break;
  }
}

void updatedata() {

  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection to ***REMOVED*** failed");
    return;
  }

  if (client.verify(FINGERPRINT, host)) {
  } else {
    Serial.println("Certificate doesn't match");
  }

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: pds_lights_esp8266\r\n" +
               "Connection: close\r\n\r\n");

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }

  version = client.read();
  power = client.read();
  brightness = client.read();
  mode = client.read();

  if (version != CURRENT_VERSION) {
    updatefirmware();
  }
}

void setup() {
  Serial.begin(19200);

  Serial.print("Booting version ");
  Serial.println(CURRENT_VERSION);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  updatedata();
  Serial.println(version);
  Serial.println(power);
  Serial.println(brightness);
  Serial.println(mode);
  delay(1000);
}
