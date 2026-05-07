#include <Arduino.h>
#include "config.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>
#include <DHT.h>
#include <PubSubClient.h>
#include <memory>
#include <time.h>

#define DHTPIN              2
#define DHTTYPE             DHT11
#define PUBLISH_INTERVAL    5000

DHT dht(DHTPIN, DHTTYPE);
BearSSL::WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);
std::unique_ptr<BearSSL::X509List> mqttCaCert;
unsigned long lastPublishMillis = 0;

void syncClock() {
  configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  Serial.print("Syncing NTP time");

  time_t now = time(nullptr);
  uint8_t retries = 0;
  while (now < 1700000000 && retries < 30) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
    retries++;
  }

  if (now >= 1700000000) {
    Serial.println(" ok");
    wifiClient.setX509Time(now);
  } else {
    Serial.println(" failed (using local clock)");
  }
}

void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  WiFi.begin(AppConfig::WIFI_SSID, AppConfig::WIFI_PASSWORD);
  Serial.print("\n\r \n\rWorking to connect");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nDHT Weather Reading Server");
  Serial.print("Connected to ");
  Serial.println(AppConfig::WIFI_SSID);
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
    if (mqttClient.connect(clientId, AppConfig::MQTT_USER, AppConfig::MQTT_PASS)) {
      Serial.println("connected");
    } else {
      char sslError[128];
      wifiClient.getLastSSLError(sslError, sizeof(sslError));
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.print(", tls=");
      Serial.print(sslError);
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

  bool suhuOk = mqttClient.publish(AppConfig::MQTT_TOPIC_SUHU, suhuPayload, true);
  bool kelembapanOk = mqttClient.publish(AppConfig::MQTT_TOPIC_KELEMBAPAN, kelembapanPayload, true);

  if (suhuOk && kelembapanOk) {
    Serial.print("Published suhu to ");
    Serial.print(AppConfig::MQTT_TOPIC_SUHU);
    Serial.print(": ");
    Serial.println(suhuPayload);

    Serial.print("Published kelembapan to ");
    Serial.print(AppConfig::MQTT_TOPIC_KELEMBAPAN);
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
  syncClock();

  if (AppConfig::MQTT_TLS_INSECURE) {
    Serial.println("WARNING: MQTT TLS hostname verification disabled (insecure)");
    wifiClient.setInsecure();
  } else {
    mqttCaCert.reset(new BearSSL::X509List(AppConfig::MQTT_CA_CERT));
    wifiClient.setTrustAnchors(mqttCaCert.get());
  }

  mqttClient.setServer(AppConfig::MQTT_SERVER, AppConfig::MQTT_PORT);
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