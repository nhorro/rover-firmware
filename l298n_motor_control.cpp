#include "l298n_motor_control.h"

#define DEFAULT_MOTOR_ACC 1.0 

l298_motor_control::l298_motor_control() 
: 
  motors {
	    { L298_PIN_ENA, L298_PIN_IN1, L298_PIN_IN2, 0.0, 0.0, DEFAULT_MOTOR_ACC },
	    { L298_PIN_ENB, L298_PIN_IN3, L298_PIN_IN4, 0.0, 0.0, DEFAULT_MOTOR_ACC }
	}
{

}

void l298_motor_control::setup()
{    
	for(int i=0;i<2;i++)
	{
		// All pins as output
		pinMode( this->motors[i].ena_pin, OUTPUT );
		pinMode( this->motors[i].in1_pin, OUTPUT );
		pinMode( this->motors[i].in2_pin, OUTPUT );
	}     
}

void l298_motor_control::update_motor_speeds(int16_t speeds[2], uint8_t mask )
{
 	for(int i=0;i<2;i++)
 	{
	    if ( mask & (motor_control_flags::motor_a<<i) ) 
	    {	    	
			this->motors[i].current_speed = speeds[i];
			if ( this->motors[i].current_speed >= 0 ) 
			{
			  digitalWrite (this->motors[i].in1_pin, HIGH);
			  digitalWrite (this->motors[i].in2_pin, LOW);
			}
			else
			{
			  digitalWrite (this->motors[i].in1_pin, LOW);
			  digitalWrite (this->motors[i].in2_pin, HIGH);        
			} 
			analogWrite ( this->motors[i].ena_pin, abs(this->motors[i].current_speed) ); 
	    }        
	}
}