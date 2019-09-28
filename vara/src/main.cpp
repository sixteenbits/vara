#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

// WiFi
const char* ssid = "Klinaap";
const char* password = "klinton10";
WiFiClient esp_client;
// MQTT
const char* rpi_broker = "192.168.43.215";
PubSubClient client(esp_client);
const char* topic = "jamtoday/vara";

long lastMsg = 0;
char msg[50];
//String msg;
int value = 0;
//int cmd;

void setup_wifi() {
 
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Conectando a  ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.print("\nWiFi conectado\nIP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(topic, "Enviando el primer mensaje");
      // ... and resubscribe
      //client.subscribe("casa/despacho/luz");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void command(int cmd){
  switch (cmd)
  {
  case 1:
    client.publish(topic, "l");
    Serial.println("Izquierda");
    break;
  case 2:
    client.publish(topic, "r");
    Serial.println("Derecha");
    break;
  case 3:
    client.publish(topic, "v");
    Serial.println("Varazo");
    break;
  default:
    Serial.println("Comando inválido");
    break;
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  setup_wifi();
  client.setServer(rpi_broker, 1883);
  //client.setCallback(callback);
}


void loop() {
 
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
 
  long now = millis();
  if (now - lastMsg > 2000) {
    command(1); delay(1000);
    command(2); delay(1000);
    command(3); delay(5000);
  }
}
