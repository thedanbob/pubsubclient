#pragma once

#include "MQTT/Message.h"
#include "MQTT/PacketParser.h"

namespace MQTT {
  //! First response to Publish when qos == 2
  class PublishRec : public Message {
  private:
    uint32_t variable_header_length(void) const override;
    void write_variable_header(uint8_t *buf, uint32_t& bufpos) const override;

    message_type response_type(void) const override { return PUBREL; }

    //! Private constructor from a network buffer
    PublishRec(uint8_t* data, uint32_t length);

    friend PacketParser;

  public:
    //! Constructor from a packet id
    explicit PublishRec(uint16_t pid);

  };
}
