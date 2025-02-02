// code.ino - code used to operate door
// 2/1/2025 - John Mathews

// include libraries
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Servo.h>
#include "environment.h"

// global variables - SG90
Servo SG90;
const int SG90_signal = D4;
const int normal_angle = 0;
const int trigger_angle = 180;

// global variables
WiFiClientSecure client;

void connect_wifi(){

  // attempt WiFi connection
  Serial.println("connecting to Wi-Fi...");
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED){

    delay(1000);
    Serial.println(".");
  }

  // successful connection to WiFi
  Serial.println("\n\nWi-Fi connected. Starting code execution.\n");
}

void connect_server(){

  // check SHA1 fingerprint of certificate
  client.setFingerprint(FINGERPRINT);

  // connect using SSL
  Serial.println("connecting to server...");
  if (!client.connect("johnmathews.tech", 443)){

    Serial.println("SSL connection failed");
    return;
  }

  Serial.println("SSL connection success");
}

// code initialization
void setup(){
  
  // initialize Serial and SG90 signal pin & angle
  Serial.begin(115200);
  SG90.attach(SG90_signal);
  SG90.write(normal_angle);
  Serial.println("NodeMCU v3 powered on, initializing ...");

  // connect to WiFi
  connect_wifi();

  // connect to server
  connect_server();
}

// code execution
void loop(){

  Serial.println("entering loop() execution");

  // while connected to WiFi & server, get GET requests
  while(WiFi.status() == WL_CONNECTED && client.connected()){

    Serial.println("Sending GET request using SSL...");

    // send GET request to look at trigger's status
    client.print(String("GET /pages/trigger_status.txt HTTP/1.1\r\n") +
                  "Host: johnmathews.tech\r\n" +
                  "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)\r\n" +
                  "Referer: https://johnmathews.tech\r\n" +
                  "Connection: keep-alive\r\n\r\n");

    // wait for response
    long timeout = millis();
    while (client.available() == 0){

        if (millis() - timeout > 5000){ 

          Serial.println(">>> server 5sec response timeout");
          return;
        }
    }

    // read response
    String payload = "";
    while (client.available()) payload += char(client.read());

    Serial.println("Server response: " + payload);

    // check for code 500 response
    if (payload.indexOf("500 Internal Server Error") >= 0){
      
      Serial.println("Received 500 Internal Server Error, switching to backup server...");
      client.stop();
      break;  
    }

    // check if trigger is active
    if (payload.indexOf("true") >= 0) {
        Serial.println("Trigger received, activating servo...");
        SG90.write(trigger_angle);
        delay(5000);
        SG90.write(normal_angle);
    }

    // wait before polling again
    delay(5000);
  } 

  // regain wifi and/or server connection if lost
  if (WiFi.status() != WL_CONNECTED){

    Serial.println("WiFi connection lost, reconnecting...");
    connect_wifi();
  }
  if (!client.connected()){

    Serial.println("server connection lost, reconnecting...");
    connect_server();
  }
}