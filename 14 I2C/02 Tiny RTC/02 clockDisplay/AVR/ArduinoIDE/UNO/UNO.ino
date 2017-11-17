/*
  Autor: Ramón Junquera
  Tema: Módulo de reloj Tiny RTC I2C Real Time
  Objetivo: Demo de librería
  Material adicional: breadboard, Arduino UNO, cables, módulo "Tiny RTC I2C"

  Descripción:
 
  No se pone en hora el reloj!!!!
  
  Mostraremos las capacidades de la librería de gestión del RTC.
  Inicialmente comprobamos si el reloj está presente.
  Lo ponemos en hora en función de la hora de compilación.
  Mostramos por el puerto serie la hora completa cada segundo.
  
  Resultado:
  Vemos la hora del reloj en el monitor del puerto serie
*/

#include <Arduino.h>
#include "RoJoRTCclock.h" //Gestión del reloj del módulo Tiny RTC
#include "RoJoTM1637.h" //Gestión del display

//Definimos constantes de pines
const byte pinCLK=2;
const byte pinDIO=3;
const byte pinSwitch=12;

//Creamos objetos de gestión
RoJoRTCclock clock;
RoJoTM1637 display;

//Modo. Qué se muestra en pantalla?
// 0 = __:SS
// 1 = HH:MM
// 2 = DD MM
// 3 = YY YY
byte displayMode=1; //Comenzamos en el modo 1

//Próximo refresco del display
uint32_t nextRefresh=0;

void showError()
{
  //Mostramos el display vación con las dos puntos parpadeando continuamente
  display.clear();
  while(1)
  {
    display.setDoubleDot(true);
    delay(500);
    display.setDoubleDot(false);
    delay(500);
  }
}

void checkSwitch()
{
  //Comprueba si se ha pulsado el interruptor integrado

  //Próxima vez que será válida una pulsación
  static uint32_t nextPress=0;
  //Estado anterior
  static bool lastStatus=HIGH;

  //Hora actual
  uint32_t localTime=millis();
  //Estado actual
  bool currentStatus=digitalRead(pinSwitch);
  //Si el estado ha cambiado...
  if(lastStatus!=currentStatus)
  {
    //El interruptor ha cambiado de estado
    //Si el estado actual es pulsado y ya podemos cambiar el estado...
    if(!currentStatus && localTime>nextPress)
    {
      //...lo cambiamos
      displayMode++;
      //Nunca podrá ser mayor que 3
      displayMode%=4;
    }
    //Calculamos cuándo podremos admitir el próximo cambio de estado
    nextPress=localTime+300; //En 3 décimas de segundo
  }
}

void setup()
{
  //Inicializamos el display
  display.begin(pinCLK,pinDIO);
  //Ajustamos a un brillo medio
  display.setBrightness(4);

  //Si el reloj no está presente...mostramos error en el display
  if(!clock.check()) showError();
  //Configuramos el pin del interruptor como entrada con resistencias de pullup activas
  //Nos permitirá cambiar de modo de visualización
  pinMode(pinSwitch,INPUT_PULLUP);
}

void loop()
{
  //Obtenemos el tiempo que lleva encendido el dispositivo
  uint32_t localTime=millis();
  //Si hay que refrescar el display...
  if(localTime>nextRefresh)
  {
    //Obtenemos la hora del RTC
    datetime currentTime=clock.get();
    //Dependiendo del modo de visualización...
    switch(displayMode)
    {
      case 0: //Modo __:SS
        //Dibujamos los puntos
        display.setDoubleDot(true);
        //Borramos los dos primeros caracteres
        display.set2DigitChar(0,16,16);
        //Escribimos los segundos
        display.set2DigitValue(2,currentTime.second);
        break;
      case 1: //Modo HH:MM
        //Dibujamos los puntos si estamos en el primer medio segundo
        display.setDoubleDot(localTime%1000<500);
        //Escribimos la hora
        display.set2DigitValue(0,currentTime.hour);
        //Escribimos los minutos
        display.set2DigitValue(2,currentTime.minute);
        break;
      case 2: //Modo DD MM
        //No dibujamos los puntos
        display.setDoubleDot(false);
        //Escribimos el dia
        display.set2DigitValue(0,currentTime.day);
        //Escribimos el mes
        display.set2DigitValue(2,currentTime.month);
        break;
      case 3: //Modo YY YY
        //No dibujamos los puntos
        display.setDoubleDot(false);
        //Escribimos los dos primeros dígitos del año
        display.set2DigitValue(0,currentTime.year/100);
        //Escribimos los dos últimos dígitos del año
        display.set2DigitValue(2,currentTime.year%100);
        break;
      default:
        break;
    }
    //Se refrescará de uevo en medio segundo
    nextRefresh+=500;
  }
  //Comprueba si se ha pulsado el interruptor
  checkSwitch();
}
