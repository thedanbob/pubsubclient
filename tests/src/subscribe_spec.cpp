#include "PubSubClient.h"
#include "ShimClient.h"
#include "Buffer.h"
#include "BDDTest.h"
#include "trace.h"


IPAddress server(172, 16, 0, 2);
byte connack[] = {0x20,0x02,0x00,0x00};

void callback(const MQTT::Publish& pub) {
  // handle message arrived
}



int test_subscribe_no_qos() {
  IT("subscribe without qos defaults to 0");
  ShimClient shimClient;
  shimClient.setAllowConnect(true);
  shimClient.respond(connack, sizeof(connack));

  PubSubClient client(shimClient,server, 1883);
  client.set_callback(callback);
  int rc = client.connect("client_test1");
  IS_TRUE(rc);

  byte subscribe[] = { 0x82,0xa,0x0,0x2,0x0,0x5,0x74,0x6f,0x70,0x69,0x63,0x0 };
  shimClient.expect(subscribe, sizeof(subscribe));
  byte suback[] = { 0x90,0x3,0x0,0x2,0x0 };
  shimClient.respond(suback, sizeof(suback));

  rc = client.subscribe((char*)"topic");
  IS_TRUE(rc);

  IS_FALSE(shimClient.error());

  END_IT
}

int test_subscribe_qos_1() {
  IT("subscribes qos 1");
  ShimClient shimClient;
  shimClient.setAllowConnect(true);
  shimClient.respond(connack, sizeof(connack));

  PubSubClient client(shimClient,server, 1883);
  client.set_callback(callback);
  int rc = client.connect("client_test1");
  IS_TRUE(rc);

  byte subscribe[] = { 0x82,0xa,0x0,0x2,0x0,0x5,0x74,0x6f,0x70,0x69,0x63,0x1 };
  shimClient.expect(subscribe, sizeof(subscribe));
  byte suback[] = { 0x90,0x3,0x0,0x2,0x1 };
  shimClient.respond(suback, sizeof(suback));

  rc = client.subscribe((char*)"topic",1);
  IS_TRUE(rc);

  IS_FALSE(shimClient.error());

  END_IT
}

int test_subscribe_not_connected() {
  IT("subscribe fails when not connected");
  ShimClient shimClient;

  PubSubClient client(shimClient,server, 1883);
  client.set_callback(callback);

  int rc = client.subscribe((char*)"topic");
  IS_FALSE(rc);

  IS_FALSE(shimClient.error());

  END_IT
}

int test_subscribe_invalid_qos() {
  IT("subscribe fails when not connected");
  ShimClient shimClient;
  shimClient.setAllowConnect(true);
  shimClient.respond(connack, sizeof(connack));

  PubSubClient client(shimClient,server, 1883);
  client.set_callback(callback);
  int rc = client.connect("client_test1");
  IS_TRUE(rc);

  rc = client.subscribe((char*)"topic",2);
  IS_FALSE(rc);
  rc = client.subscribe((char*)"topic",254);
  IS_FALSE(rc);

  IS_FALSE(shimClient.error());

  END_IT
}

int test_unsubscribe() {
  IT("unsubscribes");
  ShimClient shimClient;
  shimClient.setAllowConnect(true);
  shimClient.respond(connack, sizeof(connack));

  PubSubClient client(shimClient,server, 1883);
  client.set_callback(callback);
  int rc = client.connect("client_test1");
  IS_TRUE(rc);

  byte unsubscribe[] = { 0xA2,0x9,0x0,0x2,0x0,0x5,0x74,0x6f,0x70,0x69,0x63 };
  shimClient.expect(unsubscribe, sizeof(unsubscribe));
  byte unsuback[] = { 0xB0,0x2,0x0,0x2 };
  shimClient.respond(unsuback, sizeof(unsuback));

  rc = client.unsubscribe((char*)"topic");
  IS_TRUE(rc);

  IS_FALSE(shimClient.error());

  END_IT
}

int test_unsubscribe_not_connected() {
  IT("unsubscribe fails when not connected");
  ShimClient shimClient;

  PubSubClient client(shimClient,server, 1883);
  client.set_callback(callback);

  int rc = client.unsubscribe((char*)"topic");
  IS_FALSE(rc);

  IS_FALSE(shimClient.error());

  END_IT
}

int main() {
  test_subscribe_no_qos();
  test_subscribe_qos_1();
  test_subscribe_not_connected();
  test_subscribe_invalid_qos();
  test_unsubscribe();
  test_unsubscribe_not_connected();
  FINISH
}
