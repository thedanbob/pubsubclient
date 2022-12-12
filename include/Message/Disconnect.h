#pragma once

#include "Message.h"

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
