// MQTT settings
#define CLIENT_NAME "relay-02"
#define TOPIC_RELAY_STATE "root/"CLIENT_NAME"/state"
#define TOPIC_RELAY_CONTROL "root/"CLIENT_NAME"/control"

// Initial relay state
int relay_state = HIGH;
// Initial LED state
int led_state = HIGH;


// Wi-Fi settings
const char* ssid = "wifi_name";
const char* password = "wifi_pass";

// MQTT server
const char* mqtt_server = "192.168.0.1";
const int mqtt_port = 1883;