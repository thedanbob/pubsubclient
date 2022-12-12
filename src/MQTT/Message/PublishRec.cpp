#include "MQTT/Message/PublishRec.h"
#include "MQTT/utils.h"

namespace MQTT {
  PublishRec::PublishRec(uint16_t pid) :
    Message(PUBREC)
  {
    _packet_id = pid;
  }

  PublishRec::PublishRec(uint8_t* data, uint32_t length) :
    Message(PUBREC)
  {
    uint32_t pos = 0;
    _packet_id = read_uint16(data, pos);
  }

  uint32_t PublishRec::variable_header_length(void) const {
    return 2;
  }

  void PublishRec::write_variable_header(uint8_t *buf, uint32_t& bufpos) const {
    write_packet_id(buf, bufpos);
  }
}
