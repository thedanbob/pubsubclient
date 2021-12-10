#include "PubSubClient.h"
#include "ShimClient.h"
#include "Buffer.h"
#include "Stream.h"
#include "BDDTest.h"
#include "trace.h"

#define MQTT_MAX_PACKET_SIZE 4096

IPAddress server(172, 16, 0, 2);
byte connack[] = {0x20,0x02,0x00,0x00};

bool callback_called = false;
String lastTopic;
char lastPayload[MQTT_MAX_PACKET_SIZE];
unsigned int lastLength;

void reset_callback() {
  callback_called = false;
  lastTopic[0] = '\0';
  lastPayload[0] = '\0';
  lastLength = 0;
}

void callback(const MQTT::Publish& pub) {
  callback_called = true;
  lastTopic = pub.topic();
  memcpy(lastPayload, pub.payload(), pub.payload_len());
  lastLength = pub.payload_len();
}

uint8_t add_remaining_length(uint8_t* packet, uint8_t pos, uint32_t remaining_length) {
  do {
    uint8_t digit = remaining_length & 0x7f; // x % 128
    remaining_length >>= 7; // x /= 128
    if (remaining_length)
      digit |= 0x80; // |= 128 (set continuation bit)
    packet[pos++] = digit;
  } while (remaining_length);

  return pos;
}

int test_receive_callback() {
  IT("receives a callback message");

  ShimClient shimClient;
  shimClient.setAllowConnect(true);
  shimClient.respond(connack, sizeof(connack));

  PubSubClient client(shimClient,server, 1883);
  client.set_callback(callback);
  int rc = client.connect("client_test1");
  IS_TRUE(rc);

  byte publish[] = {0x30,0xe,0x0,0x5,0x74,0x6f,0x70,0x69,0x63,0x70,0x61,0x79,0x6c,0x6f,0x61,0x64};
  shimClient.respond(publish, sizeof(publish));
  reset_callback();

  rc = client.loop();

  IS_TRUE(rc);

  IS_TRUE(callback_called);
  IS_TRUE(lastTopic == "topic");
  IS_TRUE(memcmp(lastPayload,"payload",7)==0);
  IS_TRUE(lastLength == 7);

  IS_FALSE(shimClient.error());

  END_IT
}

int test_receive_max_sized_message() {
  IT("receives an max-sized message");

  ShimClient shimClient;
  shimClient.setAllowConnect(true);
  shimClient.respond(connack, sizeof(connack));

  PubSubClient client(shimClient,server, 1883);
  client.set_callback(callback);
  int rc = client.connect("client_test1");
  IS_TRUE(rc);

  uint32_t length = MQTT_MAX_PACKET_SIZE;
  byte publish[length];
  memset(publish, 'A', length);

  // Fixed header
  publish[0] = 0x30;
  uint8_t pos = add_remaining_length(publish, 1, length - 3);

  // Variable header
  byte topic[] = {0x0,0x5,0x74,0x6f,0x70,0x69,0x63};
  memcpy(publish + pos, topic, sizeof(topic));
  pos += sizeof(topic);

  shimClient.respond(publish, length);
  reset_callback();

  rc = client.loop();

  IS_TRUE(rc);

  IS_TRUE(callback_called);
  IS_TRUE(lastTopic == "topic");
  IS_TRUE(lastLength == length - pos);
  IS_TRUE(memcmp(lastPayload, publish + pos, length - pos)==0);

  IS_FALSE(shimClient.error());

  END_IT
}

int test_receive_qos1() {
  IT("receives a qos1 message");

  ShimClient shimClient;
  shimClient.setAllowConnect(true);
  shimClient.respond(connack, sizeof(connack));

  PubSubClient client(shimClient,server, 1883);
  client.set_callback(callback);
  int rc = client.connect("client_test1");
  IS_TRUE(rc);

  byte publish[] = {0x32,0x10,0x0,0x5,0x74,0x6f,0x70,0x69,0x63,0x12,0x34,0x70,0x61,0x79,0x6c,0x6f,0x61,0x64};
  shimClient.respond(publish, sizeof(publish));

  byte puback[] = {0x40,0x2,0x12,0x34};
  shimClient.expect(puback, sizeof(puback));
  reset_callback();

  rc = client.loop();

  IS_TRUE(rc);

  IS_TRUE(callback_called);
  IS_TRUE(lastTopic == "topic");
  IS_TRUE(memcmp(lastPayload,"payload",7)==0);
  IS_TRUE(lastLength == 7);

  IS_FALSE(shimClient.error());

  END_IT
}

// FIXME: these do not actually test streaming

// int test_receive_stream() {
//   IT("receives a streamed callback message");
//   reset_callback();

//   Stream stream;
//   stream.expect((uint8_t*)"payload",7);

//   ShimClient shimClient;
//   shimClient.setAllowConnect(true);
//   shimClient.respond(connack, sizeof(connack));

//   PubSubClient client(shimClient, server, 1883); // stream?
//   client.set_callback(callback);
//   int rc = client.connect("client_test1");
//   IS_TRUE(rc);

//   byte publish[] = {0x30,0xe,0x0,0x5,0x74,0x6f,0x70,0x69,0x63,0x70,0x61,0x79,0x6c,0x6f,0x61,0x64};
//   shimClient.respond(publish, sizeof(publish));

//   rc = client.loop();

//   IS_TRUE(rc);

//   IS_TRUE(callback_called);
//   IS_TRUE(lastTopic == "topic");
//   IS_TRUE(lastLength == 7);

//   IS_FALSE(stream.error());
//   IS_FALSE(shimClient.error());

//   END_IT
// }

// int test_receive_oversized_stream_message() {
//   IT("drops an oversized message");
//   reset_callback();

//   Stream stream;

//   ShimClient shimClient;
//   shimClient.setAllowConnect(true);
//   shimClient.respond(connack, sizeof(connack));

//   PubSubClient client(shimClient, server, 1883); // stream?
//   client.set_callback(callback);
//   int rc = client.connect("client_test1");
//   IS_TRUE(rc);

//   uint32_t length = MQTT_MAX_PACKET_SIZE+1;
//   uint8_t length_length = length_field_length(length - 2);
//   byte payload[] = {0x0,0x5,0x74,0x6f,0x70,0x69,0x63,0x70,0x61,0x79,0x6c,0x6f,0x61,0x64};
//   byte publish[1 + length_length + sizeof(payload)];
//   publish[0] = 0x30;
//   add_remaining_length(publish, 1, length - 2);
//   memcpy(publish + 1 + length_length, payload, sizeof(payload));

//   byte bigPublish[length];
//   memset(bigPublish,'A',length - 1);
//   bigPublish[length - 1] = 'B';
//   memcpy(bigPublish,publish,16);

//   shimClient.respond(bigPublish, length);
//   stream.expect(payload, sizeof(payload));

//   rc = client.loop();

//   IS_TRUE(rc);

//   IS_TRUE(callback_called);
//   IS_TRUE(lastTopic == "topic");
//   IS_TRUE(lastLength == length-9);

//   IS_FALSE(stream.error());
//   IS_FALSE(shimClient.error());

//   END_IT
// }

int main() {
  test_receive_callback();
  test_receive_max_sized_message();
  test_receive_qos1();
  // test_receive_stream();
  // test_receive_oversized_stream_message();

  FINISH
}
