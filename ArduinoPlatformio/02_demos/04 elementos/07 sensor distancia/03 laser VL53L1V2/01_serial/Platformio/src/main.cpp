/*
  Autor: Ramón Junquera
  Tema: Sensor de distancia VL53L1X
  Fecha: 20210730
  Objetivo: Aprender el funcionamiento del sensor de distancia LIDAR.
  Notas:
  - Suponemos que utilizamos un sensor con el identificador I2C por defecto
  Resultado:
  - Se muestra la distancia medida en el puerto serie.
  - Podemos comprobar que en vez de llamar al método timeoutOcurred, podemos simplificarlo a:
    Si devuelve una medida de valor 0 es que ha ocurrido un error.
*/

#include <Arduino.h>
#include <RoJoVL53L1X.h> //Librería de gestión del sensor LIDAR

//Declaración de variables globales
RoJoVL53L1X sensor; //Objeto de gestión del sensor LIDAR

void setup() {
  Serial.begin(115200);

  //Si no podemos inicializar el sensor con los pines por defecto...
  if (!sensor.begin()) {
    //DEBUG. Error porque el sensor está en el identificador 0x
    Serial.println("Error inicializando el sensor"); //Informamos
    while(1); //Paramos
  }
}

void loop() {
  Serial.print(sensor.read()); //Mostramos la distancia leida
  if(sensor.timeoutOccurred()) Serial.print(" timeout"); //Indicamos si se ha producido un error
  Serial.println();
  delay(400); //Esperamos un poco para facilitar la lectura de los datos
}
