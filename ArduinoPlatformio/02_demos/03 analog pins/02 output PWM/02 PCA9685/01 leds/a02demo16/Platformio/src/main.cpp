
/*
  Tema: PCA9685 gestor de PWM
  Fecha: 20220216
  Autor: Ramón Junquera
  Objetivo: Efecto Kitt con todos los canales de un módulo

  Descripción:
    Es este ejemplo utilizaremos todos los canales de un módulo para hacer el efecto Kitt.
    
  Resultado:
    Comprobamos que podemos controlar de manera independiente todos 16 canales
*/

#include <Arduino.h>
#include <RoJoPCA9685.h>

RoJoPCA9685 pwm;

int i[16]; //Array con la intensidad de cada canal (led)

//Traslada los valores de intensidad actuales a los leds
//y reduce la intensidad en una cantidad fija para la próxima vez
void ApplyAndFade() {
  //Recorremos todas las intensidades y las copiamos al chip
  for(byte c=0;c<16;c++) pwm.setPWM(c,i[c]);
  for(byte c=0;c<16;c++) { //Recorremos todas las intensidades...
    i[c]-=50; //...y las reducimos en una cantidad fija
    if(i[c]<0) i[c]=0; //Si hemos reducido demasiado, como mínimo lo dejamos en 0
  }
}

void setup() {
  pwm.begin();
  for(byte c=0;c<16;c++) i[c]=0; //Inicialmente la intensidad de todos los leds es cero (apagados)
}
 
void loop() {
  for(byte c=0;c<16;c++) { //Recorreremos todos los leds en dirección ascendente
    i[c]=4095; //La intensidad del led en curso será máxima
    //Repetiremos un bucle que sólo baja la intensidad de todos los leds en cada ciclo
    for(int z=0;z<8;z++) {
      ApplyAndFade(); //Aplicamos la configuración actual
      delay(10); //Esperamos un momento
    }
  }
  for(byte c=14;c>0;c--) { //Recorreremos todos los leds en dirección descencente
    i[c]=4095; //La intensidad del led en curso será máxima
    //Repetiremos un bucle que sólo baja la intensidad de todos los leds en cada ciclo
    for(int z=0;z<8;z++) {
      ApplyAndFade(); //Aplicamos la configuración actual
      delay(10); //Esperamos un momento
    }
  }
}
