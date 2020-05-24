#ifndef MPU9250_AHRS_H
#define MPU9250_AHRS_H

#include <Arduino.h>
#include <Wire.h>
#include <stdint.h>

#define DEBUG_PRINT_FLAGS_ACC  0x01
#define DEBUG_PRINT_FLAGS_GYRO 0x02
#define DEBUG_PRINT_FLAGS_MAG  0x04

#define DEBUG_PRINT_MAGACC_EULER  0x01
#define DEBUG_PRINT_GYRO_EULER    0x02

class mpu9250 {
public:
    mpu9250();

    /**
     Initialize MPU9250
     @returns < 0 on failure.
    */
    int8_t setup();

    inline int8_t get_status() const { return this->status; }

    /**
     Set magnetometer bias correction
     */
    void set_mag_bias_correction(float x, float y, float z);

    /**
      Read from MPU9250.
     */
    void read();

    /**
      Map sensor readings to engineering units: 
        - Accelerometer: m/s^2 
        - Gyroscope:     radians per second
        - Magnetometer   uT (micro Tesla)
     */
    void transform_units();
    
    /**
      Calculate euler angles from acelerometer and magnetometer
     */  
    void calc_euler_angles_from_accmag();  

    /**
     Integrate gyroscope angular velocities
     @param t current time in milliseconds
     */
    void integrate_gyro_angles(uint32_t t);

    const int16_t* get_raw_values()           const { return this->raw_values; }
    const float*   get_processed_values()     const { return this->processed_values; }
    const float*   get_magacc_euler_angles()  const { return this->euler_angles_magacc; }
    const float*   get_gyro_euler_angles()    const { return this->euler_angles_gyro; }

    // Data
    enum imu_value_e{
      acc_x, acc_y, acc_z,
      gyro_x, gyro_y, gyro_z,
      mag_x, mag_y, mag_z,
      temperature,
      last    
    };
    enum euler_angle_e{ pitch = 0, roll = 1, yaw = 2 };

    /* Debug */    
    void debug_print_raw_values(int flags);
    void debug_print_processed_values(int flags);    
    void debug_print_euler_angles(int flags);
private:
    int8_t status;
    int16_t raw_values[imu_value_e::last];      /* raw readings from MPU9250 */
    float processed_values[imu_value_e::last];  /* raw readings from MPU9250 */

    /* Euler angles */
    float euler_angles_gyro[3];   // roll, pitch, yaw (radians)
    float euler_angles_magacc[3]; // roll, pitch, yaw (radians)

    /* Accelerometer */

    /* Gyroscope */
    uint32_t last_gyro_update;
    
    /* Magnetometer */
    bool mag_valid; // last reading was valid (mag)
    
    // Magnetometer :: Calibration
    float mag_bias_correction[3];
};

#endif // MPU9250_AHRS_H

