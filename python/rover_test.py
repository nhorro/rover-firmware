#!/usr/bin/env python
# coding: utf-8

# # Rover Basic Bench Test

ROVER_PORT = '/dev/serial0'
ROVER_BAUDRATE = 115200

from rover import RoverClient
import time

try:
	print("Connecting to Rover")
	rover = RoverClient(ROVER_PORT,ROVER_BAUDRATE)
	rover.connect()

	# #### Connectivity test
	print("Led test")
	rover.led_on()
	time.sleep(1)
	rover.led_off()
	time.sleep(1)
	rover.request_tmy()
	time.sleep(1)

	# #### Low level motor control (no trajectory/PID)
	print("Basic speed control test")
	MAX_SPEED = 255

	# Move forward increasing velocity gradually.
	print("Move forward increasing velocity gradually.")
	for i in range(0,MAX_SPEED):
	    rover.update_motor_speeds( [i, i], RoverClient.MOTOR_A | RoverClient.MOTOR_B )
	    time.sleep(0.1)

	# Decrease velocity gradually.
	print("Decrease velocity gradually.")
	for i in range(0,MAX_SPEED):
	    rover.update_motor_speeds( [MAX_SPEED-i, MAX_SPEED-i], RoverClient.MOTOR_A | RoverClient.MOTOR_B )
	    time.sleep(0.1)

	# Move in reverse increasing velocity gradually.
	print("Move in reverse increasing velocity gradually.")
	for i in range(0,MAX_SPEED):
	    rover.update_motor_speeds( [-i, -i], RoverClient.MOTOR_A | RoverClient.MOTOR_B )
	    time.sleep(0.1)

	# Decrease velocity gradually.
	print("Decrease velocity gradually.")
	for i in range(0,MAX_SPEED):
	    rover.update_motor_speeds( [-MAX_SPEED+i, -MAX_SPEED+i], RoverClient.MOTOR_A | RoverClient.MOTOR_B )
	    time.sleep(0.1)
except KeyboardInterrupt:
	pass

# #### Tear down

# Disconnect from Rover.
print("Disconnecting from rover")
rover.disconnect()


