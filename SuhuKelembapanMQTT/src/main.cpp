#include <Arduino.h>
#include "config.h"
#include <ESP8266WiFi.h>
#include <DHT.h>

#define DHTPIN              2
#define DHTTYPE             DHT11
#define LED_PIN             2     // Built-in LED on GPIO2 (active-LOW)
#define LED_BLINK_INTERVAL  100   // Fast blink interval (ms)

DHT dht(DHTPIN, DHTTYPE);
unsigned long previousLedMillis = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  // LED off initially

  dht.begin();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("\n\r \n\rWorking to connect");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nDHT Weather Reading Server");
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Fast blink 10x to indicate WiFi connected
  for (int i = 0; i < 10; i++) {
    digitalWrite(LED_PIN, LOW);
    delay(100);
    digitalWrite(LED_PIN, HIGH);
    delay(100);
  }
}

void loop() {
  unsigned long currentMillis = millis();

  if (WiFi.status() == WL_CONNECTED) {
    if (currentMillis - previousLedMillis >= LED_BLINK_INTERVAL) {
      previousLedMillis = currentMillis;
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
  } else {
    digitalWrite(LED_PIN, HIGH);  // LED off when not connected
  }
}