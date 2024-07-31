#include "ZSX11D.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <math.h> // Include math library for sine function

const int chipSelect = 5; // Change this to your SD card CS pin
const int ledPin = 2; // Built-in LED pin for ESP32
const char* logFilename = "/motor_log.txt"; // New log file name

// Pin Definition
const unsigned int pinPWM = 25; 
const unsigned int pinBrake = 17; 
const unsigned int pinDirection = 16; 
const unsigned int pinHallSensor = 14; 
const unsigned int pwmChannel = 0; 

ZSX11D motor(pinPWM, pinBrake, pinDirection, pinHallSensor, pwmChannel, false);

// Ramp parameters
const unsigned long softStartDuration = 1500; // Duration of the soft start in milliseconds
const unsigned long maintainSpeedDuration = 3000; // Duration to maintain speed before stopping
const unsigned long softStopDuration = 1500; // Duration of the soft stop in milliseconds

// Variables for ramping
int targetSpeed = 0;
unsigned long startTime = 0;
unsigned long lastLogTime = 0;

void setup() {
  motor.begin(90, 1);
  Serial.begin(9600); 
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // Ensure the LED is off initially
  
  if (!SD.begin(chipSelect)) {
    digitalWrite(ledPin, HIGH); // Turn on the LED to indicate failure
    return;
  }

  File logFile = SD.open(logFilename, FILE_WRITE);
  if (logFile) {
    logFile.println("Time (ms), Motor Speed, Frequency");
    logFile.close();
  } else {
    digitalWrite(ledPin, HIGH); // Turn on the LED to indicate failure
  }

  startTime = millis(); // Initialize the start time
}

void loop() {
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - startTime;

    // Check if serial data is available
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n'); // Read the serial input until newline
    command.trim(); // Remove any leading or trailing whitespace

    // Try to parse the command as an integer
    targetSpeed = command.toInt();
    motor.setSpeed(targetSpeed);
  }

  motor.loopPID(); 

  Serial.print(motor.Setpoint); 
  Serial.print(" || "); 
  Serial.print(motor.Input); 
  Serial.print(" || "); 
  Serial.print(motor.Output); 
  Serial.print(" || "); 
  Serial.println(motor.getFrequency());

  if (currentTime - lastLogTime >= 20) { // Log every 100 ms
    lastLogTime = currentTime; // Update the last log time
    
    // Log the data to the SD card
    File logFile = SD.open(logFilename, FILE_APPEND);
    if (logFile) {
      logFile.print(currentTime);
      logFile.print(", ");
      logFile.print(targetSpeed);
      logFile.print(", ");
      logFile.println(motor.getFrequency());
      logFile.close();
      digitalWrite(ledPin, LOW);
    } else {
      digitalWrite(ledPin, HIGH); // Turn on the LED to indicate failure
    }
  }
}
