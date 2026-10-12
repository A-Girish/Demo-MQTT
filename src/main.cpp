#include <WiFi.h>
#include <PubSubClient.h>

// --- WiFi ---
const char* WIFI_SSID = "Me";
const char* WIFI_PASSWORD = "12345678";

// --- MQTT ---
const char* MQTT_BROKER = "broker.hivemq.com"; // public test broker, replace with your own
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC = "machinemonitoring/ldr/Girish";
const char* MQTT_CLIENT_ID = "Girish";

// --- LDR ---
const int LDR_PIN = 34; // GPIO4 / "34" on ESP32

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void connectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT...");
    if (mqttClient.connect(MQTT_CLIENT_ID)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" retrying in 2s");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  connectWiFi();
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }
  if (!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop();

  int ldrValue = analogRead(LDR_PIN);
  char payload[8];
  itoa(ldrValue, payload, 10);

  mqttClient.publish(MQTT_TOPIC, payload);
  Serial.print("Published LDR value: ");
  Serial.println(ldrValue);

  delay(2000);
}
