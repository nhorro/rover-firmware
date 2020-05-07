#ifndef REPORT_DEF_H
#define REPORT_DEF_H

enum report_index_e {
	// Basic application reports
	REPORT_TELEMETRY_REQUEST       					=	0x80,
	REPORT_COMMAND_EXECUTION_STATUS					=	0x81,

	// BEGIN Application Specific Reports here
	REPORT_IMU_AHRS_STATE							=	0x82,
	// END Application Specific Reports here

	REPORT__LAST
};

#endif // REPORT_DEF_H
