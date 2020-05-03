#include "application.h"

application::application() :
		opcode_callbacks
		{ 
			// Basic
			&application::request_tmy,
			&application::led_on,
			&application::led_off,

			// BEGIN Application Specific Commands here

			// Rover control
			&application::update_motor_speeds

			// END Application Specific Commands here
		}
{

}

void application::setup()
{
	SerialIF.begin(APP_SERIAL_IF_BAUDRATE);
	pinMode(LED_BUILTIN, OUTPUT);

	// BEGIN Application Setup Code here
	this->motor_ctl.setup();
	// END Application Setup  Code here
}

void application::loop()
{	
	// Read commands
	if(int n= SerialIF.available())
	{
		do {
			this->feed(SerialIF.read());
		} while(--n);
	}

	// BEGIN Application Code here
	// END Application Code here
	this->check_timeout();
}

void application::handle_packet(const uint8_t* payload, uint8_t n)
{
	uint8_t opcode = payload[0];
	if (OPCODE_REQUEST_TMY == opcode)
	{
		this->request_tmy(payload, n);
	}
	else
	{
		this->tmy[TMY_PARAM_ACCEPTED_PACKETS]++;
		this->tmy[TMY_PARAM_LAST_ERROR] =
				(opcode < OPCODE_LAST) ?
						(this->*(opcode_callbacks[opcode]))(payload + 1,
								n - 1) :
						error_code::unknown_opcode;
		this->tmy[TMY_PARAM_LAST_OPCODE] = static_cast<uint8_t>(opcode);

		// Generate execution report
		this->get_payload_buffer()[0] = REPORT_COMMAND_EXECUTION_STATUS;
		this->get_payload_buffer()[1] = this->tmy[TMY_PARAM_LAST_OPCODE];
		this->get_payload_buffer()[2] = this->tmy[TMY_PARAM_LAST_ERROR];
		this->send(3);		
	}
}

void application::set_error(error_code ec)
{
	if (packet_decoder::error_code::timeout != ec)
	{
		this->tmy[TMY_PARAM_REJECTED_PACKETS]++;
		this->tmy[TMY_PARAM_LAST_ERROR] = static_cast<uint8_t>(ec);
	}
}

void application::send_impl(const uint8_t* buf, uint8_t n)
{
	SerialIF.write(buf, n);
}

/* Opcodes */

application::error_code application::request_tmy(const uint8_t* payload, uint8_t n)
{
	// TODO Report type
	//uint8_t report_type = payload[0];

	// BEGIN Application TMY Handling here			
	this->get_payload_buffer()[0] = REPORT_TELEMETRY_REQUEST;

	// BEGIN Application TMY Handling here	
	this->get_payload_buffer()[1] = this->tmy[TMY_PARAM_ACCEPTED_PACKETS];
	this->get_payload_buffer()[2] = this->tmy[TMY_PARAM_REJECTED_PACKETS];
	this->get_payload_buffer()[3] = this->tmy[TMY_PARAM_LAST_OPCODE];
	this->get_payload_buffer()[4] = this->tmy[TMY_PARAM_LAST_ERROR];
	this->send(1+4);
	// END Application TMY Handling here	
	
	return error_code::success;
}

application::error_code application::led_on(const uint8_t* payload, uint8_t n)
{
	digitalWrite(LED_BUILTIN, HIGH);
	return error_code::success;
}

application::error_code application::led_off(const uint8_t* payload, uint8_t n)
{
	digitalWrite(LED_BUILTIN, LOW);
	return error_code::success;
}

// BEGIN Application Specific Commands Handling here

application::error_code application::update_motor_speeds(const uint8_t* payload, uint8_t n)
{
	this->speeds[0] = (payload[0] << 8) | (payload[1] & 0xFF);
	this->speeds[1] = (payload[2] << 8) | (payload[3] & 0xFF);
	uint8_t flags = payload[4];
	this->motor_ctl.update_motor_speeds(this->speeds, flags);
	return error_code::success;
}