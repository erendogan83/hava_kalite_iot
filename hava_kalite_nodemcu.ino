#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "";  // wi-fi ağı adı
const char* password = ""; // wi-fi ağı şifresi
const char* mqtt_server = ""; // raspberry pi 4 ip si

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {

  delay(100);
  Serial.println();
  Serial.print("Baglaniyor ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi baglandi");
  Serial.println("IP addresi: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {

}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Baglaniyor MQTT ....");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("baglandi");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  char gelen_veri[255];

  memset(gelen_veri, 0, 255);

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  while (Serial.available()) {
    Serial.readBytes(gelen_veri, 310); //Read the serial data and store in var
    Serial.println(gelen_veri);
    client.publish("/hava_kalite", gelen_veri);
  }
}
