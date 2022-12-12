#pragma once

#include "MQTT/Message.h"
#include "MQTT/PacketParser.h"

namespace MQTT {
  //! Response to Publish when qos == 1
  class PublishAck : public Message {
  private:
    uint32_t variable_header_length(void) const override { return sizeof(_packet_id); }
    void write_variable_header(uint8_t *buf, uint32_t& bufpos) const override { write_packet_id(buf, bufpos); }

    //! Private constructor from a network buffer
    PublishAck(uint8_t* data, uint32_t length);

    friend PacketParser;

  public:
    //! Constructor from a packet id
    explicit PublishAck(uint16_t pid);

  };
}
