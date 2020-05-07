#ifndef ROVER_APPLICATION_H
#define ROVER_APPLICATION_H

#include <Arduino.h>

#include <stdint.h>
#include "protocol.h"
#include "tctm/cmd_def.h" 		// Opcodes definition
#include "tctm/tmy_def.h"		// Telemetry definition
#include "tctm/report_def.h"	// Report definition

#include "l298n_motor_control.h"
#include "imu_ahrs.h"

#define SerialIF 				Serial1 // Use Serial1 to avoid problems with Serial0 and USB.
#define APP_SERIAL_IF_BAUDRATE	115200

#define IMU_UPDATE_FREQ 40

class application: 
	public protocol::packet_decoder,
	public protocol::packet_encoder	
{
public:
	application();
	void setup();
	void loop();
private:
	uint8_t tmy[TMY_PARAM_LAST] __attribute__((aligned (4)));
	using opcode_callback = application::error_code(application::*)(const uint8_t* payload, uint8_t n);
	opcode_callback opcode_callbacks[OPCODE_LAST];

	/* required by packet_decoder */
	void handle_packet(const uint8_t* payload, uint8_t n) override;
	void set_error(error_code ec) override;
	void send_impl(const uint8_t* buf, uint8_t n) override;
	void handle_connection_lost() override;

	/* Commands :: System commands */
	application::error_code request_tmy(const uint8_t* payload, uint8_t n);
	application::error_code led_on(const uint8_t* payload, uint8_t n);
	application::error_code led_off(const uint8_t* payload, uint8_t n);

	/* Commands :: Application Specific */

	// BEGIN Application Specific Commands here
	application::error_code update_motor_speeds(const uint8_t* payload, uint8_t n);

	void send_imu_report();
	// END Application Specific Commands here

	// BEGIN Application Specific Data here
	l298_motor_control motor_ctl;
	int16_t speeds[2] = { 0, 0 };

	imu_ahrs imu;

	uint32_t control_cycle_t0;
	// END Application Specific Data here
};

#endif // ROVER_APPLICATION_H
