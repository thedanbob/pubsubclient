#include "PublishComp.h"
#include "MQTT.h"

namespace MQTT {
  PublishComp::PublishComp(uint16_t pid) :
    Message(PUBCOMP)
  {
    _packet_id = pid;
  }

  PublishComp::PublishComp(uint8_t* data, uint32_t length) :
    Message(PUBCOMP)
  {
    uint32_t pos = 0;
    _packet_id = read_uint16(data, pos);
  }

  uint32_t PublishComp::variable_header_length(void) const {
    return 2;
  }

  void PublishComp::write_variable_header(uint8_t *buf, uint32_t& bufpos) const {
    write_packet_id(buf, bufpos);
  }
}
