#pragma once

#include "Message.h"
#include "PacketParser.h"

namespace MQTT {
  //! Response to Unsubscribe
  class UnsubscribeAck : public Message {
  private:
    //! Private constructor from a network buffer
    UnsubscribeAck(uint8_t* data, uint32_t length);

    friend PacketParser;

  };
}
