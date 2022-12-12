#pragma once

#include <Arduino.h>
#include <Client.h>

namespace MQTT {
  //! Write a 16-bit value, big-endian order
  void write(uint8_t *buf, uint32_t& bufpos, uint16_t data);

  //! Write an arbitrary chunk of data, with 16-bit length first
  void write(uint8_t *buf, uint32_t& bufpos, uint8_t *data, uint16_t dlen);

  //! Write a string, with 16-bit length first
  void write(uint8_t *buf, uint32_t& bufpos, String str);

  void write_bare_payload(uint8_t *buf, uint32_t& bufpos, uint8_t *data, uint32_t dlen);

  //! Read a byte from a buffer
  uint8_t read_uint8(uint8_t *buf, uint32_t& pos);

  //! Read 16 bits from a buffer
  uint16_t read_uint16(uint8_t *buf, uint32_t& pos);

  //! Read a string from a buffer
  String read_string(uint8_t *buf, uint32_t& pos);

  //! Read a byte from a Client object
  uint8_t read_uint8(Client& client);

  //! Read 16 bits from a Client object
  uint16_t read_uint16(Client& client);

  //! Read a string from a Client object
  String read_string(Client& client);
}
