#ifndef CMD_DEF_H
#define CMD_DEF_H

enum opcode_index_e {
	OPCODE_REQUEST_TMY						= 0x00,
	OPCODE_LED_ON							= 0x01,
	OPCODE_LED_OFF							= 0x02,
	OPCODE_UPDATE_MOTOR_SPEEDS				= 0x03,
	OPCODE_LAST
};

#endif
