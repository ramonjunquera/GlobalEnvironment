/*
  Autor: Ramón Junquera
  Fecha: 20191025
  Tema: Sistemas de debug en ATtiny85
  Objetivo: Aprender a utilizar la librería DigiKeyboard.h
  Material adicional: ATtiny85, breadboard

  Descripción:
  Ejemplo simple de utilización de la librería DigiKeyboard.h
  Emulamos el comportamiento de un teclado.
  Envía pulsaciones al PC con mensajes cada segundo.
  
  Resultado:
  Vemos en el block de notas la información procedente de la placa
*/

#define kbd_es_es //Emulación de teclado en español
#include <DigiKeyboard.h> //Gestión de emulación de teclado

//Definición de variables globales
int counter=0;

void setup()
{
  //Es normal que el ordenador tarde 2 o 3 segundos en detectar el ATtiny85 como nuevo teclado
  //Si enviamos pulsaciones en ese tiempo, se perderán.
  //Por eso ponemos un retardo inicial.
  //Es conveniente utilizar la función de delay de DigiKeyboard en vez del delay clásico
  //porque se asegura de mantener la comunicación con el ordenador al que está conectado
  DigiKeyboard.delay(3000);
  //Por seguridad, enviamos como inicialización un carácter nulo para que se vacíe el buffer
  //Esto impide que se pueda perder el primero de los caracteres que enviaremos
  DigiKeyboard.sendKeyStroke(0);
  //Enviamos el primer mensaje
  DigiKeyboard.println("El programa ha comenzado");
}

void loop()
{
  //Aumentamos el valor del contador
  counter++;
  //Mostramos el valor del contador para debug
  DigiKeyboard.print(millis());
  DigiKeyboard.print(" counter=");
  DigiKeyboard.println(counter);
  //Esperamos un segundo
  DigiKeyboard.delay(1000);
}
