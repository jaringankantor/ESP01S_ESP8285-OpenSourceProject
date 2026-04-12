#include <Arduino.h>
#include "config.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>
#include <DHT.h>
#include <PubSubClient.h>
#include <memory>
#include <stdlib.h>

#define DHTPIN              2
#define DHTTYPE             DHT11
#define PUBLISH_INTERVAL    5000
#define DEFAULT_MQTT_PORT   8883

DHT dht(DHTPIN, DHTTYPE);
BearSSL::WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);
std::unique_ptr<BearSSL::X509List> mqttCaCert;
unsigned long lastPublishMillis = 0;

void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

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
}

void connectMQTT() {
  if (mqttClient.connected()) {
    return;
  }

  char clientId[32];
  snprintf(clientId, sizeof(clientId), "esp8285-%06X", ESP.getChipId());

  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT...");
    if (mqttClient.connect(clientId, MQTT_USER, MQTT_PASS)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" retry in 5 seconds");
      delay(5000);
    }
  }
}

void publishSensorData() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor");
    return;
  }

  char suhuPayload[16];
  char kelembapanPayload[16];

  dtostrf(temperature, 0, 2, suhuPayload);
  dtostrf(humidity, 0, 2, kelembapanPayload);

  bool suhuOk = mqttClient.publish(MQTT_TOPIC_SUHU, suhuPayload, true);
  bool kelembapanOk = mqttClient.publish(MQTT_TOPIC_KELEMBAPAN, kelembapanPayload, true);

  if (suhuOk && kelembapanOk) {
    Serial.print("Published suhu to ");
    Serial.print(MQTT_TOPIC_SUHU);
    Serial.print(": ");
    Serial.println(suhuPayload);

    Serial.print("Published kelembapan to ");
    Serial.print(MQTT_TOPIC_KELEMBAPAN);
    Serial.print(": ");
    Serial.println(kelembapanPayload);
  } else {
    Serial.println("Failed to publish one or more MQTT messages");
  }
}

void setup() {
  Serial.begin(115200);

  dht.begin();
  connectWiFi();

  mqttCaCert.reset(new BearSSL::X509List(MQTT_CA_CERT));
  wifiClient.setTrustAnchors(mqttCaCert.get());

  uint16_t mqttPort = static_cast<uint16_t>(atoi(MQTT_PORT));
  if (mqttPort == 0) {
    mqttPort = DEFAULT_MQTT_PORT;
    Serial.print("Invalid MQTT_PORT, fallback to ");
    Serial.println(mqttPort);
  }

  mqttClient.setServer(MQTT_SERVER, mqttPort);
  connectMQTT();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!mqttClient.connected()) {
    connectMQTT();
  }

  mqttClient.loop();

  unsigned long currentMillis = millis();
  if (currentMillis - lastPublishMillis >= PUBLISH_INTERVAL) {
    lastPublishMillis = currentMillis;
    publishSensorData();
  }
}