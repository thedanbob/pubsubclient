#include "MQTT/PacketParser.h"
#include "MQTT/utils.h"
#include "MQTT/Message/ConnectAck.h"
#include "MQTT/Message/Ping.h"
#include "MQTT/Message/PingResp.h"
#include "MQTT/Message/Publish.h"
#include "MQTT/Message/PublishAck.h"
#include "MQTT/Message/PublishComp.h"
#include "MQTT/Message/PublishRec.h"
#include "MQTT/Message/PublishRel.h"
#include "MQTT/Message/SubscribeAck.h"
#include "MQTT/Message/UnsubscribeAck.h"

namespace MQTT {
  PacketParser::PacketParser(Client& client) :
    _client(client),
    _state(State::Start),
    _msg(nullptr)
  {}

  bool PacketParser::_read_type_flags(void) {
    if (_client.available() < 1)
      return false;

    // Read type and flags
    uint8_t type_flags = read_uint8(_client);
    _flags = type_flags & 0x0f;
    _type = type_flags >> 4;

    _state = State::ReadLength;
    _remaining_length = 0;
    _length_shifter = 0;

    return true;
  }

  bool PacketParser::_read_length(void) {
    // Read remaining length of packet
    {
      uint8_t digit;
      do {
        if (_client.available() < 1)
          return false;

        digit = read_uint8(_client);
        _remaining_length += (digit & 0x7f) << _length_shifter;
        _length_shifter += 7;
      } while (digit & 0x80);
    }
    _to_read = _remaining_length;

    // If the packet is too big, only allow streaming it
    if (_remaining_length > MQTT_TOO_BIG) {
      _state = State::CreateObject;
      return true;
    }

    // Otherwise, allocate a buffer to read the data
    if (_remaining_length > 0)
      _remaining_data = new uint8_t[_remaining_length];
    else
      _remaining_data = nullptr;
    _read_point = _remaining_data;

    _state = State::ReadContents;

    return true;
  }

  bool PacketParser::_read_remaining(void) {
    while ((_to_read > 0) && (_client.available() > 0)) {
      int read_size = _client.read(_read_point, _to_read);
      if (read_size == -1)
        return false;
      _to_read -= read_size;
      _read_point += read_size;
    }

    // Come around again if we haven't read everything
    if (_to_read > 0)
      return false;

    _state = State::CreateObject;
    return true;
  }

  bool PacketParser::_construct_object(void) {
    if (_remaining_length > MQTT_TOO_BIG) {
      switch (_type) {
      case PUBLISH:
        _msg = new Publish(_flags, _client, _remaining_length);
        break;

      case SUBACK:
        _msg = new SubscribeAck(_client, _remaining_length);
        break;

      default:
        _msg = nullptr;
      }
      _state = State::HaveObject;
      return true;
    }

    switch (_type) {
    case CONNACK:
      _msg = new ConnectAck(_remaining_data, _remaining_length);
      break;

    case PUBLISH:
      _msg = new Publish(_flags, _remaining_data, _remaining_length);
      break;

    case PUBACK:
      _msg = new PublishAck(_remaining_data, _remaining_length);
      break;

    case PUBREC:
      _msg = new PublishRec(_remaining_data, _remaining_length);
      break;

    case PUBREL:
      _msg = new PublishRel(_remaining_data, _remaining_length);
      break;

    case PUBCOMP:
      _msg = new PublishComp(_remaining_data, _remaining_length);
      break;

    case SUBACK:
      _msg = new SubscribeAck(_remaining_data, _remaining_length);
      break;

    case UNSUBACK:
      _msg = new UnsubscribeAck(_remaining_data, _remaining_length);
      break;

    case PINGREQ:
      _msg = new Ping;
      break;

    case PINGRESP:
      _msg = new PingResp;
      break;

    }
    if (_remaining_data != nullptr)
      delete [] _remaining_data;

    _state = State::HaveObject;
    return true;
  }

  Message* PacketParser::parse(void) {
    while (_state != State::HaveObject) {
      switch (_state) {
      case State::ReadTypeFlags:
        if (!_read_type_flags())
          return nullptr;

        break;

      case State::ReadLength:
        if (!_read_length())
          return nullptr;

        break;

      case State::ReadContents:
        if (!_read_remaining())
          return nullptr;

        break;

      case State::CreateObject:
        if (!_construct_object())
          return nullptr;

        break;

      default:
        break;
      }
    }

    _state = State::Start; // Reset the parser state machine
    return _msg;
  }
}
