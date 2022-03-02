/*
  Autor: Ramón Junquera
  Tema: Sensor de distancia VL53L1X
  Fecha: 20220226
  Objetivo: Uso de varios sensores conectados al mismo bus I2C
  Descripción:
  - Utilizaremos varios sensores simultáneamente conectados al mismo bus I2C.
  - Es necesario que cada sensor tenga su propio identificador I2C único.
  - Como tenemos que cambiar los identificadores, es necesario cablear el pin de reset (XSHUT).
  - Utilizamos el método multiBegin. Es estático y podemos llamarlo directamente desde la clase.
  - Para comprobar que funciona, enviamos al puerto serie las lecturas de los 2 sensores
  Resultado:
  - Podemos utilizar más de un sensor simultáneamente porque todos tienen identificadores distintos.
*/

#include <Arduino.h>
#include <RoJoVL53L1X.h>

//Declaración de constantes globales
const byte idI2C[]={42,41}; //Identificadores I2C. No puede haber 2 iguales!!. Recomendable que el último sea el 0x29=41
//Pin de reset para los distintos dispositivos
#ifdef ESP32OLED16MB
  const byte pinReset[]={23,19}; //Pines de reset
#elif defined(ARDUINO_AVR_UNO)
  const byte pinReset[]={A3,A2}; //Pines de reset
#elif __arm__ //RPi
  const byte pinReset[]={18,23}; //Pines de reset
#else
  #error Dispositivo desconocido
#endif

//Declaración de variables globales
RoJoVL53L1X sensor[2]; //Objetos de gestión de los sensores LIDAR

void setup() {
  Serial.begin(115200);
  byte errorCode=RoJoVL53L1X::multiBegin(2,idI2C,pinReset,sensor);
  if(errorCode) { //Si tenemos algún error...
    Serial.println("Código de error de inicialización: "+String(errorCode));
    while(1); //Paramos
  }
}

void loop() {
  for(byte i=0;i<2;i++) { //Recorremos todos los sensores
    Serial.print(sensor[i].read());
    Serial.print(" ");
  }
  Serial.println();
  delay(400); //Esperamos un poco para facilitar la lectura de los datos
}
