#pragma once

#include "MQTT/Message.h"
#include "MQTT/PacketParser.h"

namespace MQTT {
  //! Response to Subscribe
  class SubscribeAck : public Message {
  private:
    uint8_t *_rcs;
    uint32_t _num_rcs;

    //! Private constructor from a network buffer
    SubscribeAck(uint8_t* data, uint32_t length);

    //! Private constructor from a network stream
    SubscribeAck(Client& client, uint32_t remaining_length);

    friend PacketParser;

  public:
    ~SubscribeAck();

    //! Get the number of return codes available
    uint32_t num_rcs(void) const { return _num_rcs; }

    //! Get a return code
    uint8_t rc(uint8_t i) const { return _rcs[i]; }

    //! Get the next return code from a stream
    uint8_t next_rc(void) const;

  };
}
