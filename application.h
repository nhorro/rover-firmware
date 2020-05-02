#ifndef ROVER_APPLICATION_H
#define ROVER_APPLICATION_H

#include <Arduino.h>

#include <stdint.h>
#include "protocol.h"
#include "tctm/cmd_def.h"
#include "tctm/tmy_def.h"

#include "l298n_motor_control.h"

class application: 
	public protocol::packet_decoder,
	public protocol::packet_encoder	
{
public:
	application();
	void setup();
	void loop();
private:
	uint8_t tmy[TMY_PARAM_LAST];
	using opcode_callback = application::error_code(application::*)(const uint8_t* payload, uint8_t n);
	opcode_callback opcode_callbacks[OPCODE_LAST];

	/* Application objects */
	l298_motor_control motor_ctl;
	int16_t speeds[2] = { 0, 0 };

	/* required by packet_decoder */
	void handle_packet(const uint8_t* payload, uint8_t n) override;
	void set_error(error_code ec) override;
	void send_impl(const uint8_t* buf, uint8_t n) override;

	/* Commands :: System commands */
	application::error_code request_tmy(const uint8_t* payload, uint8_t n);
	application::error_code led_on(const uint8_t* payload, uint8_t n);
	application::error_code led_off(const uint8_t* payload, uint8_t n);
	application::error_code update_motor_speeds(const uint8_t* payload, uint8_t n);
};

#endif // ROVER_APPLICATION_H
