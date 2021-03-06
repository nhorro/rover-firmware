#ifndef CONFIG_H
#define CONFIG_H

/* Serial Configuration */

#define SerialIF 				Serial  // Use Serial0 (USB) for development with desktop PC.
//#define SerialIF 				Serial1 // Use Serial1 with Pi to avoid problems with Serial0 and USB.

#define APP_SERIAL_IF_BAUDRATE	115200

#define PACKET_TIMEOUT_IN_MS 	1000
#define HEARTBEAT_TIMEOUT_IN_MS 1000


/* General report frequency*/
#define GENERAL_TMY_REPORT_FREQ 	 1

/* IMU/AHRS Configuration */
#define IMU_UPDATE_FREQ 			40
#define IMU_REPORT_FREQ 		 	 5


#endif