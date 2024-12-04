#ifndef CONNECTION_DETAILS_H
#define CONNECTION_DETAILS_H

const char *ssid = "My WiFi";
const char *password = "My Password";


const char *mqtt_server = "192.168.1.240";
const char *mqtt_username = "mqttuser";
const char *mqtt_password = "mqttpassword";

// Device status information
const char *mqtt_sensor_topic = "topic/bme/status";
const char *mqtt_reading_topic = "topic/bme/reading";
const char *mqtt_signal_topic = "topic/bme/signal";
// Device readings
const char *mqtt_temp_topic = "topic/temperature";
const char *mqtt_pressure_topic = "topic/pressure";
const char *mqtt_humidity_topic = "topic/humidity";
#endif
