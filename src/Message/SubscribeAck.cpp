#include "Message/SubscribeAck.h"
#include "MQTT.h"

namespace MQTT {
  SubscribeAck::SubscribeAck(uint8_t* data, uint32_t length) :
    Message(SUBACK),
    _rcs(nullptr)
  {
    uint32_t pos = 0;
    _packet_id = read_uint16(data, pos);

    _num_rcs = length - pos;
    if (_num_rcs > 0) {
      _rcs = new uint8_t[_num_rcs];
      for (uint32_t i = 0; i < _num_rcs; i++)
        _rcs[i] = read_uint8(data, pos);
    }
  }

  SubscribeAck::SubscribeAck(Client& client, uint32_t remaining_length) :
    Message(SUBACK),
    _rcs(nullptr),
    _num_rcs(remaining_length - 2)
  {
    _stream_client = &client;

    // Read packet id
    _packet_id = read_uint16(client);

    // Client stream is now at the start of the list of rcs
  }

  SubscribeAck::~SubscribeAck() {
    if (_rcs != nullptr)
      delete [] _rcs;
  }

  uint8_t SubscribeAck::next_rc(void) const {
    return read_uint8(*_stream_client);
  }
}
