#ifndef ZSX11D_h
#define ZSX11D_h 

#include "Arduino.h"
#include <math.h>
#include <QuickPID.h>

#define PWM_FREQUENCY 5000
#define PWM_RESOLUTION 8
#define MIN_SPEED 24
#define MAX_SPEED 255
#define FORWARD 1
#define STOP 0
#define REVERSE -1
#define PULSE_TIMING_WINDOW_MS 200

#define NORMAL_TIMER_NUMBER 0
#define REVERSED_TIMER_NUMBER 1
#define TIMER_PRESCALAR 80
#define MAX_TIMER_PERIOD_MICROSECONDS 333333 // 60/(RPMmin * PPR)

#define KP 0.6
#define KI 2.034
#define KD 0.044

class ZSX11D {
public:
  ZSX11D(uint8_t pinPWMOut, uint8_t pinBrake, uint8_t pinDirection, uint8_t pinHallSensor, uint8_t pwmChannel, bool isReverse);
  void begin(int PPR, int minRPM); 
  void setPWM(int dutyCycle);
  void setSpeed(int dutyCycle);
  void stop(); 
  void reset(); 
  float getFrequency();
  void loopPID(); 
  float Setpoint, Input, Output; 
  QuickPID myPID; 
private:
  uint8_t _pinPWMOut;
  uint8_t _pinBrake;
  uint8_t _pinDirection;
  uint8_t _pinHallSensor;
  uint8_t _pwmChannel;
  bool _isReverse; 
  int _dutyCycle;
  bool _isMoving;
  bool _canMove;
  int _direction;
  int _maxPeriod; 

  static ZSX11D* instances[2];
  volatile uint64_t _pulse_start, _pulse_end, _TOVF_Counter;
  volatile unsigned long _pulse_period;
  hw_timer_t *_timer;
  bool _measurement_end;
  unsigned long _last_pulse_time; 
  static void IRAM_ATTR onLeftEncoderRise();
  static void IRAM_ATTR onLeftTimerOverflow();
  static void IRAM_ATTR onRightEncoderRise();
  static void IRAM_ATTR onRightTimerOverflow();

};

#endif // ZSX11D_H
