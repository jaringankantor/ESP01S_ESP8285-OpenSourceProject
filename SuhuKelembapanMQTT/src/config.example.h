#ifndef CONFIG_H
#define CONFIG_H

// Salin file ini menjadi config.h lalu isi dengan kredensial asli.
// Jangan commit config.h ke Git.

#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"

#define MQTT_SERVER "your_mqtt_broker_ip"
#define MQTT_PORT "your_mqtt_broker_port"
#define MQTT_USER "your_mqtt_username"
#define MQTT_PASS "your_mqtt_password"
#define MQTT_TOPIC_SUHU "rumah/sensor/suhu"
#define MQTT_TOPIC_KELEMBAPAN "rumah/sensor/kelembapan"
#define MQTT_CA_CERT R"EOF(
-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)EOF"

#endif
