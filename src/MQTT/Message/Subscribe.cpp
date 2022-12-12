#include "MQTT/Message/Subscribe.h"
#include "MQTT/utils.h"

namespace MQTT {
  Subscribe::Subscribe() :
    Message(SUBSCRIBE),
    _buffer(nullptr), _buflen(0)
  {
    _need_packet_id = true;
  }

  Subscribe::Subscribe(String topic, uint8_t qos) :
    Message(SUBSCRIBE),
    _buffer(nullptr), _buflen(0)
  {
    _need_packet_id = true;
    _buffer = new uint8_t[2 + topic.length() + 1];
    write(_buffer, _buflen, topic);
    _buffer[_buflen++] = qos;
  }

  Subscribe::~Subscribe() {
    delete [] _buffer;
  }

  Subscribe& Subscribe::add_topic(String topic, uint8_t qos) {
    _buffer = (uint8_t*)realloc(_buffer, _buflen + 2 + topic.length() + 1);
    write(_buffer, _buflen, topic);
    _buffer[_buflen++] = qos;
    return *this;
  }

  uint32_t Subscribe::variable_header_length(void) const {
    return 2;
  }

  void Subscribe::write_variable_header(uint8_t *buf, uint32_t& bufpos) const {
    write_packet_id(buf, bufpos);
  }

  uint32_t Subscribe::payload_length(void) const {
    return _buflen;
  }

  void Subscribe::write_payload(uint8_t *buf, uint32_t& bufpos) const {
    if (_buffer != nullptr)
      write_bare_payload(buf, bufpos, _buffer, _buflen);
  }
}
