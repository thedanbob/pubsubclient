#include "MQTT/Message/PublishRel.h"
#include "MQTT/utils.h"

namespace MQTT {
  PublishRel::PublishRel(uint16_t pid) :
    Message(PUBREL)
  {
    _packet_id = pid;
  }

  PublishRel::PublishRel(uint8_t* data, uint32_t length) :
    Message(PUBREL)
  {
    uint32_t pos = 0;
    _packet_id = read_uint16(data, pos);
  }

  uint32_t PublishRel::variable_header_length(void) const {
    return 2;
  }

  void PublishRel::write_variable_header(uint8_t *buf, uint32_t& bufpos) const {
    write_packet_id(buf, bufpos);
  }
}
