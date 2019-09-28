/*
Procesa la aceleración del sensor MPU9255 a diferentes escalas
Modificación del ejemplo dataProcessing.ino de la libreria 
https://github.com/Bill2462/MPU9255-Arduino-Library

Equipo: SIXTEEN BITS
JAMTODAY ALMERIA 2019

*/

#include <MPU9255.h>// include MPU9255 library

#define g 9.81 // 1g ~ 9.81 m/s^2

MPU9255 mpu;

//process raw acceleration data
//input = raw reading from the sensor, sensor_scale = selected sensor scale
//returns : acceleration in m/s^2
double process_acceleration(int input, scales sensor_scale )
{
  /*
  To get acceleration in 'g', each reading has to be divided by :
   -> 16384 for +- 2g scale (default scale)
   -> 8192  for +- 4g scale
   -> 4096  for +- 8g scale
   -> 2048  for +- 16g scale
  */
  double output = 1;

  //for +- 2g

  if(sensor_scale == scale_2g)
  {
    output = input;
    output = output/16384;
    output = output*g;
  }

  //for +- 4g
  if(sensor_scale == scale_4g)
  {
    output = input;
    output = output/8192;
    output = output*g;
  }

  //for +- 8g
  if(sensor_scale == scale_8g)
  {
    output = input;
    output = output/4096;
    output = output*g;
  }

  //for +-16g
  if(sensor_scale == scale_16g)
  {
    output = input;
    output = output/2048;
    output = output*g;
  }

  return output;
}


void setup() {
  Serial.begin(115200);// initialize Serial port

  if(mpu.init())
  {
  Serial.println("initialization failed");
  }
  else
  {
  Serial.println("initialization successful!");
  }
  
}

void loop() {
  //take readings
  mpu.read_acc();
  

  ////process and print acceleration data////
  //X axis
  Serial.print("AX: ");
  Serial.print(process_acceleration(mpu.ax,scale_16g));

  //Y axis
  Serial.print("  AY: ");
  Serial.print(process_acceleration(mpu.ay,scale_16g));

  //Z axis
  Serial.print("  AZ: ");
  Serial.print(process_acceleration(mpu.az,scale_16g));

//Calcula el valor cuadratico de las 3 aceleraciones.
int x,y,z,vara;
  x=process_acceleration(mpu.ax,scale_2g);
  y=process_acceleration(mpu.ay,scale_2g);
  z=process_acceleration(mpu.az,scale_2g);
  vara=sqrt(pow(x,2)+pow(y,2)+pow(z,2));
  Serial.print("   Vara: ");
  Serial.println(vara);

  delay(100);
}
