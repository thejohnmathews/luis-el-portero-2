// test.ino - test to see if NodeMCU V3 can power the SG90 via USB
// 2/1/2025 

// include libraries
#include <Servo.h>  

// global variables
Servo myServo;          
const int servoPin = D4; 

void setup(){

  // start serial output at 115200 baud
  Serial.begin(115200);         
  myServo.attach(servoPin);     
  Serial.println("start SG90 test");
}

void loop(){

  // move to 0 degrees
  myServo.write(0);      
  Serial.println("SG90 at 0 degrees");
  delay(1000);           

  // move to 90 degrees
  myServo.write(90);
  Serial.println("SG90 at 90 degrees");
  delay(1000);

  // move to 180 degrees
  myServo.write(180);   
  Serial.println("SG90 at 180 degrees");
  delay(1000);
}
