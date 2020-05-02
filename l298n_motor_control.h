#ifndef L298N_MOTOR_CONTROL_H
#define L298N_MOTOR_CONTROL_H

#include <Arduino.h>

#define L298_PIN_ENA 2
#define L298_PIN_IN1 3
#define L298_PIN_IN2 4
#define L298_PIN_IN3 5
#define L298_PIN_IN4 6
#define L298_PIN_ENB 7

/* speed calibration */
#define MINIMUM_SPEED_A		0
#define MAXIMUM_SPEED_A		255
#define MINIMUM_SPEED_B		0
#define MAXIMUM_SPEED_B		255
#define DEFAULT_SPEED 		100

class l298_motor_control {
public:
  enum motor_control_flags {
      motor_a = 0x1,
      motor_b = 0x2
  };

  l298_motor_control();
  void setup();
  void update_motor_speeds(int16_t speeds[2], uint8_t mask );
private:
  struct motor_descriptor{
      int ena_pin;
      int in1_pin;
      int in2_pin;
      float current_speed;
      float target_speed;
      float acc;
  };
  motor_descriptor motors[2];
};

#endif // L298N_MOTOR_CONTROL_H
