#pragma once

#include "Message.h"

namespace MQTT {
  //! Ping the broker
  class Ping : public Message {
  private:
    message_type response_type(void) const override { return PINGRESP; }

  public:
    //! Constructor
    Ping() :
      Message(PINGREQ)
    {}

  };
}
