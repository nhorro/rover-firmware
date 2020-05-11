#include "imu_ahrs.h"


imu_ahrs::imu_ahrs()
	: 
		MPU9250(Wire,0x68)
{
}

bool imu_ahrs::setup()
{
	// start communication with IMU 
	this->status = this->begin();
	if ( this->status >= 0 )
	{
	  	// setting the accelerometer full scale range to +/-8G 
  		this->setAccelRange(MPU9250::ACCEL_RANGE_8G);
  		// setting the gyroscope full scale range to +/-500 deg/s
  		this->setGyroRange(MPU9250::GYRO_RANGE_500DPS);
  		// setting DLPF bandwidth to 20 Hz
  		this->setDlpfBandwidth(MPU9250::DLPF_BANDWIDTH_20HZ);
  		// setting SRD to 19 for a 50 Hz update rate
  		this->setSrd(19);	
	}
	return (this->status >= 0);
}

void imu_ahrs::update()
{
	if(this->status>=0)
	{
		this->readSensor();	
	}
}

void imu_ahrs::read_values(float* dst)
{
	if( this->status>=0 )
	{
		dst[0] = this->getAccelX_mss();
		dst[1] = this->getAccelY_mss();
		dst[2] = this->getAccelZ_mss();

		dst[3] = this->getGyroX_rads();
		dst[4] = this->getGyroY_rads();
		dst[5] = this->getGyroZ_rads();

		dst[6] = this->getMagX_uT();
		dst[7] = this->getMagY_uT();
		dst[8] = this->getMagZ_uT();

		dst[9] = this->getTemperature_C();
	}
	else
	{
		// TODO: estimate
		dst[0] = 0.0;
		dst[1] = 0.0;
		dst[2] = 0.0;

		dst[3] = 0.0;
		dst[4] = 0.0;
		dst[5] = 0.0;

		dst[6] = 0.0;
		dst[7] = 0.0;
		dst[8] = 0.0;

		dst[9] = 0.0;
	}
}