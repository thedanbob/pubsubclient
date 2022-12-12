#pragma once

#include "MQTT/Message.h"

namespace MQTT {
  //! Disconnect from the broker
  class Disconnect : public Message {
  public:
    //! Constructor
    Disconnect() :
      Message(DISCONNECT)
    {}

  };
}
