#include "application.h"

application::application() :
	  control_cycle_t0(millis())
	, opcodes
		{ 
			// Basic
			{ &application::request_tmy, opcode_flags::default_flags },
			{ &application::led_on, opcode_flags::enable_execution_status_report },
			{ &application::led_off, opcode_flags::enable_execution_status_report },

			// BEGIN Application Specific Commands here

			// Rover control
			{ &application::update_motor_speeds, opcode_flags::default_flags }

			// END Application Specific Commands here
		}
	, periodic_tasks 
		{
			{ true, millis(), (1000/IMU_UPDATE_FREQ), &application::update_imu }, 
			{ true, millis(), (1000/IMU_REPORT_FREQ), &application::send_imu_report }, 
			{ true, millis(), (1000/GENERAL_TMY_REPORT_FREQ), &application::send_general_tmy_report }
		}
{
	
}

void application::setup()
{
	Wire.begin();
	SerialIF.begin(APP_SERIAL_IF_BAUDRATE);
	pinMode(LED_BUILTIN, OUTPUT);

	// BEGIN Application Setup Code here
	this->motor_ctl.setup();

	//
	if ( this->imu.setup() >= 0 )
	{
		this->imu.set_mag_bias_correction( -4.798046398046397, 231.2808302808303, 30.96239316239316); // see notebook		
	}
	else
	{
		this->tmy[TMY_PARAM_STATUS] |= STATUS_AHRS_FAIL;
	}

	if ( false )
	{
		this->tmy[TMY_PARAM_STATUS] |= STATUS_GPS_FAIL;
	}
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

	uint32_t t1 = millis();
	// Calculate dt from beginning of control cycle
	uint32_t dt = this->control_cycle_t0 > t1 ? 
		1 + this->control_cycle_t0 + ~t1 : t1 - this->control_cycle_t0;
	if(dt>=1000)
	{
		this->control_cycle_t0 = millis();
		this->align_periodic_tasks_with_control_cycle();

		// Blink led until first command is received. 
		// Used as an indicator that SW is running.
		//if(this->tmy[TMY_PARAM_ACCEPTED_PACKETS]==0)
		{
			digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));	
		}		
  	}

	// BEGIN Application Code here
	// END Application Code here

	this->execute_periodic_tasks(t1);
	this->check_timeouts();
}

void application::handle_packet(const uint8_t* payload, uint8_t n)
{
	uint8_t opcode = payload[0];
	if (OPCODE_REQUEST_TMY == opcode)
	{
		/* General telemetry request is a special case. 
	   	   We dont want counters and status updated by this opcode. */
		(this->*(opcodes[opcode].fn))(payload + 1, n - 1);
	}
	else
	{
		this->tmy[TMY_PARAM_ACCEPTED_PACKETS]++;
		this->tmy[TMY_PARAM_LAST_ERROR] =
				(opcode < OPCODE_LAST) ?
						(this->*(opcodes[opcode].fn))(payload + 1,
								n - 1) :
						error_code::unknown_opcode;
		this->tmy[TMY_PARAM_LAST_OPCODE] = static_cast<uint8_t>(opcode);

		// Generate execution report
		if ( this->opcodes->flags & opcode_flags::enable_execution_status_report )
		{
			this->get_payload_buffer()[0] = REPORT_COMMAND_EXECUTION_STATUS;
			this->get_payload_buffer()[1] = this->tmy[TMY_PARAM_LAST_OPCODE];
			this->get_payload_buffer()[2] = this->tmy[TMY_PARAM_LAST_ERROR];
			this->send(3);			
		}
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

void application::handle_connection_lost()
{
	//REQUEST_EXTERNAL_RESET

	//shutdown motors
	this->speeds[0] = 0;
	this->speeds[1] = 0;
	this->motor_ctl.update_motor_speeds(this->speeds, 
		l298_motor_control::motor_control_flags::motor_a|
		l298_motor_control::motor_control_flags::motor_b );
}

void application::align_periodic_tasks_with_control_cycle()
{
	for(int i;i<N_PERIODIC_TASKS;i++)
	{
		this->periodic_tasks[i].t0 = this->control_cycle_t0;
	}	
}

void application::execute_periodic_tasks(uint32_t t)
{
	for(int i;i<N_PERIODIC_TASKS;i++)
	{
		if( this->periodic_tasks[i].enabled)
		{
			uint32_t dt = this->periodic_tasks[i].t0  > t ? 
				1 + this->periodic_tasks[i].t0  + ~t : t - this->periodic_tasks[i].t0;
			if(dt>=this->periodic_tasks[i].period)
			{	
				this->periodic_tasks[i].t0 = t;			
				(this->*(this->periodic_tasks[i].entrypoint))();
			}	
		}
	}
}

/* Opcodes */

application::error_code application::request_tmy(const uint8_t* payload, uint8_t n)
{
	this->send_general_tmy_report();
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

/* Reṕorts */

void application::send_general_tmy_report()
{
	this->get_payload_buffer()[0] = REPORT_GENERAL_TELEMETRY;
	this->get_payload_buffer()[1] = this->tmy[TMY_PARAM_ACCEPTED_PACKETS];
	this->get_payload_buffer()[2] = this->tmy[TMY_PARAM_REJECTED_PACKETS];
	this->get_payload_buffer()[3] = this->tmy[TMY_PARAM_LAST_OPCODE];
	this->get_payload_buffer()[4] = this->tmy[TMY_PARAM_LAST_ERROR];
	this->get_payload_buffer()[5] = this->tmy[TMY_PARAM_STATUS];
	this->send(1+5);
}

void application::send_imu_report()
{
	// BEGIN Application TMY Handling here			
	this->get_payload_buffer()[0] = REPORT_IMU_AHRS_STATE;
	this->get_payload_buffer()[1] = (this->imu.get_status()) >= 0 ? true : false;
	this->get_payload_buffer()[2] = 0; // spare
	this->get_payload_buffer()[3] = 0; // spare 

	// BEGIN Application TMY Handling here			
	this->get_payload_buffer()[0] = REPORT_IMU_AHRS_STATE;
	this->get_payload_buffer()[1] = (this->imu.get_status()) >= 0 ? true : false;
	this->get_payload_buffer()[2] = 0; // spare
	this->get_payload_buffer()[3] = 0; // spare 

	memcpy(  &this->get_payload_buffer()[4], 
			 reinterpret_cast<const uint8_t*>(this->imu.get_processed_values()), 40);	
	memcpy(  &this->get_payload_buffer()[4+40], 
			 reinterpret_cast<const uint8_t*>(this->imu.get_magacc_euler_angles()), 12);	
	memcpy(  &this->get_payload_buffer()[4+40+12], 
			 reinterpret_cast<const uint8_t*>(this->imu.get_gyro_euler_angles()), 12);	

	this->send(4 + (10*4) + (3*4) + (3*4) );
	// END Application TMY Handling here	
}

void application::update_imu()
{	
	if( !this->tmy[TMY_PARAM_STATUS] & STATUS_AHRS_FAIL )
	{
        this->imu.read();
        this->imu.transform_units();
        this->imu.calc_euler_angles_from_accmag();  
        this->imu.integrate_gyro_angles(millis());
	}
}
