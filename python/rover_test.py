#!/usr/bin/env python
# coding: utf-8

# # Rover Basic Bench Test

ROVER_PORT = '/dev/ttyACM0'
ROVER_BAUDRATE = 115200

from rover import RoverClient
import time

rover = RoverClient(ROVER_PORT,ROVER_BAUDRATE)
rover.connect()


# #### Connectivity test
rover.led_on()
time.sleep(1)
rover.led_off()
time.sleep(1)
rover.request_tmy()
time.sleep(1)

# #### Low level motor control (no trajectory/PID)
MAX_SPEED = 255

# Move forward increasing velocity gradually.
for i in range(0,MAX_SPEED):
    rover.update_motor_speeds( [i, i], RoverClient.MOTOR_A | RoverClient.MOTOR_B )
    time.sleep(0.1)

# Decrease velocity gradually.
for i in range(0,MAX_SPEED):
    rover.update_motor_speeds( [MAX_SPEED-i, MAX_SPEED-i], RoverClient.MOTOR_A | RoverClient.MOTOR_B )
    time.sleep(0.1)

# Move in reverse increasing velocity gradually.
for i in range(0,MAX_SPEED):
    rover.update_motor_speeds( [-i, -i], RoverClient.MOTOR_A | RoverClient.MOTOR_B )
    time.sleep(0.1)

# Decrease velocity gradually.
for i in range(0,MAX_SPEED):
    rover.update_motor_speeds( [-MAX_SPEED+i, -MAX_SPEED+i], RoverClient.MOTOR_A | RoverClient.MOTOR_B )
    time.sleep(0.1)

# #### Tear down

# Disconnect from Rover.
rover.disconnect()

