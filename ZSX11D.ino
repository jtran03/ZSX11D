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
const unsigned int pinBrake = 17; 
const unsigned int pinDirection = 16; 
const unsigned int pinHallSensor = 14; 
const unsigned int pwmChannel = 0; 

ZSX11D motor(pinPWM, pinBrake, pinDirection, pinHallSensor, pwmChannel, false);

// Variables for user speed
int targetSpeed = 0;

// 450 Counts
unsigned long startTime = 0;
bool tracking = false; 
unsigned long startPulseCount = 0; // Initial pulse count when tracking starts


void setup() {
  motor.begin();
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

}

void loop() {
  unsigned long currentTime = millis();

    // Check if serial data is available
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n'); // Read the serial input until newline
    command.trim(); // Remove any leading or trailing whitespace

    // Try to parse the command as an integer
    targetSpeed = command.toInt();
    motor.setRPM(targetSpeed);
  }

  motor.loopPID(); 

  Serial.print(motor.getRPMSetpoint()); 
  Serial.print(" || "); 
  Serial.print(motor.getCurrentDutyCycle()); 
  Serial.print(" || "); 
  Serial.println(motor.getRPM());

  if (currentTime - lastLogTime >= 20) { // Log every 100 ms
    lastLogTime = currentTime; // Update the last log time
    
    // Log the data to the SD card
    File logFile = SD.open(logFilename, FILE_APPEND);
    if (logFile) {
      logFile.print(currentTime);
      logFile.print(", ");
      logFile.print(motor.getRPMSetpoint());
      logFile.print(", ");
      logFile.println(motor.getRPM());
      logFile.close();
      digitalWrite(ledPin, LOW);
    } else {
      digitalWrite(ledPin, HIGH); // Turn on the LED to indicate failure
    }
  }
}
