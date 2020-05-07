#ifndef IMU_AHRS_H
#define IMU_AHRS_H

#include <Arduino.h>
#include "MPU9250.h" // Bolder Flight Systems

class imu_ahrs : private MPU9250 {
public:  
  	imu_ahrs();
  	bool setup();
  	void update();
  	void read_values(float* dst);
private:  
	int status;
};

#endif // IMU_AHRS_H