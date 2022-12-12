#pragma once

#include "MQTT/Message.h"
#include "MQTT/PacketParser.h"

namespace MQTT {
  //! Response to Connect
  class ConnectAck : public Message {
  private:
    bool _session_present;
    uint8_t _rc;

    //! Private constructor from a network buffer
    ConnectAck(uint8_t* data, uint32_t length);

    friend PacketParser;

  public:
    uint8_t rc(void) const { return _rc; }

  };
}
