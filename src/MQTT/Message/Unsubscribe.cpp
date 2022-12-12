#include "MQTT/Message/Unsubscribe.h"
#include "MQTT/utils.h"

namespace MQTT {
  Unsubscribe::Unsubscribe() :
    Message(UNSUBSCRIBE),
    _buffer(nullptr), _buflen(0)
  {
    _need_packet_id = true;
  }

  Unsubscribe::Unsubscribe(String topic) :
    Message(UNSUBSCRIBE),
    _buffer(nullptr), _buflen(0)
  {
    _need_packet_id = true;
    _buffer = new uint8_t[2 + topic.length()];
    write(_buffer, _buflen, topic);
  }

  Unsubscribe::~Unsubscribe() {
    delete [] _buffer;
  }

  Unsubscribe& Unsubscribe::add_topic(String topic) {
    _buffer = (uint8_t*)realloc(_buffer, _buflen + 2 + topic.length());
    write(_buffer, _buflen, topic);

    return *this;
  }

  uint32_t Unsubscribe::variable_header_length(void) const {
    return 2;
  }

  void Unsubscribe::write_variable_header(uint8_t *buf, uint32_t& bufpos) const {
    write_packet_id(buf, bufpos);
  }

  uint32_t Unsubscribe::payload_length(void) const {
    return _buflen;
  }

  void Unsubscribe::write_payload(uint8_t *buf, uint32_t& bufpos) const {
    if (_buffer != nullptr)
      write_bare_payload(buf, bufpos, _buffer, _buflen);
  }
}
