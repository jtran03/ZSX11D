#include "ZSX11D.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <math.h> // Include math library for sine function

const int chipSelect = 5; // Change this to your SD card CS pin
const int ledPin = 2; // Built-in LED pin for ESP32
const char* logFilename = "/motor_log.txt"; // New log file name
unsigned long lastLogTime = 0;

// Pin Definition
const unsigned int pinPWM = 25; 
const unsigned int pinDirection = 16; 
const unsigned int pinHallSensor = 14; 
const unsigned int pwmChannel = 0; 

const unsigned int rightPinPWM = 26;
const unsigned int rightPinDirection = 13; 
const unsigned int rightHallSensor = 4; 
const unsigned int rightPWMChannel = 1; 

ZSX11D motor(pinPWM, pinDirection, pinHallSensor, pwmChannel, false);
ZSX11D rightMotor(rightPinPWM, rightPinDirection, rightHallSensor, rightPWMChannel, true); 

// Variables for user speed
int targetSpeed = 0;

void setup() {
  motor.begin();
  rightMotor.begin(); 
  // Serial.begin(9600); 
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // Ensure the LED is off initially
  
  if (!SD.begin(chipSelect)) {
    digitalWrite(ledPin, HIGH); // Turn on the LED to indicate failure
    return;
  }

  File logFile = SD.open(logFilename, FILE_WRITE);
  if (logFile) {
    logFile.println("Time (ms), Desired RPM, Measured RPM, P, I, D");
    logFile.close();
  } else {
    digitalWrite(ledPin, HIGH); // Turn on the LED to indicate failure
  }

}

