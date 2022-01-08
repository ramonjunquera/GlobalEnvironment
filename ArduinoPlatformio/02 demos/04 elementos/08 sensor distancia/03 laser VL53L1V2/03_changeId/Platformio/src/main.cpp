/*
  Autor: Ramón Junquera
  Tema: Sensor de distancia VL53L1X
  Fecha: 20210730
  Objetivo: Cambio simple de identificador I2C
  Descripción:
  - En la sección de declaración de constantes globales definimos idI2C con el valor del
    identificador I2C que asignaremos al sensor.
  - Siempre que necesitemos cambiar el identificador I2C tendremos que cablear el pin XSHUT
    para hacer un reset.
  - Como desconocemos el identificador que pueda tener el principio, hacemos un hardreset para
    dejarlo con la configuración por defecto.
  Notas:
  - Cuando aplicamos un nuevo identificador I2C a un sensor, no tiene porqué mantenerlo
    después de perder la alimentación.
  Resultado:
  - EL identificador I2C cambia de valor indicado.
  - Mostramos lecturas continuas para comprobar que funciona después del cambio.
*/

#include <Arduino.h>
#include <RoJoVL53L1X.h> //Librería de gestión del sensor LIDAR

//Declaración de constantes globales
const byte idI2C=0x2A; //42=0x2A
//Pin de reset para los distintos dispositivos
#ifdef ESP32OLED16MB
  const byte pinRES=19;
#elif defined(ARDUINO_AVR_UNO)
  const byte pinRES=A3;
#else
  #error Dispositivo desconocido
#endif

//Declaración de variables globales
RoJoVL53L1X sensor; //Objeto de gestión del sensor LIDAR

void setup() {
  Serial.begin(115200);
  //Resetamos el sensor a los valores por defecto (idI2C=0x29=41) con independencia del
  //identificador I2C que pueda tener.
  sensor.hardReset(pinRES);
  //Si no podemos inicializar el sensor con los pines I2C estándar...
  if (!sensor.begin()) {
    Serial.println("Error inicializando el sensor"); //Informamos
    while(1); //Paramos
  }
  //El sensor tiene el identificador I2C por defecto (41=0x29)
  sensor.setAddress(idI2C); //Lo cambiamos por el deseado
  Serial.println("Identificador I2C asignado="+String(sensor.getAddress()));
}

void loop() { //Mostramos los valores leidos para comprobar que funciona
  Serial.println(sensor.read());
  delay(400);
}
