#include "MQTT/utils.h"

namespace MQTT {
  void write(uint8_t *buf, uint32_t& bufpos, uint16_t data) {
    buf[bufpos++] = data >> 8;
    buf[bufpos++] = data & 0xff;
  }

  void write(uint8_t *buf, uint32_t& bufpos, uint8_t *data, uint16_t dlen) {
    write(buf, bufpos, dlen);
    memcpy(buf + bufpos, data, dlen);
    bufpos += dlen;
  }

  void write(uint8_t *buf, uint32_t& bufpos, String str) {
    const char* c = str.c_str();
    uint32_t length_pos = bufpos;
    bufpos += 2;
    uint16_t count = 0;
    while (*c) {
      buf[bufpos++] = *c++;
      count++;
    }
    write(buf, length_pos, count);
  }

  void write_bare_payload(uint8_t *buf, uint32_t& bufpos, uint8_t *data, uint32_t dlen) {
    memcpy(buf + bufpos, data, dlen);
    bufpos += dlen;
  }

  uint8_t read_uint8(uint8_t *buf, uint32_t& pos) {
    return buf[pos++];
  }

  uint16_t read_uint16(uint8_t *buf, uint32_t& pos) {
    uint16_t val = buf[pos++] << 8;
    val |= buf[pos++];
    return val;
  }

  String read_string(uint8_t *buf, uint32_t& pos) {
    uint16_t len = read_uint16(buf, pos);
    String val;
    val.reserve(len);
    for (uint16_t i = 0; i < len; i++) {
      val += (char)read_uint8(buf, pos);
    }

    return val;
  }

  uint8_t read_uint8(Client& client) {
    while(!client.available()) {}
    return client.read();
  }

  uint16_t read_uint16(Client& client) {
    uint16_t val = read_uint8(client) << 8;
    val |= read_uint8(client);
    return val;
  }

  String read_string(Client& client) {
    uint16_t len = read_uint16(client);
    String val;
    val.reserve(len);
    for (uint16_t i = 0; i < len; i++)
      val += (char)read_uint8(client);

    return val;
  }
}
