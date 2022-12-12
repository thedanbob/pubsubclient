#pragma once

#include "MQTT/Message.h"
#include "MQTT/PacketParser.h"

namespace MQTT {
    //! Publish a payload to a topic
  class Publish : public Message {
  protected:
    String _topic;
    uint32_t _payload_len;
    uint8_t *_payload;
    bool _payload_mine;

    uint32_t variable_header_length(void) const override;
    void write_variable_header(uint8_t *buf, uint32_t& bufpos) const override;
    uint32_t payload_length(void) const override;
    void write_payload(uint8_t *buf, uint32_t& bufpos) const override;

    message_type response_type(void) const override;

    //! Private constructor from a payload and allowing _payload_mine to be set
    Publish(String topic, uint8_t* payload, uint32_t length, bool mine) :
      Message(PUBLISH),
      _topic(topic),
      _payload_len(length),
      _payload(payload),
      _payload_mine(mine)
    {}

    //! Private constructor from a network buffer
    Publish(uint8_t flags, uint8_t* data, uint32_t length);

    //! Private constructor from a network stream
    Publish(uint8_t flags, Client& client, uint32_t remaining_length);

    friend PacketParser;

  public:
    //! Constructor from string payload
    /*!
      \param topic Topic of this message
      \param payload Payload of this message
     */
    Publish(String topic, String payload);

    //! Constructor from arbitrary payload
    /*!
      \param topic Topic of this message
      \param payload Pointer to a block of data
      \param length The length of the data stored at 'payload'
     */
    Publish(String topic, uint8_t* payload, uint32_t length) :
      Publish(topic, payload, length, false)
    {}

    //! Constructor from a callback
    /*!
      \param topic Topic of this message
      \param pcb A callback function that writes the payload directly to the network Client object
      \param length The length of the data that 'pcb' will send
     */
    Publish(String topic, payload_callback_t pcb, uint32_t length);

    //! Constructor from a string stored in flash using the F() macro
    Publish(String topic, const __FlashStringHelper* payload);

    friend Publish Publish_P(String topic, PGM_P payload, uint32_t length);

    ~Publish();

    //! Get retain flag
    bool retain(void) const { return _flags & 0x01; }
    //! Set retain flag
    Publish& set_retain(bool r = true) { _flags = (_flags & ~0x01) | r; return *this; }
    //! Unset retain flag
    Publish& unset_retain(void) { _flags = _flags & ~0x01; return *this; }

    //! Get QoS value
    uint8_t qos(void) const { return (_flags >> 1) & 0x03; }
    //! Set QoS value
    Publish& set_qos(uint8_t q);
    //! Unset QoS value
    Publish& unset_qos(void) { _flags &= ~0x06; _need_packet_id = false; return *this; }

    //! Get dup flag
    bool dup(void) const { return (_flags >> 3) & 0x01; }
    //! Set dup flag
    Publish& set_dup(bool d = true) { _flags = (_flags & ~0x08) | (d ? 0x08 : 0); return *this; }
    //! Unset dup flag
    Publish& unset_dup(void) { _flags = _flags & ~0x08; return *this; }

    //! Get the topic string
    String topic(void) const { return _topic; }

    //! Get the payload as a string
    String payload_string(void) const;

    //! Get the payload pointer
    uint8_t* payload(void) const { return _payload; }
    //! Get the payload length
    uint32_t payload_len(void) const { return _payload_len; }

    //! Get the network stream for reading the payload
    Client* payload_stream(void) const { return _stream_client; }
  };

  //! A function made to look like a constructor, reading the payload from flash
  Publish Publish_P(String topic, PGM_P payload, uint32_t length);
}
