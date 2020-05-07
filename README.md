# 4WD rover arduino firmware

Arduino Due firmware for [4WD rover kit](https://www.banggood.com/DIY-4WD-Smart-Robot-Car-Chassis-Kits-with-Speed-Encoder-p-1311282.html?rmmds=detail-left-hotproducts__6&cur_warehouse=CN) with a Python client for integration with Raspberry Pi, ROS, etc.

Current implementation status:

| Feature                                                    | Status             |
| ---------------------------------------------------------- | ------------------ |
| Telecommand and Telemetry serial interface.                | :heavy_check_mark: |
| Basic motor control with L298N.                            | :heavy_check_mark: |
| Motor control with velocity profiles.                      |                    |
| Navigation & State Estimation - AHRS with MPU9250.         |                    |
| Navigation & State Estimation - GPS.                       |                    |
| Navigation & State Estimation - Odometry (wheel encoders). |                    |
| Navigation & State Estimation - Data fusion.               |                    |

## Dependencies

- [arduino-cli](https://github.com/arduino/arduino-cli)
- [Bolder Flight Dynamics MPU9250](https://github.com/bolderflight/MPU9250)

## Rover hardware setup

### Part list

Platform:

- 1x Arduino Due.
- 1x L298N.
- 4WD Chassis kit:
  - 4x DC Motors wtih gear reduction + wheels.
- 1x Battery holder x2 18650 (8.4v).
- 2x 18650 batteries.

Payload:

- Raspberry Pi 3B+.
- Webcam C170.

### Wiring

#### Arduino - L298N

| L298N Pin | Arduino Due Pin | Wire Color |
| --------- | --------------- | ---------- |
| ENA       | 2               | Black      |
| IN1       | 3               | Yellow     |
| IN2       | 4               | Brown      |
| IN3       | 5               | Red        |
| IN4       | 6               | Black      |
| ENB       | 7               | Green      |

#### L298N - Motors

| L298N Pin | Motor      | Wire Color |
| --------- | ---------- | ---------- |
| OUT1      | LF, LB (-) | Black      |
| OUT2      | LF, LB (+) | Red        |
| OUT3      | RF,RB(+)   | Red        |
| OUT4      | RF,RB(-)   | Black      |

With Car facing forwards: LF=Left/Front, RB=Right/Back,etc.

#### Serial Interface with Raspberry Pi 3B+

| Arduino Due Pin | Raspberry Pi 3B+ Pin | Wire Color |
| --------------- | -------------------- | ---------- |
| GND             | 6 (GND)              | GND        |
| 19 (RX1)        | 8 (TXD)              | White      |
| 18 (TX1)        | 10 (RXD)             | Yellow     |

#### Arduino - MPU9250

| Arduino Due Pin | MPU9250 Pin | Wire Color |
| --------------- | ----------- | ---------- |
| VCC             | 3.3V        | Red        |
| GND             | GND         | Black      |
| SDA (20)        | SDA         | Greem      |
| SCL (21)        | SCL         | Yellow     |

#### Arduino - GPS

| Arduino Due Pin | GPS Pin | Wire Color |
| --------------- | ------- | ---------- |
| GND            |         |            |
| RX2            |         |            |
| TX2            |         |            |

## SW interface

The SW protocol uses the following packet format:

```
"PKT!<payload><crc16>\n"
```

where:

- "PKT!" is the sync word.
- Payload is a sequence consisting of up to 64 bytes.
- crc16 is the crc16 of the payload.
- \n is the packet terminator.

Two types of packets are used:

- **Commands**: the payload consists of an OPCODE plus command-specific data.
- **Reports**: the payload consists of a REPORT_ID and report specific data. Two special types of reports are mandatory:
  - Telemetry.
  - Command execution status.

### Commands

| Opcode | Mnemonic            | Description                                                 | Parameters         |
| ------ | ------------------- | ----------------------------------------------------------- | ------------------ |
| 0x00   | REQUEST_TMY         | Request telemetry. A general telemetry report is generated. | None.              |
| 0x01   | LED_ON              | Turn on test led.                                           | None.              |
| 0x02   | LED_OFF             | Turn off test led.                                          | None.              |
| 0x03   | UPDATE_MOTOR_SPEEDS | Update motor speeds.                                        | See details below. |

#### 0x03 UPDATE_MOTOR_SPEEDS

Update motor speed values. Each motor speed is specified as an int16 from -255 to 255.

| Byte Offset | Description                                      |
| ----------- | ------------------------------------------------ |
| 0           | Motor A Speed MSB.                               |
| 1           | Motor A Speed LSB.                               |
| 2           | Motor B Speed MSB.                               |
| 3           | Motor B Speed LSB.                               |
| 4           | Flags. b01: Enable Motor A, b10: Enable Motor B. |

### Reports

#### 0x80 GENERAL_TELEMETRY_REPORT

This report is generated in response to a REQUEST_TMY command.

| Offset | Parameter                      | Description                                                  |
| ------ | ------------------------------ | ------------------------------------------------------------ |
| 0      | REPORT_TELEMETRY_REQUEST(0x80) | Report Id.                                                   |
| 1      | TMY_PARAM_ACCEPTED_PACKETS     | Accepted packets (telemetry request do not increase this counter). |
| 2      | TMY_PARAM_REJECTED_PACKETS     | Rejected packets (telemetry request do not increase this counter). |
| 3      | TMY_PARAM_LAST_OPCODE          | Last opcode received.                                        |
| 4      | TMY_PARAM_LAST_ERROR           | Error code from last opcode executed.                        |

#### 0x81 REPORT_COMMAND_EXECUTION_STATUS

This report is generated after a command is executed.

| Offset | Parameter                             | Description                   |
| ------ | ------------------------------------- | ----------------------------- |
| 0      | REPORT_COMMAND_EXECUTION_STATUS(0x81) | Report Id.                    |
| 1      | OPCODE                                | Executed opcode.              |
| 2      | STATUS_CODE                           | Opcode execution status code. |



## Firmware build instructions

Tested on Arduino Due.
List available boards.

~~~bash
arduino-cli board list
Port         Type              Board Name                     FQBN                          Core       
/dev/ttyACM0 Serial Port (USB) Arduino Due (Programming Port) arduino:sam:arduino_due_x_dbg arduino:sam
~~~

Build and upload (from one directory upwards):

~~~bash
arduino-cli compile --fqbn arduino:sam:arduino_due_x rover-firmware
arduino-cli upload -p /dev/ttyACM0 --fqbn arduino:sam:arduino_due_x rover-firmware
~~~

## Firmware tests

Jupyter: see [RoverBasicBenchTest.ipynb](python/RoverBasicBenchTest.ipynb)

```bash
cd python
jupyter notebook
```

or:

```bash
cd python
python rover_test.py # or python silent_rover_test.py
```

