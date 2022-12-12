#pragma once

#include "Message.h"

// MQTT_KEEPALIVE : keepAlive interval in Seconds
#define MQTT_KEEPALIVE 15

namespace MQTT {
  //! Message sent when connecting to a broker
  class Connect : public Message {
  protected:
    bool _clean_session;
    uint8_t _will_qos;
    bool _will_retain;

    String _clientid;
    String _will_topic;
    uint8_t *_will_message;
    uint16_t _will_message_len;
    String _username, _password;

    uint16_t _keepalive;

    uint32_t variable_header_length(void) const override;
    void write_variable_header(uint8_t *buf, uint32_t& bufpos) const override;
    uint32_t payload_length(void) const override;
    void write_payload(uint8_t *buf, uint32_t& bufpos) const override;

    message_type response_type(void) const override { return CONNACK; }

  public:
    //! Connect with a client ID
    explicit Connect(String cid);

    //! Set the "clear session" flag
    Connect& set_clean_session(bool cs = true) { _clean_session = cs; return *this; }
    //! Unset the "clear session" flag
    Connect& unset_clean_session(void) { _clean_session = false; return *this; }

    //! Set the "will" flag and associated attributes
    Connect& set_will(String willTopic, String willMessage, uint8_t willQos = 0, bool willRetain = false);
    //! Set the "will" flag and attributes, with an arbitrary will message
    Connect& set_will(String willTopic, uint8_t *willMessage, uint16_t willMessageLength, uint8_t willQos = 0, bool willRetain = false);
    //! Unset the "will" flag and associated attributes
    Connect& unset_will(void) { _will_topic = ""; return *this; }

    //! Set the username and password for authentication
    Connect& set_auth(String u, String p) { _username = u; _password = p; return *this; }
    //! Unset the username and password for authentication
    Connect& unset_auth(void) { _username = ""; _password = ""; return *this; }

    //! Get the keepalive period
    uint16_t keepalive(void) const { return _keepalive; }
    //! Set the keepalive period
    Connect& set_keepalive(uint16_t k) { _keepalive = k; return *this; }

    ~Connect();

  };
}
