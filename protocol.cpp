#include "protocol.h"

namespace protocol {

packet_decoder::packet_decoder()
	:
		state_handlers {
			&packet_decoder::handle_pkt_state_idle,
			&packet_decoder::handle_pkt_state_expecting_start_sync1,
			&packet_decoder::handle_pkt_state_expecting_start_sync2,
			&packet_decoder::handle_pkt_state_expecting_start_sync3,
			&packet_decoder::handle_pkt_state_expecting_length,
			&packet_decoder::handle_pkt_state_expecting_payload,
			&packet_decoder::handle_pkt_state_expecting_crc1,
			&packet_decoder::handle_pkt_state_expecting_crc2,
			&packet_decoder::handle_pkt_state_expecting_terminator
		}
	, last_received_packet_t0(millis())
{
	this->reset();
}

void packet_decoder::handle_pkt_state_idle()
{
	if (PACKET_SYNC_0_CHAR == this->last_received_char)
	{
		this->current_state =
				pkt_state::pkt_state_expecting_start_sync1;
	}
}

void packet_decoder::handle_pkt_state_expecting_start_sync1()
{
	if (PACKET_SYNC_1_CHAR == this->last_received_char)
	{
		this->current_state =
				pkt_state::pkt_state_expecting_start_sync2;
	}
	else
	{
		this->set_error(error_code::bad_sync);
		this->reset();
	}
}

void packet_decoder::handle_pkt_state_expecting_start_sync2()
{
	if (PACKET_SYNC_2_CHAR == this->last_received_char)
	{
		this->current_state =
				pkt_state::pkt_state_expecting_start_sync3;
	}
	else
	{
		this->set_error(error_code::bad_sync);
		this->reset();
	}
}

void packet_decoder::handle_pkt_state_expecting_start_sync3()
{
	if (PACKET_SYNC_3_CHAR == this->last_received_char)
	{
		this->current_state = pkt_state::pkt_state_expecting_length;
	}
	else
	{
		this->set_error(error_code::bad_sync);
		this->reset();
	}
}

void packet_decoder::handle_pkt_state_expecting_length()
{
	this->payload_length = this->last_received_char;
	if (this->payload_length && (this->payload_length <= PAYLOAD_BUFFER_SIZE))
	{
		this->current_state = pkt_state::pkt_state_expecting_payload;
	}
	else
	{
		this->set_error(error_code::invalid_length);
		this->reset();
	}
}

void packet_decoder::handle_pkt_state_expecting_payload()
{
	this->received_payload_buffer[this->received_payload_index] =
			this->last_received_char;
	if (++this->received_payload_index == this->payload_length)
	{
		this->expected_crc16 = calc_crc16(
				this->received_payload_buffer,
				this->received_payload_index);
		this->received_payload_buffer[this->received_payload_index] =
				'\0';
		this->current_state = pkt_state::pkt_state_expecting_crc1;
	}
}

void packet_decoder::handle_pkt_state_expecting_crc1()
{
	this->crc16 = this->last_received_char << 8;
	this->current_state = pkt_state::pkt_state_expecting_crc2;
}

void packet_decoder::handle_pkt_state_expecting_crc2()
{
	this->crc16 |= this->last_received_char;

	if (this->expected_crc16 == this->crc16)
	{
		this->current_state = pkt_state::pkt_state_expecting_terminator;
	}
	else
	{
		this->set_error(error_code::bad_crc);
		this->reset();
	}
}

void packet_decoder::handle_pkt_state_expecting_terminator()
{
	if (PACKET_TERMINATOR_CHAR == this->last_received_char)
	{
		this->last_received_packet_t0 = millis();
		this->handle_packet(this->received_payload_buffer,
				this->received_payload_index);
	}
	else
	{
		this->set_error(error_code::bad_terminator);
	}
	this->reset();	
}


void packet_decoder::feed(uint8_t c)
{
	this->last_received_char = c;

	(this->*(state_handlers[static_cast<int>(this->current_state)]))();
}

void packet_decoder::check_timeouts()
{
	// Handle timeouts here	
	uint32_t t1 = millis();

	// Check timeout of current packet being processed
	uint32_t dt = this->start_of_packet_t0 > t1 ? 
		1 + this->start_of_packet_t0 + ~t1 : t1 - this->start_of_packet_t0;
	if(dt>=PACKET_TIMEOUT_IN_MS)
	{
    	this->reset();
  	}

  	// Check heartbeat timeout
	dt = this->last_received_packet_t0 > t1 ? 
		1 + this->last_received_packet_t0 + ~t1 : t1 - this->last_received_packet_t0;
	if(dt>HEARTBEAT_TIMEOUT_IN_MS)
	{
    	this->handle_connection_lost();
  	}

}

void packet_decoder::reset()
{
	this->current_state = pkt_state::pkt_state_idle;
	this->received_payload_index = 0;
	this->start_of_packet_t0 = millis();
	this->crc16 = 0;
}

packet_encoder::packet_encoder() 
{
	this->buffer[0] = PACKET_SYNC_0_CHAR;
	this->buffer[1] = PACKET_SYNC_1_CHAR;
	this->buffer[2] = PACKET_SYNC_2_CHAR;
	this->buffer[3] = PACKET_SYNC_3_CHAR;
}

void packet_encoder::calc_crc_and_close_packet(uint8_t length)
{	
	buffer[HEADER_SIZE-1] = length;
	uint16_t crc = calc_crc16(this->get_payload_buffer(), length);
	buffer[HEADER_SIZE + length] = (crc >> 8) & 0xFF;	
	buffer[HEADER_SIZE + length + 1] = crc & 0xFF;
	buffer[HEADER_SIZE + length + 2] = PACKET_TERMINATOR_CHAR;
}

uint8_t* packet_encoder::get_payload_buffer()
{
	return this->buffer + HEADER_SIZE;
}

const uint8_t* packet_encoder::get_packet() const
{
	return this->buffer;
}

void packet_encoder::send(uint8_t length)
{
	this->calc_crc_and_close_packet(length);
	this->send_impl(this->get_packet(),
		HEADER_SIZE+
		buffer[HEADER_SIZE-1]+
		TRAILER_SIZE
	);	
}

uint16_t calc_crc16(const uint8_t* data_p, uint8_t length)
{
	uint8_t x;
	uint16_t crc = 0xFFFF;
	while (length--)
	{
		x = crc >> 8 ^ *data_p++;
		x ^= x >> 4;
		crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x << 5))
				^ ((uint16_t) x);
	}
	return crc;
}

} // namespace protocol
