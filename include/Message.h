#pragma once

#if defined(ESP8266) || defined(ESP32)
#include <pgmspace.h>
#include <functional>
#endif

#include <Arduino.h>
#include <Client.h>

class PubSubClient;

//! namespace for classes representing MQTT messages
namespace MQTT {

  enum message_type {
    None,
    CONNECT,      // Client request to connect to Server
    CONNACK,      // Connect Acknowledgment
    PUBLISH,      // Publish message
    PUBACK,       // Publish Acknowledgment
    PUBREC,       // Publish Received (assured delivery part 1)
    PUBREL,       // Publish Release (assured delivery part 2)
    PUBCOMP,      // Publish Complete (assured delivery part 3)
    SUBSCRIBE,    // Client Subscribe request
    SUBACK,       // Subscribe Acknowledgment
    UNSUBSCRIBE,  // Client Unsubscribe request
    UNSUBACK,     // Unsubscribe Acknowledgment
    PINGREQ,      // PING Request
    PINGRESP,     // PING Response
    DISCONNECT,   // Client is Disconnecting
    Reserved,     // Reserved
  };

  //! The Quality of Service (QoS) level is an agreement between sender and receiver of a message regarding the guarantees of delivering a message.
  enum Qos {
      QOS0 = 0, //! At most once
      QOS1 = 1, //! At least once
      QOS2 = 2  //! Exactly once
  };

#ifdef _GLIBCXX_FUNCTIONAL
  typedef std::function<bool(Client&)> payload_callback_t;
#else
  typedef bool(*payload_callback_t)(Client&);
#endif

  //! Abstract base class
  class Message {
  protected:
    message_type _type;
    uint8_t _flags;
    uint16_t _packet_id;  //! Not all message types use a packet id, but most do
    bool _need_packet_id;
    Client* _stream_client;
    payload_callback_t _payload_callback;

    //! Private constructor from type and flags
    explicit Message(message_type t, uint8_t f = 0) :
      _type(t), _flags(f),
      _packet_id(0), _need_packet_id(false),
      _stream_client(nullptr),
      _payload_callback(nullptr)
    {}

    //! Virtual destructor
    virtual ~Message() {}

    //! Length of the fixed header
    /*!
      \param rlength Remaining lengh i.e variable header + payload
    */
    uint8_t fixed_header_length(uint32_t rlength) const;

    //! Write the fixed header to a buffer
    /*!
      \param buf Pointer to start of buffer (never advances)
      \param bufpos Current position in buffer
      \param rlength Remaining lengh i.e variable header + payload
    */
    void write_fixed_header(uint8_t *buf, uint32_t& bufpos, uint32_t rlength) const;

    //! Does this message need a packet id before being sent?
    bool need_packet_id(void) const { return _need_packet_id; }

    //! Set the packet id
    void set_packet_id(uint16_t pid) { _packet_id = pid; }

    //! Write the packet id to a buffer
    /*!
      \param buf Pointer to start of buffer (never advances)
      \param bufpos Current position in buffer
    */
    void write_packet_id(uint8_t *buf, uint32_t& bufpos) const;

    //! Length of variable header
    virtual uint32_t variable_header_length(void) const { return 0; }

    //! Write variable header
    /*!
      \param buf Pointer to start of buffer (never advances)
      \param bufpos Current position in buffer
    */
    virtual void write_variable_header(uint8_t *buf, uint32_t& bufpos) const { }

    //! Length of payload
    virtual uint32_t payload_length(void) const { return 0; }

    //! Write payload
    /*!
      \param buf Pointer to start of buffer (never advances)
      \param bufpos Current position in buffer
    */
    virtual void write_payload(uint8_t *buf, uint32_t& bufpos) const { }

    //! Message type to expect in response to this message
    virtual message_type response_type(void) const { return None; }

    friend PubSubClient; // Just to allow it to call response_type()

  public:
    //! Send the message out
    bool send(Client& client);

    //! Get the message type
    message_type type(void) const { return _type; }

    //! Get the packet id
    uint16_t packet_id(void) const { return _packet_id; }

    //! Does this message have a network stream for reading the (large) payload?
    bool has_stream(void) const { return _stream_client != nullptr; }

  };
}
