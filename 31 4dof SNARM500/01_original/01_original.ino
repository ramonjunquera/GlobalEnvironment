/*
  Autor: Ramón Junquera
  Fecha: 20190120
  Objetivo: Simplificación y análisis de gestión original de 4dof SNARM500
  Material: 4dof SNARM500 (brazo robot de 4 servomotores)
  URL original: http://cdn.sinoning.com/code/snarm.zip

  Modificaciones:
  - Los pines de los servos se guardan en una tabla
  - Los valores máximo y mínimo de recorrido de los servos se guardan en tablas
  - Se sustituye la librería VarSpeedServo.h por la estándar Servo.h
  - Se define como constante el valor de la dirección de la EEPROM donde se guarda el modo
  - Se simplifica la lectura y proceso de comandos por el puerto serie
  - Se simplifica la lectura de potenciómetros, conversión y aplicación de valores al los servos
  - Se aumenta la velocidad del puerto serie a 115200 baudios
  - Se añaden comentarios explicativos

  El brazo robot consta de 4 servomotores. Cada uno de ellos es tiene asociado un
  potenciómetro con el que se controla manualmente.
  Además del control manual, mediante comandos por el puerto serie se pueden asignar valores a los
  servos.

  Comandos:
  - Carácter m. Modo manual. Los servos responden a los potenciómetros.
  - Carácter n. Modo automático. Los servos responden a los comandos del puerto serie.
  - Caracteres numéricos. Se tomarán para componer el valor que se quiere aplicar a un servo.
  - Caracteres a,b,c,d. Corresponden a cada uno de los servos. Se aplicará el valor actual a un servo
    si estamos en modo automático.

  Definición de pines
   #  pinPot pinServo
  --- ------ --------
   0    A0      11
   1    A1      10
   2    A2       9
   3    A3       6
*/

#include <Arduino.h>
#include <Servo.h> //Librería de gestión de los servomotores
#include <EEPROM.h> //Librería de gestión de la memoria EEPROM

//Declaración de constantes globales
const byte pinServos[4]={11,10,9,6}; //Pines de los servos
const byte minServo[4]={90,0,0,40}; //Valores mínimos para la conversión de potenciómetro a servo
const byte maxServo[4]={160,179,179,179}; //Valores máximos para la conversión de potenciómetro a servo
const uint16_t eepromAddress=1001; //Dirección de EEPROM para guardar el modo

//Declaración de variables globales
//VarSpeedServo servos[4]; //Servomotores
Servo servos[4]; //Servomotores
byte mode=0; //Modo: 0=manual (potenciómetro), 1=automático (puerto serie)

void setup()
{
  Serial.begin(115200);
  //Asignamos los pines que controlan a cada servo
  for(byte i=0;i<4;i++) servos[i].attach(pinServos[i]);
  //Leemos el último modo utilizado
  mode=EEPROM.read(eepromAddress);
}
void loop()
{
  static byte v=0; //Valor compuesto por los carácteres numéricos procedentes del puerto serie

  //Si hay algún carácter pendiente de ser recibido por el puerto serie...
  if (Serial.available())
  {
    char ch = Serial.read(); //Leemos un carácter del puerto serie
    switch(ch)
    {
      case '0'...'9': //Si es numérico...
        //El nuevo carácter serán las unidades del valor
        v=v*10+ch-'0'; 
        break;
      case 'a'...'d': //Si es una referencia a un servo: a, b, c, d
        //Si estamos en modo automático...escribimos el valor en el servo que corresponde
        if(mode==1) servos[ch-'a'].write(v);
        v=0; //Reseteamos el valor numérico
        break;
      case 'm'...'n': //Si se trata de un cambio de modo. m=manual, n=automático
        mode=ch-'m'; //Calculamos el modo
        EEPROM.write(eepromAddress,mode); //Escribimos el modo en la posición 1001
        v=0; //Reseteamos el valor numérico
        Serial.print("Mode = ");
        Serial.println(mode?"auto":"manual");
        break;
    }
  }

  //Si estamos en modo manual (potenciómetros)...
  if(mode==0)
  {
    //Recorremos todos los potenciómetros
    for(byte i=0;i<4;i++)
    {
      uint16_t val=analogRead(i); //Leemos el valor del potenciómetro
      val=map(val,0,1023,minServo[i],maxServo[i]); //Mapeamos su valor para el servo
      servos[i].write(val); //Aplicamos el valor
    }
    delay(20);
  }
}
