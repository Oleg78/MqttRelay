#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include "settings.h"

#define RELAY_PIN 12
#define LED_BUILTIN 13

char msg[50];                      // Message to the topic
long lastReconnectMQTTAttempt = 0; // Time of the last MQTT connection attempt
long lastLedSwitch = 0;            // Time of the last OnBoard LED swithed

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("MQTT message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if ((char)payload[0] == '1') {
    if (relay_on() == 1) {
      Serial.println("Relay swithed on");
    } else {
      Serial.println("Relay is already on");
    }
  } else {
    if (relay_off() == 1) {
      Serial.println("Relay swithed off");
    } else {
      Serial.println("Relay is already off");
    }
  }

}

int relay_on(void) {
  if (relay_state == LOW) {
    relay_state = HIGH;
    digitalWrite(RELAY_PIN, HIGH);
    if (client.connected()) {
      sprintf(msg, "%s", String(1).c_str());
      client.publish(TOPIC_RELAY_STATE, msg);
    }
    return 1;
  } else {
    return 0;
  }
}

int relay_off(void) {
  if (relay_state == HIGH) {
    relay_state = LOW;
    digitalWrite(RELAY_PIN, LOW);
    if (client.connected()) {
      sprintf(msg, "%s", String(0).c_str());
      client.publish(TOPIC_RELAY_STATE, msg);
    }
    return 1;
  } else {
    return 0;
  }
}

boolean connectMQTT(){
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  Serial.print("Attempting MQTT connection...");
  // Attempt to connect
  if (client.connect(CLIENT_NAME)) {
    Serial.println("connected");
    client.subscribe(TOPIC_RELAY_CONTROL);
  } else {
    Serial.print("Connection failed, rc=");
    Serial.print(client.state());
  }
  client.connected();
}

void connectWIFI(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  // Wait for connection
  int i = 0;
  while (WiFi.status() != WL_CONNECTED or i < 10) {
    i++;
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.print("WIFI not yet connected...");
  }
}


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, led_state);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, relay_state);
  Serial.begin(115200);
  Serial.println("");
  connectWIFI();
  connectMQTT();
}

// the loop function runs over and over again forever
void loop() {
  long now = millis();
  
  if (!client.connected()) {
    if (now - lastReconnectMQTTAttempt > 5000) {
      lastReconnectMQTTAttempt = now;
      if (connectMQTT()) {
        lastReconnectMQTTAttempt = 0;
      }
    }
  } else {
    client.loop();
  }

  // I am alive :)
  if (now - lastLedSwitch > 500) {
    lastLedSwitch = now;
    if (led_state == HIGH) {
      digitalWrite(LED_BUILTIN, LOW);
      led_state = LOW;
    } else {
      digitalWrite(LED_BUILTIN, HIGH);
      led_state = HIGH;
    }
  }
  
  delay(500);
}
