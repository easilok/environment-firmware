#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "ConnectionDetails.h"

#define SEALEVELPRESSURE_HPA (1013.25)
#define MQTT_CONNECT_LED 1000

Adafruit_BME280 bme;

float temperature, humidity, pressure, altitude;
unsigned long readingCounter = 0;

unsigned long ledToggleTimer = 0;
int ledState = LOW;

/* MQTT */
#define MQTT_RECONNECT_STANDBY 5000
#define SENSOR_PUBLISH_TIMER 2000
#define SENSOR_SLEEP_TIMER 13000

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long mqttReconnectTimer = 0, sensorPublishTimer = 0;

void setup() {
  Serial.begin(115200);
  delay(100);

  bme.begin(0x76);

  Serial.println("Connecting to ");
  Serial.println(ssid);

  pinMode(LED_BUILTIN, OUTPUT); // Initialize the LED_BUILTIN pin as an output

  wifi_reconnect();

  randomSeed(micros());

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    wifi_reconnect();
  }

  /* MQTT */
  if (!client.connected()) {
    mqtt_reconnect();
  }
  mqtt_handle_sending_data();
  client.loop();

  /* LED Signal Running */
  if (!client.connected()) {
    if (millis() - ledToggleTimer > MQTT_CONNECT_LED) {
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }
      ledToggleTimer = millis();
    }
  } else if ((WiFi.status() == WL_CONNECTED) && (client.connected())) {
    ledState = HIGH;
  }
  digitalWrite(LED_BUILTIN, ledState);
}

void readSensorData() {
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = bme.readPressure() / 100.0F;
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  readingCounter++;
}

void mqtt_handle_sending_data() {
  if (client.connected()) {
    if (millis() - sensorPublishTimer > SENSOR_PUBLISH_TIMER) {
      readSensorData();
      publish_sensor_readings(temperature, humidity, pressure, altitude,
                              readingCounter);
      sensorPublishTimer = millis();
      // ESP.deepSleep(SENSOR_SLEEP_TIMER);
      delay(SENSOR_SLEEP_TIMER);
    }
  }
}

void publish_sensor_readings(float temperature, float humidity, float pressure, float altitude, unsigned long readingCounter) {
  char itoaTemp[10] = {0};
  if (client.connected()) {
    Serial.println("");
    Serial.println("Publishing sensor data..!");
    // client.publish(mqtt_sensor_topic, "Connected");
    itoa(temperature, itoaTemp, 10);
    if (!client.publish(mqtt_temp_topic, itoaTemp)) {
      Serial.println("Failed to publish temperature");
    }
    itoa(humidity, itoaTemp, 10);
    if (!client.publish(mqtt_humidity_topic, itoaTemp)) {
      Serial.println("Failed to publish humidity");
    }
    // itoa(pressure, itoaTemp, 10);
    // if (!client.publish(mqtt_pressure_topic, itoaTemp)) {
    //   Serial.println("Failed to publish pressure");
    // }
    itoa(readingCounter, itoaTemp, 10);
    if (!client.publish(mqtt_reading_topic, itoaTemp)) {
      Serial.println("Failed to publish reading count");
    }
    itoa(WiFi.RSSI(), itoaTemp, 10);
    if (!client.publish(mqtt_signal_topic, itoaTemp)) {
      Serial.println("Failed to publish signal strength");
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    // digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is
    // the voltage level but actually the LED is on; this is because it is
    // active low on the ESP-01)
  } else {
    // digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the
    // voltage HIGH
  }
}

void wifi_reconnect() {

  // connect to your local wi-fi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    // turn led on
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    // turn led off
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());

  ledToggleTimer = millis();
}

void mqtt_reconnect() {
  // If not connected to wifi -> don't attempt connection
  if (WiFi.status() != WL_CONNECTED)
    return;
  if (client.connected())
    return;
  // Loop until we're reconnected
  if (millis() - mqttReconnectTimer > MQTT_RECONNECT_STANDBY) {
    mqttReconnectTimer = millis();
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password,
                       mqtt_sensor_topic, 0, true, "Disconnected")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(mqtt_sensor_topic, "Connected");
      sensorPublishTimer = millis() + SENSOR_PUBLISH_TIMER;
      // ... and resubscribe
      // client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
    }
  }
}
