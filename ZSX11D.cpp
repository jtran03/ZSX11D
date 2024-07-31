#include "ZSX11D.h"

ZSX11D* ZSX11D::instances[2] = {nullptr, nullptr};

ZSX11D::ZSX11D(uint8_t pinPWMOut, uint8_t pinBrake, uint8_t pinDirection, uint8_t pinHallSensor, uint8_t pwmChannel, bool isReverse) : myPID(&Input, &Output, &Setpoint){
  _pinPWMOut = pinPWMOut;
  _pinBrake = pinBrake;
  _pinDirection = pinDirection;
  _pinHallSensor = pinHallSensor;
  _pwmChannel = pwmChannel;
  _isReverse = isReverse;

  _isMoving = false;
  _canMove = true;
  _direction = 0;

  _TOVF_Counter = 0; 
  _last_pulse_time = 0;
  _pulse_period = INFINITY; 

  pinMode(_pinDirection, OUTPUT);

  ledcSetup(_pwmChannel, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(_pinPWMOut, _pwmChannel);
}

void ZSX11D::begin(int PPR, int minRPM) {

  if (_isReverse) {
    instances[0] = this;
    attachInterrupt(digitalPinToInterrupt(_pinHallSensor), onLeftEncoderRise, RISING);
    _timer = timerBegin(NORMAL_TIMER_NUMBER, TIMER_PRESCALAR, true); // Use timer 0, prescaler 80, count up
    timerAttachInterrupt(_timer, &onLeftTimerOverflow, true); // Attach the overflow interrupt
    timerAlarmWrite(_timer, MAX_TIMER_PERIOD_MICROSECONDS, true); // Set the max timer period to 1 second
    timerAlarmEnable(_timer); // Enable the timer alarm
    timerStart(_timer);
  } else {
    instances[1] = this;
    attachInterrupt(digitalPinToInterrupt(_pinHallSensor), onRightEncoderRise, RISING);
    _timer = timerBegin(REVERSED_TIMER_NUMBER, TIMER_PRESCALAR, true); // Use timer 0, prescaler 80, count up
    timerAttachInterrupt(_timer, &onRightTimerOverflow, true); // Attach the overflow interrupt
    timerAlarmWrite(_timer, MAX_TIMER_PERIOD_MICROSECONDS, true); // Set the max timer period to 1 second
    timerAlarmEnable(_timer); // Enable the timer alarm
    timerStart(_timer);
  }

  // PID ******************* 
  myPID.SetTunings(KP, KI, KD); 
  myPID.SetMode(myPID.Control::automatic);
  myPID.SetOutputLimits(0, 255);
}

void ZSX11D::setPWM(int dutyCycle) {
  ledcWrite(_pwmChannel, abs(dutyCycle)); 
}

void ZSX11D::setSpeed(int dutyCycle) {
  _dutyCycle = dutyCycle;
  Setpoint = _dutyCycle; 

  if (dutyCycle > 0){
    digitalWrite(_pinDirection, HIGH);
    _direction = FORWARD; // FORWARD
  } else if (dutyCycle < 0){
    digitalWrite(_pinDirection, LOW);
    _direction = REVERSE; // REVERSE
  } else if (dutyCycle == 0){
    myPID.Reset(); 
  }
}

float ZSX11D::getFrequency() {
  return 1.0 / (_pulse_period * 1e-6);
}

void ZSX11D::loopPID() {
  Input = getFrequency(); 
  myPID.Compute(); 
  setPWM(Output); 
}

// Left Timer Overflow Event
void IRAM_ATTR ZSX11D::onLeftTimerOverflow() {
  instances[0]->_TOVF_Counter++;

  if (micros() - instances[0]->_last_pulse_time > MAX_TIMER_PERIOD_MICROSECONDS) {
    instances[0]->_pulse_period = INFINITY; 
  }
}

// Right Timer Overflow Event
void IRAM_ATTR ZSX11D::onRightTimerOverflow() {
  instances[1]->_TOVF_Counter++;

  if (micros() - instances[1]->_last_pulse_time > MAX_TIMER_PERIOD_MICROSECONDS) {
    instances[1]->_pulse_period = INFINITY;
  }
}

// Encoder Rising Event
void IRAM_ATTR ZSX11D::onLeftEncoderRise() {
  // If it is the start of the measurement (1st rising edge)
  if (!instances[0]->_measurement_end) {
    instances[0]->_pulse_start = timerRead(instances[0]->_timer); // Capture the current time
    instances[0]->_measurement_end = true;

  // If it is the end of the measurement (2nd rising edge)
  } else {
    instances[0]->_pulse_end = timerRead(instances[0]->_timer);
    if (instances[0]->_pulse_start < instances[0]->_pulse_end && instances[0]->_TOVF_Counter != 0) { // Edge case where both pulses have overflowed
      instances[0]->_pulse_period = instances[0]->_pulse_end - instances[0]->_pulse_start;
    } else {
      instances[0]->_pulse_period = instances[0]->_pulse_end + (MAX_TIMER_PERIOD_MICROSECONDS * instances[0]->_TOVF_Counter) - instances[0]->_pulse_start; // End has overflowed
    }

    // Reset timer overflow
    instances[0]->_TOVF_Counter = 0;
    // Prepare for the next measurement
    instances[0]->_measurement_end = false;
  }
  // Timeout Variable
  instances[0]->_last_pulse_time = micros(); 
}

// Encoder Rising Event
void IRAM_ATTR ZSX11D::onRightEncoderRise() {
  // If it is the start of the measurement (1st rising edge)
  if (!instances[1]->_measurement_end) {
    instances[1]->_pulse_start = timerRead(instances[1]->_timer); // Capture the current time
    instances[1]->_measurement_end = true;

  // If it is the end of the measurement (2nd rising edge)
  } else {
    instances[1]->_pulse_end = timerRead(instances[1]->_timer);
    if (instances[1]->_pulse_start < instances[1]->_pulse_end && instances[1]->_TOVF_Counter != 0) { // Edge case where both pulses have overflowed
      instances[1]->_pulse_period = instances[1]->_pulse_end - instances[1]->_pulse_start;
    } else {
      instances[1]->_pulse_period = instances[1]->_pulse_end + (MAX_TIMER_PERIOD_MICROSECONDS * instances[1]->_TOVF_Counter) - instances[1]->_pulse_start; // End has overflowed
    }

    // Reset timer overflow
    instances[1]->_TOVF_Counter = 0;

    // Prepare for the next measurement
    instances[1]->_measurement_end = false;
  }
  // Timeout variable
  instances[1]->_last_pulse_time = micros(); 
}
