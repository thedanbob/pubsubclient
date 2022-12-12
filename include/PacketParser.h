#pragma once

#include "Message.h"

// Packets larger than this can only be streamed
#ifndef MQTT_TOO_BIG
  #define MQTT_TOO_BIG 4096
#endif

namespace MQTT {
  //! Packet parser
  class PacketParser {
  private:
    enum class State {
      Start,
      ReadTypeFlags = 0,
      ReadLength,
      ReadContents,
      CreateObject,
      HaveObject,
    };

    Client &_client;
    State _state;
    uint8_t _flags, _type, _length_shifter;
    uint32_t _remaining_length, _to_read;
    uint8_t *_remaining_data, *_read_point;
    Message *_msg;

    bool _read_type_flags(void);
    bool _read_length(void);
    bool _read_remaining(void);
    bool _construct_object(void);

  public:
    explicit PacketParser(Client& client);

  /*!
    remember to free the object once you're finished with it
    \return A pointer to an object derived from the Message class, representing the packet. If no complete packet was available, nullptr is returned.
  */
    Message* parse(void);
  };
}
