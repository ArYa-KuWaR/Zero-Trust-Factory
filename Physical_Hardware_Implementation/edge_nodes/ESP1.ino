#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// WiFi Credentials
const char* ssid        = "Aadi";
const char* password    = "123456789";

// ✅ Static IP Configuration for your phone hotspot
IPAddress local_IP(10, 128, 99, 207); 
IPAddress gateway(10, 128, 99, 88);  
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(10, 128, 99, 88);

// MQTT Configuration - USING INSECURE PORT 1883
const char* mqtt_server = "10.128.99.193"; 
const int   mqtt_port      = 1883;  // ✅ Changed from 8883
const char* mqtt_client_id = "esp32-client-1";
const char* topic_temp     = "ot/level1/sensor/esp32-1/temperature";
const char* topic_hum      = "ot/level1/sensor/esp32-1/humidity";

#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// ✅ Switching to Standard WiFiClient (No SSL)
WiFiClient espClient; 
PubSubClient client(espClient);

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  if (!WiFi.config(local_IP, gateway, subnet, dns)) {
    Serial.println("STA Failed to configure");
  }
  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nWiFi connected: " + WiFi.localIP().toString());
}

void connectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting MQTT...");
    // Since 'allow_anonymous true' is in your config, we don't need credentials
    if (client.connect(mqtt_client_id)) {
      Serial.println("connected ✅");
    } else {
      Serial.printf("failed rc=%d, retry in 5s\n", client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  dht.begin();
  
  connectWiFi();

  // No setClock() or Certificates needed for Port 1883
  client.setServer(mqtt_server, mqtt_port);
  connectMQTT();
}

void loop() {
  if (!client.connected()) connectMQTT();
  client.loop();

  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("DHT22 read failed");
  } else {
    char t[8], h[8];
    dtostrf(temp, 4, 2, t);
    dtostrf(hum,  4, 2, h);
    client.publish(topic_temp, t);
    client.publish(topic_hum,  h);
    Serial.printf("Temp: %s°C  Hum: %s%%\n", t, h);
  }
  delay(5000);
}
