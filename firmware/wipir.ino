/*
 * PIRMotionSensor.ino
 * Example sketch for the PIR motion sensor
 *
 * Copyright (c) 2017 Jesus Cano
 * Website    : http://jesus.engineer
 * Author     : Jesus Cano
 * Create Time: Sep 18,2017
 * Change Log :
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>

#define LED	5
#define WIFI_NAME "xxxxx"
#define WIFI_PASSWORD "xxxxx"
#define POST_URL "xxxxx"

RTC_DATA_ATTR int detectionCount = 0;

void setup() {
  
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  Serial.begin(115200);
  Serial.println();

  ++detectionCount;
  Serial.println("Detection count: " + String(detectionCount));
  print_wakeup_reason();
  
  Serial.print("Sending to internet...\n");
  sendInternet();
  Serial.print("Done sending\n");  

  delay(5000);
  
  esp_deep_sleep_enable_ext0_wakeup(GPIO_NUM_4, 1);
  Serial.println("Going to sleep now");
  Serial.println();
  Serial.println();
  Serial.println();
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

void loop() {
  //This is not going to be called
}

void print_wakeup_reason(){
  
  esp_deep_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_deep_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case 1  : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case 2  : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case 3  : Serial.println("Wakeup caused by timer"); break;
    case 4  : Serial.println("Wakeup caused by touchpad"); break;
    case 5  : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.println("Wakeup was not caused by deep sleep"); break;
  }
}

void sendInternet() {

  WiFiMulti wifiMulti;

  Serial.print("Adding AP...");
  wifiMulti.addAP(WIFI_NAME, WIFI_PASSWORD);
  Serial.print("done\n");
  
  // wait for WiFi connection
  if((wifiMulti.run() == WL_CONNECTED)) {
    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    //http.addHeader("User-Agent", "ESP32/Thing"); // does not work
    http.begin(POST_URL);
    
    Serial.print("[HTTP] POST...\n");
    int httpCode = http.POST("");

    if(httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] POST... code: %d\n", httpCode);

        // file found at server
        if(httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            Serial.printf("[HTTP] POST... response: %s\n", payload.c_str());
        }
    } else {
        Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
}
