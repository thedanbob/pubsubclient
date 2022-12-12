#pragma once

#include "MQTT/Message.h"
#include "MQTT/PacketParser.h"

namespace MQTT {
  //! Response to PublishRec
  class PublishRel : public Message {
  private:
    uint32_t variable_header_length(void) const override;
    void write_variable_header(uint8_t *buf, uint32_t& bufpos) const override;

    message_type response_type(void) const override { return PUBCOMP; }

    //! Private constructor from a network buffer
    PublishRel(uint8_t* data, uint32_t length);

    friend PacketParser;

  public:
    //! Constructor from a packet id
    explicit PublishRel(uint16_t pid);

  };
}
