#pragma once

#include "MQTT/Message.h"

namespace MQTT {
  //! Subscribe to one or more topics
  class Subscribe : public Message {
  private:
    uint8_t *_buffer;
    uint32_t _buflen;

    uint32_t variable_header_length(void) const override;
    void write_variable_header(uint8_t *buf, uint32_t& bufpos) const override;
    uint32_t payload_length(void) const override;
    void write_payload(uint8_t *buf, uint32_t& bufpos) const override;

    message_type response_type(void) const override { return SUBACK; }

  public:
    //! Constructor that starts an empty list of subscriptions
    Subscribe();

    //! Constructor from a topic and optional QoS level
    explicit Subscribe(String topic, uint8_t qos = 0);

    ~Subscribe();

    //! Add another topic and optional QoS level
    Subscribe& add_topic(String topic, uint8_t qos = 0);

  };
}
