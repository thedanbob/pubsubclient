#include "Message.h"
#include "MQTT.h"

namespace MQTT {
  uint8_t Message::fixed_header_length(uint32_t rlength) const {
    if (rlength < 128)
      return 2;
    else if (rlength < 16384)
      return 3;
    else if (rlength < 2097152)
      return 4;
    else
      return 5;
  }

  void Message::write_fixed_header(uint8_t *buf, uint32_t& bufpos, uint32_t rlength) const {
    buf[bufpos] = _type << 4;

    switch (_type) {
    case PUBLISH:
      buf[bufpos] |= _flags & 0x0f;
      break;
    case PUBREL:
    case SUBSCRIBE:
    case UNSUBSCRIBE:
      buf[bufpos] |= 0x02;
    default:
      break;
    }
    bufpos++;

    // Remaining length
    do {
      uint8_t digit = rlength & 0x7f;
      rlength >>= 7;
      if (rlength)
        digit |= 0x80;
      buf[bufpos++] = digit;
    } while (rlength);
  }

  void Message::write_packet_id(uint8_t *buf, uint32_t& bufpos) const {
    write(buf, bufpos, _packet_id);
  }

  bool Message::send(Client& client) {
    uint32_t variable_header_len = variable_header_length();
    uint32_t remaining_length = variable_header_len + payload_length();
    uint32_t packet_length = fixed_header_length(remaining_length);
    if (_payload_callback == nullptr)
      packet_length += remaining_length;
    else
      packet_length += variable_header_len;

    uint8_t *packet = new uint8_t[packet_length];

    uint32_t pos = 0;
    write_fixed_header(packet, pos, remaining_length);
    write_variable_header(packet, pos);

    write_payload(packet, pos);

    uint32_t sent = client.write(const_cast<const uint8_t*>(packet), packet_length);
    delete [] packet;
    if (sent != packet_length)
      return false;

    if (_payload_callback != nullptr)
      return _payload_callback(client);

    return true;
  }
}
