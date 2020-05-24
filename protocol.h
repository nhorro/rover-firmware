#ifndef PACKET_DECODER_H
#define PACKET_DECODER_H

#include "config.h"
#include <stdint.h>
#include <Arduino.h> // millis

namespace protocol {

#define HEADER_SIZE (4+1) /* SYNC + LENGTH */
#define PAYLOAD_BUFFER_SIZE 96
#define TRAILER_SIZE (2+1) /* CRC16 + TERMINATOR */

#define MAX_PACKET_SIZE = HEADER_SIZE + PAYLOAD_BUFFER_SIZE + TRAILER_SIZE

const char PACKET_SYNC_0_CHAR = 'P';
const char PACKET_SYNC_1_CHAR = 'K';
const char PACKET_SYNC_2_CHAR = 'T';
const char PACKET_SYNC_3_CHAR = '!';
const char PACKET_TERMINATOR_CHAR = '\n';



class packet_encoder
{
public:
	packet_encoder();
	uint8_t* get_payload_buffer();
	void send(uint8_t length);
	
	/** Overriden by user */
	virtual void send_impl(const uint8_t* buf, uint8_t n) = 0;
private:
	uint8_t buffer[HEADER_SIZE+PAYLOAD_BUFFER_SIZE+TRAILER_SIZE] __attribute__((aligned (4)));

protected:
	void calc_crc_and_close_packet(uint8_t length);
	const uint8_t* get_packet() const;
};

class packet_decoder
{
public:
	const uint32_t cmd_timeout = 1000000;

	enum error_code
	{
		success = 0,
		bad_sync = 1,
		invalid_length = 2,
		bad_crc = 3,
		bad_terminator = 4,
		unknown_opcode = 5,
		timeout
	};

	packet_decoder();

	void feed(uint8_t c);
	void check_timeouts();
	void reset();

	/** Overriden by user */
	virtual void handle_packet(const uint8_t* payload, uint8_t n) = 0;
	virtual void set_error(error_code ec) = 0;	
	virtual void handle_connection_lost() = 0;
private:
	enum pkt_state
	{
		pkt_state_idle=0,
		pkt_state_expecting_start_sync1,
		pkt_state_expecting_start_sync2,
		pkt_state_expecting_start_sync3,
		pkt_state_expecting_length,
		pkt_state_expecting_payload,
		pkt_state_expecting_crc1,
		pkt_state_expecting_crc2,
		pkt_state_expecting_terminator,
		pkt_state_last
	};
	using packet_state_handler = void(packet_decoder::*)(void);
	packet_state_handler state_handlers[pkt_state::pkt_state_last];

	// Times
	uint32_t start_of_packet_t0;
	uint32_t last_received_packet_t0;

	pkt_state current_state;
	uint8_t received_payload_buffer[PAYLOAD_BUFFER_SIZE + 1];
	uint8_t received_payload_index;
	uint16_t expected_crc16;
	uint16_t crc16;
	uint8_t last_received_char;
	bool new_data_available;
	uint8_t payload_length;

	void handle_pkt_state_idle();
	void handle_pkt_state_expecting_start_sync1();
	void handle_pkt_state_expecting_start_sync2();
	void handle_pkt_state_expecting_start_sync3();
	void handle_pkt_state_expecting_length();
	void handle_pkt_state_expecting_payload();
	void handle_pkt_state_expecting_crc1();
	void handle_pkt_state_expecting_crc2();
	void handle_pkt_state_expecting_terminator();
};

uint16_t calc_crc16(const uint8_t* data_p, uint8_t length);

} // namespace serialprotocol

#endif // PACKET_DECODER_H
