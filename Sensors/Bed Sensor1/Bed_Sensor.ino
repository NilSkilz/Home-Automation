/*

  _   _ ___ _     ____  _  _____ _     _____
 | \ | |_ _| |   / ___|| |/ /_ _| |   |__  /
 |  \| || || |   \___ \| ' / | || |     / / 
 | |\  || || |___ ___) | . \ | || |___ / /_ 
 |_| \_|___|_____|____/|_|\_\___|_____/____|
                                            
  
  - Support for the ESP8266 boards. 
        - You can add it to the board manager by going to File -> Preference and pasting http://arduino.esp8266.com/stable/package_esp8266com_index.json into the Additional Board Managers URL field.
        - Next, download the ESP8266 dependancies by going to Tools -> Board -> Board Manager and searching for ESP8266 and installing it.
  
  - You will also need to download the follow libraries by going to Sketch -> Include Libraries -> Manage Libraries
      - PubSubClient
      - HX711  
*/

#include <Arduino.h>
#include <HX711.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "config.h"

int OTAport = 8266;

HX711 scale;                          // Initiate HX711 library
WiFiClient wifiClient;                // Initiate WiFi library
PubSubClient client(wifiClient);      // Initiate PubSubClient library


/********************************** START SETUP *****************************************/

void setup() {
  Serial.begin(74880);
  Serial.println();
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting...");

  while (WiFi.status() != WL_CONNECTED) {       // Wait till Wifi connected
    delay(500);
    Serial.print(".");
  }

  setupOTA();

  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());                     // Print IP address

  client.setServer(MQTT_SERVER, 1883);                // Set MQTT server and port number
  client.setCallback(callback);                       // Set callback address, this is used for remote tare
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);   // Start scale on specified pins
  scale.wait_ready();                                 //Ensure scale is ready, this is a blocking function
  scale.set_scale();                                  
  Serial.println("Scale Set");
  scale.wait_ready();
  scale.tare();                                       // Tare scale on startup
  scale.wait_ready();
  Serial.println("Scale Zeroed");
}

/********************************** SETUP OTA *****************************************/

void setupOTA() {
  ArduinoOTA.setPort(OTAport);
  ArduinoOTA.setHostname(OTA_HOSTNAME);
  ArduinoOTA.setPassword((const char *)OTA_PASSWORD);

  ArduinoOTA.onStart([]() {
    Serial.println("Starting Node named " + String(OTA_HOSTNAME));
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
}

/********************************** MAIN LOOP *****************************************/

void loop() {
  float reading; // Float for reading
  float raw; // Float for raw value which can be useful
  scale.wait_ready(); // Wait till scale is ready, this is blocking if your hardware is not connected properly.
  scale.set_scale(calibration_factor);  // Sets the calibration factor.

  // Ensure we are still connected to MQTT Topics
  if (!client.connected()) {
    reconnect();
  }

  ArduinoOTA.handle();
  
  Serial.print("Reading: ");            // Prints weight readings in .2 decimal kg units.
  scale.wait_ready();
  reading = scale.get_units(10);        //Read scale in g/Kg
  raw = scale.read_average(5);          //Read raw value from scale too
  Serial.print(reading, 2);
  Serial.println(" kg");
  Serial.print("Raw: ");
  Serial.println(raw);
  Serial.print("Calibration factor: "); // Prints calibration factor.
  Serial.println(calibration_factor);

  if (reading < 0) {
    reading = 0.00;     //Sets reading to 0 if it is a negative value, sometimes loadcells will drift into slightly negative values
  }

  String value_str = String(reading);
  String value_raw_str = String(raw);
  client.publish(STATE_TOPIC, (char *)value_str.c_str());               // Publish weight to the STATE topic
  client.publish(STATE_RAW_TOPIC, (char *)value_raw_str.c_str());       // Publish raw value to the RAW topic

  client.loop();          // MQTT task loop
  scale.power_down();    // Puts the scale to sleep mode for 3 seconds. I had issues getting readings if I did not do this
  delay(3000);
  scale.power_up();
}

void reconnect() {
  while (!client.connected()) {       // Loop until connected to MQTT server
    Serial.print("Attempting MQTT connection...");
    if (client.connect(HOSTNAME, MQTT_USERNAME, MQTT_PASSWORD, AVAILABILITY_TOPIC, 2, true, "offline")) {       //Connect to MQTT server
      Serial.println("connected"); 
      client.publish(AVAILABILITY_TOPIC, "online", true);         // Once connected, publish online to the availability topic
      client.subscribe(TARE_TOPIC);       //Subscribe to tare topic for remote tare
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);  // Will attempt connection again in 5 seconds
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  if (strcmp(topic, TARE_TOPIC) == 0) {
    Serial.println("starting tare...");
    scale.wait_ready();
    scale.set_scale();
    scale.tare();       //Reset scale to zero
    Serial.println("Scale reset to zero");
  }
}
