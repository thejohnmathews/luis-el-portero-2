// code.ino - code used to operate door
// 2/1/2025 - John Mathews

// include libraries
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Servo.h>
#include "environment.h"

// global variables - networking
const char* ssid = SSID;
const char* password = PASSWORD;
const char* serverURL = SERVERURL;

// global variables - SG90
Servo SG90;
const int SG90_signal = D4;
const int normal_angle = 0;
const int trigger_angle = 180;

// code initialization
void setup(){

  // power on
  Serial.println("NodeMCU v3 powered on, initializing ...");
  
  // initialize Serial and SG90 signal pin & angle
  Serial.begin(115200);
  SG90.attach(SG90_signal);
  SG90.write(normal_angle);

  // attempt WiFi connection
  Serial.println("connecting to Wi-Fi...");
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED){

    delay(1000);
    Serial.println(".");
  }

  // successful connection to WiFi
  Serial.println("\n\nWi-Fi connected.\n");

}

// code execution
void loop() {
  

}
