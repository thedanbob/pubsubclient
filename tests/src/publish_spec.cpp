#include "PubSubClient.h"
#include "ShimClient.h"
#include "Buffer.h"
#include "BDDTest.h"
#include "trace.h"


IPAddress server(172, 16, 0, 2);
byte connack[] = { 0x20, 0x02, 0x00, 0x00 };

void callback(const MQTT::Publish& pub) {
  // handle message arrived
}

int test_publish() {
  IT("publishes a null-terminated string");
  ShimClient shimClient;
  shimClient.setAllowConnect(true);

  shimClient.respond(connack, sizeof(connack));

  PubSubClient client(shimClient, server, 1883);
  client.set_callback(callback);
  int rc = client.connect("client_test1");
  IS_TRUE(rc);

  byte publish[] = {0x30,0xe,0x0,0x5,0x74,0x6f,0x70,0x69,0x63,0x70,0x61,0x79,0x6c,0x6f,0x61,0x64};
  shimClient.expect(publish, sizeof(publish));

  rc = client.publish((char*)"topic",(char*)"payload");
  IS_TRUE(rc);

  IS_FALSE(shimClient.error());

  END_IT
}

int test_publish_bytes() {
  IT("publishes a byte array");
  ShimClient shimClient;
  shimClient.setAllowConnect(true);
  shimClient.respond(connack, sizeof(connack));

  PubSubClient client(shimClient, server, 1883);
  client.set_callback(callback);
  int rc = client.connect("client_test1");
  IS_TRUE(rc);

  byte publish[] = {0x30,0xc,0x0,0x5,0x74,0x6f,0x70,0x69,0x63,0x1,0x2,0x3,0x0,0x5};
  shimClient.expect(publish, sizeof(publish));

  byte payload[] = {0x01,0x02,0x03,0x0,0x05};
  rc = client.publish((char*)"topic", payload, sizeof(payload));
  IS_TRUE(rc);

  IS_FALSE(shimClient.error());

  END_IT
}

int test_publish_retained() {
  IT("publishes retained");
  ShimClient shimClient;
  shimClient.setAllowConnect(true);
  shimClient.respond(connack,4);

  PubSubClient client(shimClient, server, 1883);
  client.set_callback(callback);
  int rc = client.connect("client_test1");
  IS_TRUE(rc);

  byte publish[] = {0x31,0xc,0x0,0x5,0x74,0x6f,0x70,0x69,0x63,0x1,0x2,0x3,0x0,0x5};
  shimClient.expect(publish, sizeof(publish));

  byte payload[] = {0x01,0x02,0x03,0x0,0x05};
  rc = client.publish((char*)"topic", payload, sizeof(payload), true);
  IS_TRUE(rc);

  IS_FALSE(shimClient.error());

  END_IT
}

int test_publish_not_connected() {
  IT("publish fails when not connected");
  ShimClient shimClient;

  PubSubClient client(shimClient, server, 1883);

  int rc = client.publish((char*)"topic",(char*)"payload");
  IS_FALSE(rc);

  IS_FALSE(shimClient.error());

  END_IT
}


int test_publish_P() {
  IT("publishes using PROGMEM");
  ShimClient shimClient;
  shimClient.setAllowConnect(true);
  shimClient.respond(connack, sizeof(connack));

  PubSubClient client(shimClient, server, 1883);
  int rc = client.connect("client_test1");
  IS_TRUE(rc);

  byte publish[] = {0x31,0xc,0x0,0x5,0x74,0x6f,0x70,0x69,0x63,0x1,0x2,0x3,0x0,0x5};
  shimClient.expect(publish, sizeof(publish));

  char payload[] = {0x01,0x02,0x03,0x0,0x05};
  rc = client.publish_P((char*)"topic", payload, sizeof(payload), true);
  IS_TRUE(rc);

  IS_FALSE(shimClient.error());

  END_IT
}


int main() {
  test_publish();
  test_publish_bytes();
  test_publish_retained();
  test_publish_not_connected();
  test_publish_P();

  FINISH
}
