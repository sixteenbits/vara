#include <Arduino.h>
#include <ESP8266WiFi.h>    // WiFi
#include <PubSubClient.h>   // MQTT
#include "MPU9255.h"        // IMU - include MPU9255 library

// WiFi ----------------------------------------------------------------
const char* ssid = "Klinaap";
const char* password = "klinton10";
WiFiClient esp_client;

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
// MQTT ----------------------------------------------------------------
const char* rpi_broker = "192.168.43.215";
PubSubClient client(esp_client);
const char* topic = "jamtoday/vara";
long lastMsg = 0;
char msg[50];
//String msg;
int value = 0;
//int cmd;

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


// IMU ----------------------------------------------------------------
/*
Procesa la aceleración del sensor MPU9255 a diferentes escalas
Modificación del ejemplo dataProcessing.ino de la libreria 
https://github.com/Bill2462/MPU9255-Arduino-Library

Equipo: SIXTEEN BITS
JAMTODAY ALMERIA 2019

*/
#define g 9.81 // 1g ~ 9.81 m/s^2
MPU9255 mpu;

double ax, ay, az;
int cx, cy, cz, vara;
//process raw acceleration data
//input = raw reading from the sensor, sensor_scale = selected sensor scale
//returns : acceleration in m/s^2

double process_acceleration(int input, scales sensor_scale ){
  /*
  To get acceleration in 'g', each reading has to be divided by :
   -> 16384 for +- 2g scale (default scale)
   -> 8192  for +- 4g scale
   -> 4096  for +- 8g scale
   -> 2048  for +- 16g scale
  */
  double output = 1;
  //for +- 2g
  if(sensor_scale == scale_2g){
    output = input;
    output = output/16384;
    output = output*g;
  }
  //for +- 4g
  if(sensor_scale == scale_4g){
    output = input;
    output = output/8192;
    output = output*g;
  }
  //for +- 8g
  if(sensor_scale == scale_8g){
    output = input;
    output = output/4096;
    output = output*g;
  }
  //for +-16g
  if(sensor_scale == scale_16g){
    output = input;
    output = output/2048;
    output = output*g;
  }
  return output;
}

void imu_values(){
  // Leer aceleracion
  mpu.read_acc();
  // Aceleracion
  ax = process_acceleration(mpu.ax,scale_16g);
  ay = process_acceleration(mpu.ay,scale_16g);
  az = process_acceleration(mpu.az,scale_16g);
  // Valores cuadraticos
  cx = process_acceleration(mpu.ax,scale_16g);
  cy = process_acceleration(mpu.ay,scale_16g);
  cz = process_acceleration(mpu.az,scale_16g);
  vara = sqrt(pow(cx, 2) + pow(cy, 2) + pow(cz, 2));
}

void imu_print(){
  //X Y Z ejes
  Serial.println("AX: " + String(ax) + "  AY: " + String(ay) + "  AZ: " + String(az) + "  Vara: " + String(vara));
}


void imu_calculate(){
  if(ax != -0.00){
    if(ax > 50.00){ // Izquierda
      command(1);
    } else if (ax < -50.00) {
      command(2);   // Derecha
    } else if(vara > 90.00){
      command(3);
    }
  }
}

void imu_loop(){
  // Leer valores
  imu_values();
  // Imprimir valores
  //imu_print();
  // MQTT Valores
  imu_calculate();
}

void setup() {
  // Serial
  Serial.begin(115200);
  // WiFi
  setup_wifi();
  // MQTT
  client.setServer(rpi_broker, 1883);
  //client.setCallback(callback);
  // IMU
  if(mpu.init()){
    Serial.println("IMU OK");
  } else {
    Serial.println("Problemas con la IMU");
  }
}


void loop() {
  // MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  // IMU
  imu_loop();
  /* 
  long now = millis();
  if (now - lastMsg > 2000) {
    //command(1); delay(1000);
    //command(2); delay(1000);
    //command(3); delay(5000);
  }
  */
  delay(250);
}

