#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <math.h>

#define ssid  ""
#define pswd  ""
#define mqttServer  ""

#define DHTTYPE DHT22
#define DHTPIN  15

WiFiClient wirelessClient;
PubSubClient mqttClient(wirelessClient);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.println("Starting...");

  WiFi.begin(ssid, pswd);
  Serial.println("Connecting to WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected! IP: ");
  Serial.println(WiFi.localIP());

  mqttClient.setServer(mqttServer, 1883);
  if (mqttClient.connect("office")) {
    Serial.println("Connected to MQTT!");
  } else {
    Serial.println("Failed to connect to MQTT.");
  }

  dht.begin();
}

void loop() {
  mqttClient.loop();

  mqttClient.subscribe("temperature");

  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temp) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    char tempString[8];
    dtostrf(temp, 6, 2, tempString);
    if (mqttClient.publish("temperature", tempString)) {
      Serial.print("Published temperature: ");
      Serial.println(temp);
    } else {
      Serial.println("Error publishing temperature.");
    }
    char humidityString[8];
    dtostrf(humidity, 6, 2, humidityString);
    if (mqttClient.publish("humidity", humidityString)) {
      Serial.print("Published humidity: ");
      Serial.println(humidity);
    } else {
      Serial.println("Error publishing humidity.");
    }
  }

  delay(5000);
}
