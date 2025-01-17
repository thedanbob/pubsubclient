#include <unistd.h>
#include "PubSubClient.h"
#include "ShimClient.h"
#include "Buffer.h"
#include "BDDTest.h"
#include "trace.h"


IPAddress server(172, 16, 0, 2);
byte connack[] = { 0x20, 0x02, 0x00, 0x00 };
byte pingreq[] = { 0xc0, 0x0 };
byte pingresp[] = { 0xd0, 0x0 };

void callback(const MQTT::Publish& pub) {
  // handle message arrived
}


int test_keepalive_pings_idle() {
  IT("keeps an idle connection alive");

  ShimClient shimClient;
  shimClient.setAllowConnect(true);
  shimClient.respond(connack, sizeof(connack));

  PubSubClient client(shimClient,server, 1883);
  client.set_callback(callback);
  int rc = client.connect("client_test1");
  IS_TRUE(rc);

  for (int i = 0; i < 50; i++) {
    if ( i == 15 || i == 31 || i == 47) {
      shimClient.expect(pingreq, sizeof(pingreq));
      shimClient.respond(pingresp, sizeof(pingresp));
    }
    sleep(1);
    rc = client.loop();
    IS_TRUE(rc);
  }

  IS_FALSE(shimClient.error());

  END_IT
}

int test_keepalive_pings_with_outbound_qos0() {
  IT("keeps a connection alive that only sends qos0");

  ShimClient shimClient;
  shimClient.setAllowConnect(true);
  shimClient.respond(connack, sizeof(connack));

  PubSubClient client(shimClient,server, 1883);
  client.set_callback(callback);
  int rc = client.connect("client_test1");
  IS_TRUE(rc);

  byte publish[] = {0x30,0xe,0x0,0x5,0x74,0x6f,0x70,0x69,0x63,0x70,0x61,0x79,0x6c,0x6f,0x61,0x64};

  for (int i = 0; i < 50; i++) {
    TRACE(i<<":");
    shimClient.expect(publish, sizeof(publish));
    rc = client.publish((char*)"topic",(char*)"payload");
    IS_TRUE(rc);
    IS_FALSE(shimClient.error());
    sleep(1);
    if ( i == 15 || i == 31 || i == 47) {
      shimClient.expect(pingreq, sizeof(pingreq));
      shimClient.respond(pingresp, sizeof(pingresp));
    }
    rc = client.loop();
    IS_TRUE(rc);
    IS_FALSE(shimClient.error());
  }

  END_IT
}

int test_keepalive_pings_with_inbound_qos0() {
  IT("keeps a connection alive that only receives qos0");

  ShimClient shimClient;
  shimClient.setAllowConnect(true);
  shimClient.respond(connack, sizeof(connack));

  PubSubClient client(shimClient,server, 1883);
  client.set_callback(callback);
  int rc = client.connect("client_test1");
  IS_TRUE(rc);

  byte publish[] = {0x30,0xe,0x0,0x5,0x74,0x6f,0x70,0x69,0x63,0x70,0x61,0x79,0x6c,0x6f,0x61,0x64};

  for (int i = 0; i < 50; i++) {
    TRACE(i<<":");
    sleep(1);
    if ( i == 15 || i == 31 || i == 47) {
      shimClient.expect(pingreq, sizeof(pingreq));
      shimClient.respond(pingresp, sizeof(pingresp));
    }
    shimClient.respond(publish, sizeof(publish));
    rc = client.loop();
    IS_TRUE(rc);
    IS_FALSE(shimClient.error());
  }

  END_IT
}

int test_keepalive_no_pings_inbound_qos1() {
  IT("does not send pings for connections with inbound qos1");

  ShimClient shimClient;
  shimClient.setAllowConnect(true);

  byte connack[] = { 0x20, 0x02, 0x00, 0x00 };
  shimClient.respond(connack, sizeof(connack));

  PubSubClient client(shimClient,server, 1883);
  client.set_callback(callback);
  int rc = client.connect("client_test1");
  IS_TRUE(rc);

  byte publish[] = {0x32,0x10,0x0,0x5,0x74,0x6f,0x70,0x69,0x63,0x12,0x34,0x70,0x61,0x79,0x6c,0x6f,0x61,0x64};
  byte puback[] = {0x40,0x2,0x12,0x34};

  for (int i = 0; i < 50; i++) {
    shimClient.respond(publish, sizeof(publish));
    shimClient.expect(puback, sizeof(puback));
    sleep(1);
    rc = client.loop();
    IS_TRUE(rc);
    IS_FALSE(shimClient.error());
  }

  END_IT
}

int test_keepalive_disconnects_hung() {
  IT("disconnects a hung connection");

  ShimClient shimClient;
  shimClient.setAllowConnect(true);

  shimClient.respond(connack, sizeof(connack));

  PubSubClient client(shimClient,server, 1883);
  client.set_callback(callback);
  int rc = client.connect("client_test1");
  IS_TRUE(rc);

  shimClient.expect(pingreq, sizeof(pingreq));

  for (int i = 0; i < 32; i++) {
    sleep(1);
    rc = client.loop();
  }
  IS_FALSE(rc);

  IS_FALSE(shimClient.error());

  END_IT
}

int main() {
  test_keepalive_pings_idle();
  test_keepalive_pings_with_outbound_qos0();
  test_keepalive_pings_with_inbound_qos0();
  test_keepalive_no_pings_inbound_qos1();
  test_keepalive_disconnects_hung();

  FINISH
}
