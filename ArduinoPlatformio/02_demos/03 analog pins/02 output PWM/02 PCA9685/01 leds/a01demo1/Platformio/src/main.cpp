/*
  Tema: PCA9685 gestor de PWM
  Fecha: 20220216
  Autor: Ramón Junquera
  Objetivo: Descripción general y ejemplo simple

  Descripción:
  PCA9685 es una placa para la gestión de PWM.
  Se controla a través del protocolo I2C.
  Es capaz de gestionar hasta 16 canales con una resolución de 12 bits (2^12=4096 niveles).
  Se puede ajustar la frecuencia desde 24Hz hasta 1526Hz.
  Es totalmente autónomo. No es como la familia TLC59xx que requiere una señal de reloj externa para regular la frecuencia.
  El reloj es interno.
  Permite una fuente de alimentación externa para los leds/motores con un voltaje entre 2.3V y 5.5V
  Permite encadenar hasta un máximo de 62 módulos PCA9685, obteniendo 64*16=1024 salidas PWM.

  Utilizaremos la librería: Adafruit PWM Servo Driver Library para el control de PCA9685
  En nuestro ejemplo, puesto que sólo tenemos un led que encender, no alimentamos la PCA9685 externamente.
  Además el led toma su GND del GND general.

  En el caso de utilizar servomotores, es muy conveniente alimentar el módulo externamente porque consumen mucho más que los led.
  En este caso cada servomotor debe tomar la alimentación y control de los 3 pines de cada canal en el módulo.
    
  Resultado:
  Aumentamos y disminuimos la intensidad del led progresivamente.
*/

#include <Arduino.h>
#include <RoJoPCA9685.h> //Gestión de PCA9685

RoJoPCA9685 pwm; //Creamos objeto de gestión con id de I2C

//Variables globales
int delta=1;
int level=0;

void setup() {
  Serial.begin(115200);
  while(!Serial) delay(1);
  pwm.begin();
}
 
void loop() {
  level+=delta; //Calculamos el nuevo nivel
  //Si hemos llegado a un extremo...cambiamos la dirección
  if((level==0) || (level==4095)) delta=-delta;
  pwm.setPWM(0,level); //Aplicamos el nivel al primer canal
}
