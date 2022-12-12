#include "Message/PublishAck.h"
#include "MQTT.h"

namespace MQTT {
  PublishAck::PublishAck(uint16_t pid) :
    Message(PUBACK)
  {
    _packet_id = pid;
  }

  PublishAck::PublishAck(uint8_t* data, uint32_t length) :
    Message(PUBACK)
  {
    uint32_t pos = 0;
    _packet_id = read_uint16(data, pos);
  }
}
