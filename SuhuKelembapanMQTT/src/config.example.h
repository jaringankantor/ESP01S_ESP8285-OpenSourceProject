#ifndef SUHU_KELEMBAPAN_MQTT_CONFIG_H
#define SUHU_KELEMBAPAN_MQTT_CONFIG_H

// Salin file ini menjadi config.h lalu isi dengan kredensial asli.
// Jangan commit config.h ke Git.

namespace AppConfig {

static constexpr const char* WIFI_SSID = "your_wifi_ssid";
static constexpr const char* WIFI_PASSWORD = "your_wifi_password";

static constexpr const char* MQTT_SERVER = "your_mqtt_broker_ip";
static constexpr const char* MQTT_TLS_SERVER_NAME = "mqtt.example.com";
static constexpr uint16_t MQTT_PORT = 8883;
static constexpr bool MQTT_TLS_INSECURE = false;
static constexpr const char* MQTT_USER = "your_mqtt_username";
static constexpr const char* MQTT_PASS = "your_mqtt_password";
static constexpr const char* MQTT_TOPIC_SUHU = "rumah/sensor/suhu";
static constexpr const char* MQTT_TOPIC_KELEMBAPAN = "rumah/sensor/kelembapan";
static constexpr const char* MQTT_CA_CERT = R"CERT(
-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)CERT";

}  // namespace AppConfig

#endif
