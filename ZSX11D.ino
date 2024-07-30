#include "ZSX11D.h"

// Pin Definition
const unsigned int pinPWM = 25; 
const unsigned int pinBrake = 17; 
const unsigned int pinDirection = 16; 
const unsigned int pinHallSensor = 14; 
const unsigned int pwmChannel = 0; 

ZSX11D motor(pinPWM, pinBrake, pinDirection, pinHallSensor, pwmChannel); 

void setup() {
  Serial.begin(9600);
  motor.begin(); 
}

void loop() {
  static unsigned long lastPrintTime = 0; // To store the last print time
  unsigned long currentTime = millis();

  // Check if serial data is available
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n'); // Read the serial input until newline
    command.trim(); // Remove any leading or trailing whitespace

    // Try to parse the command as an integer
    int newSpeed = command.toInt();

    if (newSpeed >= 24 && newSpeed <= 255) { // Check if the speed is within valid range (0-255)
      motor.setSpeed(newSpeed);
      motor.forward(); // Ensure motor is running forward
      Serial.print("Motor speed set to: ");
      Serial.println(newSpeed);
    } 
    else if (newSpeed <= -24 && newSpeed >= -255){
      motor.setSpeed(abs(newSpeed));
      motor.reverse(); // Ensure motor is running forward
      Serial.print("Motor speed set to: ");
      Serial.println(newSpeed);
    }
    else if (newSpeed == 0){
      motor.setSpeed(newSpeed);
      motor.forward(); // Ensure motor is running forward
      Serial.print("Motor speed set to: ");
      Serial.println(newSpeed);
    }
    else {
      Serial.println("Invalid speed. Please enter a value between 0 and 255.");
    }
  }
  
  if (currentTime - lastPrintTime >= 1000) { // 1000 ms = 1 second
    lastPrintTime = currentTime; // Update the last print time
    Serial.print("Frequency: ");
    Serial.println(motor.getFrequency());
  }
}

