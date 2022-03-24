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
      - ArduinoJSON  
*/


#include "config.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>

int OTAport = 8266;


/**************************** Globals **************************************************/

WiFiClient espClient;
PubSubClient client(espClient);

const int sensorPin = 2; //radar sensor
const int ledPin = 15;
const char* ssid = SSID;
const char* password = PASSWORD;

int calibrationTime = 0;
const int BUFFER_SIZE = 300;

#define MQTT_MAX_PACKET_SIZE 512

//static const uint8_t D0   = 16;
//static const uint8_t D1   = 5;
//static const uint8_t D2   = 4;
//static const uint8_t D3   = 0;
//static const uint8_t D4   = 2;
//static const uint8_t D5   = 14;
//static const uint8_t D6   = 12;
//static const uint8_t D7   = 13;
//static const uint8_t D8   = 15;
//static const uint8_t D9   = 3; 
//static const uint8_t D10  = 1;

// Variables
String motionStatus;
int pirStatus;

/********************************** START SETUP*****************************************/
void setup() {

  Serial.begin(115200);


  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(sensorPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // Connect D0 to RST to wake up
  pinMode(D0, WAKEUP_PULLUP);
  
  Serial.begin(115200);
  delay(10);

  ArduinoOTA.setPort(OTAport);

  ArduinoOTA.setHostname(OTA_HOSTNAME);

  ArduinoOTA.setPassword((const char *)OTA_PASSWORD);

  Serial.print("calibrating sensor ");
  for (int i = 0; i < calibrationTime; i++) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("Starting Node named " + String(OTA_HOSTNAME));

  setup_wifi();

  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);


  ArduinoOTA.onStart([]() {
    Serial.println("Starting");
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
  Serial.println("Ready");
  Serial.print("IPess: ");
  Serial.println(WiFi.localIP());
  reconnect();
}

/********************************** START SETUP WIFI*****************************************/
void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


/********************************** START CALLBACK*****************************************/
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  char message[length + 1];
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';
  Serial.println(message);

}

/********************************** START SEND STATE*****************************************/
void sendState() {
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();

  root["motion"] = (String)motionStatus;

  char buffer[root.measureLength() + 1];
  root.printTo(buffer, sizeof(buffer));

  Serial.println(buffer);
  client.publish(STATE_TOPIC, buffer, true);
}

/********************************** START RECONNECT*****************************************/
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(OTA_HOSTNAME, MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("connected");
      client.subscribe(SET_TOPIC);
      sendState();
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

/********************************** START MAIN LOOP***************************************/
void loop() {

  ArduinoOTA.handle();
  
  if (!client.connected()) {
    // reconnect();
    software_Reset();
  }
  client.loop();

  int sensorValue = digitalRead(sensorPin);

  if (sensorValue > 0 && pirStatus != 2) {
    digitalWrite(ledPin, HIGH);
    digitalWrite(BUILTIN_LED, LOW);//turn the radar LED on
    motionStatus = "motion detected";
      sendState();
      pirStatus = 2;
      
  } else if (sensorValue < 1 && pirStatus != 1) {
    digitalWrite(ledPin, LOW);
    digitalWrite(BUILTIN_LED, HIGH);//turn the radar LED off
    motionStatus = "standby";
    sendState();
    pirStatus = 1;
  }

    delay(100);
}


/****reset***/
void software_Reset() // Restarts program from beginning but does not reset the peripherals and registers
{
  Serial.print("resetting");
  ESP.reset(); 
}

