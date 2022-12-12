#include "MQTT/Message/Connect.h"
#include "MQTT/utils.h"

namespace MQTT {
  Connect::Connect(String cid) :
    Message(CONNECT),
    _clean_session(true),
    _clientid(cid),
    _will_message(nullptr), _will_message_len(0),
    _keepalive(MQTT_KEEPALIVE)
  {}

  Connect& Connect::set_will(String willTopic, String willMessage, uint8_t willQos, bool willRetain) {
    _will_topic = willTopic;
    _will_qos = willQos;
    _will_retain = willRetain;

    if (_will_message != nullptr)
      delete [] _will_message;

    _will_message_len = willMessage.length();
    _will_message = new uint8_t[_will_message_len];
    memcpy(_will_message, willMessage.c_str(), _will_message_len);

    return *this;
  }

  Connect& Connect::set_will(String willTopic, uint8_t *willMessage, uint16_t willMessageLength, uint8_t willQos, bool willRetain) {
    _will_topic = willTopic;
    _will_qos = willQos;
    _will_retain = willRetain;

    if (_will_message != nullptr)
      delete [] _will_message;

    _will_message_len = willMessageLength;
    _will_message = new uint8_t[_will_message_len];
    memcpy(_will_message, willMessage, _will_message_len);

    return *this;
  }

  Connect::~Connect() {
    if (_will_message != nullptr)
      delete [] _will_message;
  }

  uint32_t Connect::variable_header_length(void) const {
    return 10;
  }

  void Connect::write_variable_header(uint8_t *buf, uint32_t& bufpos) const {
    write(buf, bufpos, "MQTT"); // Protocol name
    buf[bufpos++] = 4;          // Protocol level
    buf[bufpos] = 0;            // Connect flags

    if (_clean_session)
      buf[bufpos] |= 0x02;

    if (_will_topic.length()) {
      buf[bufpos] |= 0x04;

      if (_will_qos > 2)
        buf[bufpos] |= 2 << 3;
      else
        buf[bufpos] |= _will_qos << 3;
      buf[bufpos] |= _will_retain << 5;
    }

    if (_username.length()) {
      buf[bufpos] |= 0x80;
      if (_password.length())
        buf[bufpos] |= 0x40;
    }
    bufpos++;

    write(buf, bufpos, _keepalive); // Keepalive period
  }

  uint32_t Connect::payload_length(void) const {
    uint32_t len = 2 + _clientid.length();
    if (_will_topic.length()) {
      len += 2 + _will_topic.length();
      len += 2 + _will_message_len;
    }
    if (_username.length()) {
      len += 2 + _username.length();
      if (_password.length())
        len += 2 + _password.length();
    }
    return len;
  }

  void Connect::write_payload(uint8_t *buf, uint32_t& bufpos) const {
    write(buf, bufpos, _clientid);

    if (_will_topic.length()) {
      write(buf, bufpos, _will_topic);
      write(buf, bufpos, _will_message, _will_message_len);
    }

    if (_username.length()) {
      write(buf, bufpos, _username);
      if (_password.length())
        write(buf, bufpos, _password);
    }
  }
}
