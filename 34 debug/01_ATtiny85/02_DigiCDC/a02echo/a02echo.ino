/*
  Autor: Ramón Junquera
  Fecha: 20191027
  Tema: Sistemas de debug en ATtiny85. Librería DigiCDC.h
  Objetivo: Comunicación bidireccional
  Material adicional: ATtiny85, breadboard

  Descripción:
  Puesto que la librería DigiCDC nos permite una comunicación bidireccional, aprenderemos
  a recibir información.

  El ejemplo hace de eco.
  Toda la información que reciba por el puerto serie USB, será devuelta y reenviada.

  Puesto que el buffer de entrada el puerto serie USB es de 48 bytes, podemos comprobar que
  si enviamos un texto de 49 caracteres, sólo se mostrarán los 48 primeros.
  Si enviamos un texto de 60 caracteres, se mostrarán correctamente los 48 primeros, pero
  de los siguientes sólo se mostrarán aquellos que le haya dado tiempo a recoger mientras
  se procesaban los recibidos. Pero seguro que no se habrán recibidos todos.

  Resultado:
  Vemos en el monitor serie como aparece el mismo texto que enviamos
*/

#include <Arduino.h>
#include <DigiCDC.h> //Gestión de puerto serie

//Definición de variables globales
unsigned int counter=0;

void setup() {                
  //Inicializamos el puerto serie USB. No se indica velocidad!
  SerialUSB.begin(); 
}

void loop() {
  //Si tenemos algún carácter pendiente de ser recibido...
  if (SerialUSB.available()) {
    //...lo leemos y a continuación lo enviamos por el mismo puerto USB
    SerialUSB.write(SerialUSB.read());
  }
}
