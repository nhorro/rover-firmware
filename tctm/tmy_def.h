#ifndef TMY_DEF_H
#define TMY_DEF_H

// All telemetry
enum tmy_index_e {
	// General Telemetry
	TMY_PARAM_ACCEPTED_PACKETS			= 0x00,
	TMY_PARAM_REJECTED_PACKETS			= 0x01,
	TMY_PARAM_LAST_OPCODE				= 0x02,
	TMY_PARAM_LAST_ERROR				= 0x03,

	// BEGIN Application Specific Telemetry here
	// END Application Specific Telemetry here

	TMY_PARAM_LAST
};

#endif
