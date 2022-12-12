#pragma once

#include "MQTT/Message.h"

namespace MQTT {
  //! Unsubscribe from one or more topics
  class Unsubscribe : public Message {
  private:
    uint8_t *_buffer;
    uint32_t _buflen;

    uint32_t variable_header_length(void) const override;
    void write_variable_header(uint8_t *buf, uint32_t& bufpos) const override;
    uint32_t payload_length(void) const override;
    void write_payload(uint8_t *buf, uint32_t& bufpos) const override;

    message_type response_type(void) const override { return UNSUBACK; }

  public:
    //! Constructor that starts with an empty list of unsubscriptions
    Unsubscribe();

    //! Constructor from a topic
    explicit Unsubscribe(String topic);

    ~Unsubscribe();

    //! Add another topic to unsubscribe from
    Unsubscribe& add_topic(String topic);

  };
}
