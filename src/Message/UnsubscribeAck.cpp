#include "UnsubscribeAck.h"
#include "MQTT.h"

namespace MQTT {
  UnsubscribeAck::UnsubscribeAck(uint8_t* data, uint32_t length) :
    Message(UNSUBACK)
  {
    uint32_t pos = 0;
    _packet_id = read_uint16(data, pos);
  }
}
