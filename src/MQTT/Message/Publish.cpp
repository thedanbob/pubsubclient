#include "MQTT/Message/Publish.h"
#include "MQTT/utils.h"

namespace MQTT {
  Publish::Publish(String topic, String payload) :
    Message(PUBLISH),
    _topic(topic),
    _payload_len(0),
    _payload(nullptr),
    _payload_mine(false)
  {
    if (payload.length() > 0) {
      _payload = new uint8_t[payload.length()];
      memcpy(_payload, payload.c_str(), payload.length());
      _payload_len = payload.length();
      _payload_mine = true;
    }
  }

  Publish::Publish(String topic, const __FlashStringHelper* payload) :
    Message(PUBLISH),
    _topic(topic),
    _payload_len(strlen_P((PGM_P)payload)),
    _payload(new uint8_t[_payload_len + 1]),
    _payload_mine(true)
  {
    strncpy_P((char*)_payload, (PGM_P)payload, _payload_len);
  }

  Publish Publish_P(String topic, PGM_P payload, uint32_t length) {
    uint8_t *p = new uint8_t[length];
    memcpy_P(p, payload, length);
    return Publish(topic, p, length, true);
  }

  Publish::Publish(uint8_t flags, uint8_t* data, uint32_t length) :
    Message(PUBLISH, flags),
    _payload_len(0),
    _payload(nullptr),
    _payload_mine(false)
  {
    uint32_t pos = 0;
    _topic = read_string(data, pos);
    if (qos() > 0)
      _packet_id = read_uint16(data, pos);

    _payload_len = length - pos;
    if (_payload_len > 0) {
      _payload = new uint8_t[_payload_len];
      memcpy(_payload, data + pos, _payload_len);
      _payload_mine = true;
    }
  }

  Publish::Publish(String topic, payload_callback_t pcb, uint32_t length) :
    Message(PUBLISH),
    _topic(topic),
    _payload_len(length),
    _payload(nullptr),
    _payload_mine(false)
  {
    _payload_callback = pcb;
  }

  Publish::Publish(uint8_t flags, Client& client, uint32_t remaining_length) :
    Message(PUBLISH, flags),
    _payload_len(remaining_length),
    _payload(nullptr),
    _payload_mine(false)
  {
    _stream_client = &client;

    // Read the topic
    _topic = read_string(client);
    _payload_len -= 2 + _topic.length();

    if (qos() > 0) {
      // Read the packet id
      _packet_id = read_uint16(client);
      _payload_len -= 2;
    }

    // Client stream is now at the start of the payload
  }

  Publish::~Publish() {
    if ((_payload_mine) && (_payload != nullptr))
      delete [] _payload;
  }

  Publish& Publish::set_qos(uint8_t q) {
    if (q > 2)
      q = 2;

    _flags &= ~0x06;
    if (q) {
      _flags |= q << 1;
      _need_packet_id = true;
    }
    return *this;
  }

  String Publish::payload_string(void) const {
    String str;
    str.reserve(_payload_len);
    for (uint32_t i = 0; i < _payload_len; i++)
      str += (char)_payload[i];

    return str;
  }

  uint32_t Publish::variable_header_length(void) const {
    return 2 + _topic.length() + (qos() ? 2 : 0);
  }

  void Publish::write_variable_header(uint8_t *buf, uint32_t& bufpos) const {
    write(buf, bufpos, _topic);
    if (qos())
      write_packet_id(buf, bufpos);
  }

  uint32_t Publish::payload_length(void) const {
    return _payload_len;
  }

  void Publish::write_payload(uint8_t *buf, uint32_t& bufpos) const {
    if (_payload != nullptr)
      write_bare_payload(buf, bufpos, _payload, _payload_len);
  }

  message_type Publish::response_type(void) const {
    switch (qos()) {
    case 0:
      return None;
    case 1:
      return PUBACK;
    case 2:
      return PUBREC;
    default:
      return None;
    }
  }
}
