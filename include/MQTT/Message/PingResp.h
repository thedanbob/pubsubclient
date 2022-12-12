#pragma once

#include "MQTT/Message.h"

namespace MQTT {
  //! Response to Ping
  class PingResp : public Message {
  public:
    //! Constructor
    PingResp() :
      Message(PINGRESP)
    {}

  };
}
