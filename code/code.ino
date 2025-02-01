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
  
  // initialize Serial and SG90 signal pin & angle
  Serial.begin(115200);
  SG90.attach(SG90_signal);
  SG90.write(normal_angle);
  Serial.println("NodeMCU v3 powered on, initializing ...");

  // attempt WiFi connection
  Serial.println("connecting to Wi-Fi...");
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED){

    delay(1000);
    Serial.println(".");
  }

  // successful connection to WiFi
  Serial.println("\n\nWi-Fi connected." + String(WiFi.localIP()) + "\n");
}

// code execution
void loop() {
  
  // connect to server
  if (WiFi.status() == WL_CONNECTED){

    // send a GET request to server
    HTTPClient http;
    http.begin(SERVERURL);
    int http_response_code = http.GET();

    // server responds with "trigger": true
    if (http_response_code == 200){

      String payload = http.getString();

      if (payload.indexOf("\"trigger\":true") >= 0){

        Serial.println("trigger received from server. activating SG90...");

        // activate SG90 for button press
        SG90.write(trigger_angle);
        delay(5000);
        SG90.write(normal_angle);
        delay(5000);
      }

      // close connection
      http.end()
    }
    // server responded with an error "trigger": false
    else{

      Serial.println("HTTP request failed. code: " + String(http_response_code));
    }
  }
  else{

    // regain connection to WiFi
    Serial.println("WiFi not connected, retrying...");
    WiFi.begin(SSID, PASSWORD);
    delay(5000);
  }

  // poll server every 5 seconds
  delay(5000);
}
