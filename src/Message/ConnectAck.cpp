#include "Message/ConnectAck.h"
#include "MQTT.h"

namespace MQTT {
  ConnectAck::ConnectAck(uint8_t* data, uint32_t length) :
    Message(CONNACK)
  {
    uint32_t pos = 0;
    uint8_t reserved = read_uint8(data, pos);
    _session_present = reserved & 0x01;
    _rc = read_uint8(data, pos);
  }
}
