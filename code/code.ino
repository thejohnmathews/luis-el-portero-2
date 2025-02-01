// code.ino - code used to operate door
// 2/1/2025 - John Mathews

// include libraries
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Servo.h>
#include "environment.h"

// global variables - networking
const char* ssid = SSID;
const char* password = PASSWORD;
const char* serverURL = SERVERURL;
const char* ssl = FINGERPRINT;

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
  Serial.println("\n\nWi-Fi connected. Starting code execution.\n");
}

// code execution
void loop(){
  
  if (WiFi.status() == WL_CONNECTED){

    WiFiClientSecure client;

    // connect via SSL
    if (!client.connect("johnmathews.tech", 443)){

      Serial.println("SSL connection failed");
      return;
    }
    
    // verify SSL cert
    if (!client.setFingerprint(FINGERPRINT)){ 

      Serial.println("SSL certificate verification failed");
      return;
    }

    Serial.println("SSL certificate verified!");
    
    // send GET request
    client.print(String("GET /trigger.php HTTP/1.1\r\n") +
                 "Host: johnmathews.tech\r\n" +
                 "Connection: close\r\n\r\n");

    // wait for response
    long timeout = millis(); 
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }

    // gather server response
    String payload = "";
    while (client.available()) payload += char(client.read());

    Serial.println("Server response: " + payload);

    // does the response contain the trigger?
    if (payload.indexOf("\"trigger\":true") >= 0){

      Serial.println("trigger received, activating servo...");
      SG90.write(trigger_angle);  
      delay(5000);                   
      SG90.write(normal_angle);  
      delay(5000);                   
    }

    // close connection
    client.stop();
  } 
  else{

    // reconnect to WiFi
    Serial.println("Wi-Fi not connected, retrying...");
    WiFi.begin(ssid, password);
    delay(5000);
  }

  // poll every 5 seconds
  delay(5000);  
}