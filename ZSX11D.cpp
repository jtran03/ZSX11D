#include "ZSX11D.h"

ZSX11D::ZSX11D(uint8_t pinPWMOut, uint8_t pinBrake, uint8_t pinDirection, uint8_t pinHallSensor, uint8_t pwmChannel) {
  _pinPWMOut = pinPWMOut; 
  _pinBrake = pinBrake; 
  _pinDirection = pinDirection; 
  _pinHallSensor = pinHallSensor; 
  _pwmChannel = pwmChannel; 

  _isMoving = false;
  _canMove = true; 
  _direction = STOP; 
  _frequencyHz = 0; 

  pinMode(_pinDirection, OUTPUT); 

  ledcSetup(_pwmChannel, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(_pinPWMOut, _pwmChannel);
}

void ZSX11D::begin(){
  setSpeed(0);
  forward();
  delay(1000); 
  FreqCountESP.begin(_pinHallSensor, PULSE_TIMING_WINDOW_MS);
}

void ZSX11D::setSpeed(unsigned short dutyCycle) {
  _dutyCycle = dutyCycle; 
}

void ZSX11D::forward() {
  digitalWrite(_pinDirection, HIGH); 
  ledcWrite(_pwmChannel, _dutyCycle);
  _direction = FORWARD; 
  _isMoving = true; 
}

void ZSX11D::reverse() {
  digitalWrite(_pinDirection, LOW); 
  ledcWrite(_pwmChannel, _dutyCycle);
  _direction = REVERSE; 
  _isMoving = true; 
}

unsigned int ZSX11D::getFrequency() {
  if (FreqCountESP.available())
  {
    _frequencyHz = FreqCountESP.read();
  }

  return _frequencyHz; 
}
