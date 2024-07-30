#ifndef ZSX11D_h
#define ZSX11D_h 

#include "Arduino.h"
#include "FreqCountESP.h"

#define PWM_FREQUENCY 5000
#define PWM_RESOLUTION 8
#define MIN_SPEED 24
#define MAX_SPEED 255
#define FORWARD 1
#define STOP 0
#define REVERSE -1
#define PULSE_TIMING_WINDOW_MS 200

class ZSX11D {
public:
  ZSX11D(uint8_t pinPWMOut, uint8_t pinBrake, uint8_t pinDirection, uint8_t pinHallSensor, uint8_t pwmChannel);
  void begin(); 
  void setSpeed(unsigned short dutyCycle);
  void forward();
  void reverse();
  void stop(); 
  void reset(); 
  unsigned int getFrequency();
  
private:
  uint8_t _pinPWMOut;
  uint8_t _pinBrake;
  uint8_t _pinDirection;
  uint8_t _pinHallSensor;
  uint8_t _pwmChannel;
  unsigned short _dutyCycle;
  bool _isMoving;
  bool _canMove;
  int _direction;
  int _frequencyHz; 
};

#endif // ZSX11D_H
