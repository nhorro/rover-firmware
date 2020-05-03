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
    for i in range(5):      
        print("Led test. Iteration ", i)

        print(" - request telemetry")
        rover.request_tmy()
        time.sleep(1)
        print(" - led on")
        rover.led_on()
        time.sleep(1)
        print(" - led off")
        rover.led_off()
        time.sleep(1)
        print(" - request telemetry")
        rover.request_tmy()
        time.sleep(1)

except KeyboardInterrupt:
    pass

# Disconnect from Rover.
print("Disconnecting from rover")
rover.disconnect()


