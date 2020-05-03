#ifndef CMD_DEF_H
#define CMD_DEF_H

enum opcode_index_e {
	OPCODE_REQUEST_TMY						= 0x00,
	OPCODE_LED_ON							= 0x01,
	OPCODE_LED_OFF							= 0x02,

	// BEGIN Application Specific Opcodes here
	OPCODE_UPDATE_MOTOR_SPEEDS				= 0x03,

	// END Application Specific Opcodes here
	OPCODE_LAST
};

#endif