void loop() {
  unsigned long currentTime = millis();

  // // Check if serial data is available
  // if (Serial.available() > 0) {
  //   String command = Serial.readStringUntil('\n'); // Read the serial input until newline
  //   command.trim(); // Remove any leading or trailing whitespace

  //   // Try to parse the command as an integer
  //   targetSpeed = command.toInt();
  //   motor.setRPM(targetSpeed);
  //   rightMotor.setRPM(targetSpeed); 
  // }

  motor.loopPID(); 
  rightMotor.loopPID(); 

  // Update PID and RPM settings at intervals with 5-second pauses
  // if (currentTime > 5000 && currentTime <= 10000) {
  //   rightMotor.setRPM(50); 
  // } else if (currentTime > 15000 && currentTime <= 20000) {
  //   rightMotor.setRPM(100); 
  // } else if (currentTime > 20000 && currentTime <= 25000) {
  //   rightMotor.setRPM(150); 
  // } else if (currentTime > 30000 && currentTime <= 35000) {
  //   rightMotor.setRPM(200); 
  // } else if (currentTime > 35000 && currentTime <= 40000) {
  //   rightMotor.setRPM(250); 
  // } else if (currentTime > 45000 && currentTime <= 50000) {
  //   rightMotor.setRPM(300); 
  // } else if (currentTime > 50000 && currentTime <= 55000) {
  //   rightMotor.setRPM(350); 
  // } else if (currentTime > 60000 && currentTime <= 65000) {
  //   rightMotor.setRPM(300); 
  // } else if (currentTime > 65000 && currentTime <= 70000) {
  //   rightMotor.setRPM(250); 
  // } else if (currentTime > 75000 && currentTime <= 80000) {
  //   rightMotor.setRPM(150); 
  // } else if (currentTime > 80000 && currentTime <= 85000) {
  //   rightMotor.setRPM(100); 
  // } else if (currentTime > 90000 && currentTime <= 95000) {
  //   rightMotor.setRPM(50); 
  // } else if (currentTime > 95000 && currentTime <= 100000) {
  //   rightMotor.setRPM(0); 
  // } else if (currentTime > 105000 && currentTime <= 110000) {
  //   rightMotor.setRPM(-50); 
  // } else if (currentTime > 110000 && currentTime <= 115000) {
  //   rightMotor.setRPM(-100); 
  // } else if (currentTime > 120000 && currentTime <= 125000) {
  //   rightMotor.setRPM(-150); 
  // } else if (currentTime > 125000 && currentTime <= 130000) {
  //   rightMotor.setRPM(-200); 
  // } else if (currentTime > 135000 && currentTime <= 140000) {
  //   rightMotor.setRPM(-250); 
  // } else if (currentTime > 140000 && currentTime <= 145000) {
  //   rightMotor.setRPM(-300); 
  // } else if (currentTime > 150000 && currentTime <= 155000) {
  //   rightMotor.setRPM(-250); 
  // } else if (currentTime > 155000 && currentTime <= 160000) {
  //   rightMotor.setRPM(-200); 
  // } else if (currentTime > 165000 && currentTime <= 170000) {
  //   rightMotor.setRPM(-150); 
  // } else if (currentTime > 170000 && currentTime <= 175000) {
  //   rightMotor.setRPM(-100); 
  // } else if (currentTime > 180000 && currentTime <= 185000) {
  //   rightMotor.setRPM(-50); 
  // } else if (currentTime > 185000) {
  //   rightMotor.setRPM(0); 
  // }

  // if (currentTime > 0 && currentTime <= 5000){
  //   rightMotor.myPID.SetTunings(2.4, 7.5, 0.025); 
  //   rightMotor.setRPM(0); 
  // } else if (currentTime > 5000 && currentTime <= 10000) {
  //   rightMotor.setRPM(120); 
  // } else if (currentTime > 15000 && currentTime <= 20000) {
  //   rightMotor.myPID.SetTunings(2.4, 7.5, 0.05); 
  //   rightMotor.setRPM(0); 
  // } else if (currentTime > 20000 && currentTime <= 25000) {
  //   rightMotor.setRPM(120); 
  // } else if (currentTime > 30000 && currentTime <= 35000) {
  //   rightMotor.myPID.SetTunings(2.4, 7.5, 0.075); 
  //   rightMotor.setRPM(0); 
  // } else if (currentTime > 35000 && currentTime <= 40000) {
  //   rightMotor.setRPM(120); 
  // } else if (currentTime > 45000 && currentTime <= 50000) {
  //   rightMotor.myPID.SetTunings(2.4, 7.5, 0.1); 
  //   rightMotor.setRPM(0); 
  // } else if (currentTime > 50000 && currentTime <= 55000) {
  //   rightMotor.setRPM(120); 
  // } else if (currentTime > 60000 && currentTime <= 65000) {
  //   rightMotor.myPID.SetTunings(2.4, 7.5, 0.125); 
  //   rightMotor.setRPM(0); 
  // } else if (currentTime > 65000 && currentTime <= 70000) {
  //   rightMotor.setRPM(120); 
  // } else if (currentTime > 75000 && currentTime <= 80000) {
  //   rightMotor.myPID.SetTunings(2.4, 7.5, 0.15); 
  //   rightMotor.setRPM(0); 
  // } else if (currentTime > 80000 && currentTime <= 85000) {
  //   rightMotor.setRPM(120); 
  // } else if (currentTime > 90000 && currentTime <= 95000) {
  //   rightMotor.myPID.SetTunings(2.4, 7.5, 0.175); 
  //   rightMotor.setRPM(0); 
  // } else if (currentTime > 95000 && currentTime <= 100000) {
  //   rightMotor.setRPM(120); 
  // } else if (currentTime > 105000 && currentTime <= 110000) {
  //   rightMotor.myPID.SetTunings(2.4, 7.5, 0.2); 
  //   rightMotor.setRPM(0); 
  // } else if (currentTime > 110000 && currentTime <= 115000) {
  //   rightMotor.setRPM(120); 
  // } else if (currentTime > 120000 && currentTime <= 125000) {
  //   rightMotor.myPID.SetTunings(2.4, 7.5, 0.225); 
  //   rightMotor.setRPM(0); 
  // } else if (currentTime > 125000 && currentTime <= 130000) {
  //   rightMotor.setRPM(120); 
  // } else if (currentTime > 135000 && currentTime <= 140000) {
  //   rightMotor.myPID.SetTunings(2.4, 7.5, 0.25); 
  //   rightMotor.setRPM(0); 
  // } else if (currentTime > 140000 && currentTime <= 145000) {
  //   rightMotor.setRPM(120); 
  // } else if (currentTime > 150000 && currentTime <= 155000) {
  //   rightMotor.myPID.SetTunings(2.4, 7.5, 0.275); 
  //   rightMotor.setRPM(0); 
  // } else if (currentTime > 155000 && currentTime <= 160000) {
  //   rightMotor.setRPM(120); 
  // } else if (currentTime > 165000 && currentTime <= 170000) {
  //   rightMotor.myPID.SetTunings(2.4, 7.5, 0.3); 
  //   rightMotor.setRPM(0); 
  // } else if (currentTime > 170000 && currentTime <= 175000) {
  //   rightMotor.setRPM(120); 
  // } else if (currentTime > 180000) {
  //   rightMotor.myPID.SetTunings(2.4, 7.5, 0.325); 
  //   rightMotor.setRPM(0); 
  // }

  if (currentTime - lastLogTime >= 20) { // Log every 100 ms
    lastLogTime = currentTime; // Update the last log time
    
    // Log the data to the SD card
    File logFile = SD.open(logFilename, FILE_APPEND);
    if (logFile) {
      logFile.print(currentTime);
      logFile.print(", ");
      logFile.print(rightMotor.getRPMSetpoint());
      logFile.print(", ");
      logFile.print(rightMotor.getRPM());
      logFile.print(", "); 
      logFile.print(rightMotor.myPID.GetKp()); 
      logFile.print(", "); 
      logFile.print(rightMotor.myPID.GetKi()); 
      logFile.print(", "); 
      logFile.print(String(rightMotor.myPID.GetKd(), 4)); 
      logFile.println(); 
      logFile.close();
      digitalWrite(ledPin, LOW);
    } else {
      digitalWrite(ledPin, HIGH); // Turn on the LED to indicate failure
    }
  }

  // Serial.print(motor.getRPMSetpoint()); 
  // Serial.print(" || "); 
  // Serial.print(motor.getCurrentDutyCycle()); 
  // Serial.print(" || "); 
  // Serial.print(motor.getRPM());
  // Serial.print(" || "); 
  // Serial.print(rightMotor.getRPMSetpoint()); 
  // Serial.print(" || "); 
  // Serial.print(rightMotor.getCurrentDutyCycle()); 
  // Serial.print(" || "); 
  // Serial.println(rightMotor.getRPM());
}
