#pragma once

#include "MQTT/Message.h"
#include "MQTT/PacketParser.h"

namespace MQTT {
  //! Response to PublishRel
  class PublishComp : public Message {
  private:
    uint32_t variable_header_length(void) const override;
    void write_variable_header(uint8_t *buf, uint32_t& bufpos) const override;

    //! Private constructor from a network buffer
    PublishComp(uint8_t* data, uint32_t length);

    friend PacketParser;

  public:
    //! Constructor from a packet id
    explicit PublishComp(uint16_t pid);

  };
}
