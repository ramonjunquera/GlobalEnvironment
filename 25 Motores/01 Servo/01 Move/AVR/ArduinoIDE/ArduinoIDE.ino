/*
  Autor: Ramón Junquera
  Tema: Servomotores
  Fecha: 20180829
  Objetivo: Demostración uso de la clase Servo.h
  Material: servomotor SG90 9g, placa Arduino

  Descripción:
  Ejemplo simple de control de un servomotor.
  Fijamos los valores límite del servo y utilizamos el método
  write para fijar el ángulo.
  Las constantes de los límites pueden variar dependiendo del servomotor.
  Incluso servomotores del mismo modelo podrían tener parámetros
  distintos. Se debe ajustar para cada caso.
  
  Resultado:
  El servomotor gira distintos ángulos
*/

#include <Arduino.h>
#include <Servo.h> //Librería de gestión de sermotores

//Creamos el objeto de gestión del servomotor
Servo servo;

void setup()
{
  //Indicamos que su señal estará conectado al pin 9
  //Puesto que los valores por defecto no eran correctos para este modelo, se redefinen.
  servo.attach(9,666,2400);
}

void loop()
{
    servo.write(0);
    delay(1500);
    servo.write(45);
    delay(500);
    servo.write(90);
    delay(500);
    servo.write(180);
    delay(1000);
}
