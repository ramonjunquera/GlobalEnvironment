/*
  Autor: Ramón Junquera
  Tema: Chip TM1637 / Display Catalex
  Objetivo: Leer y mostrar la hora real de Internet en el display
  Material adicional: breadboard, ESP-12E/NomdeMCU 1.0,cables,
    display de led de 4 dígitos tipo reloj con chip TM1637

  Descripción:
  Aprovechando las capacidades de conectividad de los ESP, nos conectaremos a un punto
  de acceso wifi que nos permita salir hasta Internet.
  Gracias a esto podemos obtener la hora de real de un servidor NTP.

  Mostraremos en el display los datos de la fecha y hora actuales.
  Puesto que sólo tenemos 4 dígitos, necesitamos varios modos de visualización.
  Los modos serán los siguientes:
    Modo 0: Se muestran los segundos en formato __:SS. Los puntos no parpadean.
    Modo 1: Se muestra la hora y los minutos en formato HH:MM. Los puntos parpadean.
    Modo 2: Se muestra el dia y el mes en formato DD_MM. No hay puntos.
    Modo 3: Se muestra el año. Sin puntos.
  Los modos son cíclicos.
  Para cambiar de modo utilizaremos el interruptor integrado en placa (FLASH).

  Para no tener que estar consultando Internet cada segundo, guardamos la diferencia
  entre la hora real de Internet y el tiempo que lleva encendido el dispositivo.
  Suponemos que esta diferencia se mantiene constante en el tiempo.
  Esta diferencia (gap) es la que tomamos como base para el cálculo de todos los 
  detalles de la fecha.

  Puesto que el reloj interno no es infalible, hemos indicado que cada hora se vuelva
  a conectar a Internet y se sincronice de nuevo (recalcule el gap).

  Iniciamos en modo 1, que es el más común.

  Atención:
  Si no nos conectamos a un punto de acceso, recuerda los detalles del último utilizado
  y se intenta conectar automáticamente.
  Esta es la razón por la que si eliminamos las líneas:
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifiSSID, wifiPassword);
  La sincronización horaria seguiría funcionando igual!

  Resultado:
  Vemos el tiempo real en el display
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <time.h>
#include "RoJoTM1637.h"

//Definimos constantes de pines
const byte pinCLK=D5;
const byte pinDIO=D7;
const byte pinSwitch=D3;

//Definimos como constantes el nombre del punto de acceso y su contraseña
const char *wifiSSID = "xxx";
const char *wifiPassword = "xxx";

//Definimos objeto de gestión del display
RoJoTM1637 display;

//Zona horaria = GMT+1
const int8_t timeZone = 1;
//Horario de verano?
const bool summerTime=true; //Sí

//Número de segundos de diferencia entre la hora real y el tiempo
//que lleva el dispositivo encendido
uint32_t timeGap=0;

//Modo. Qué se muestra en pantalla?
// 0 = __:SS
// 1 = HH:MM
// 2 = DD MM
// 3 = YY YY
byte displayMode=1; //Comenzamos en el modo 1

//Próximo refresco del display
uint32_t nextRefresh=0;

uint32_t syncTimeNTP()
{
  //Comprueba si debe sincronizar la hora con Internet
  //y lo hace si es necesario (calcula el gap de tiempo)
  //Devuelve millis

  //Próxima vez que se actualizará la hora de Internet
  static uint32_t nextTimeSync=0;

  //Tiempo que lleva en marcha el dispositivo
  uint32_t localTime=millis();
  //Si hay que sincronizar la hora con Internet...
  if(localTime>nextTimeSync)
  {
    //Obtenemos la fecha y hora reales
    time_t realTime = time(nullptr);
    //Si lo hemos conseguido...
    if (realTime)
    {
      //Si debemos sumarle la hora del horario de verano...lo hacemos
      if (summerTime) realTime += 3600;
      //Calculamos el gap de tiempo
      timeGap=realTime-localTime/1000;
      //Hemos conseguido obtener la hora
      //La próxima sincronización será dentro de una hora
      // 1h=60min
      // 60min=60*60=3600s
      // 3600s=3600*1000=3600000ms
      nextTimeSync=localTime+3600000;
    }
  }
  //Devolvemos millis
  return localTime;
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
  //Ponemos el wifi en modo cliente que conecta a punto de acceso
  WiFi.mode(WIFI_STA);
  //Conecta con punto de acceso
  WiFi.begin(wifiSSID, wifiPassword);
  //Si las credenciales son correctas, ya conectará y permitirá el
  //acceso a los servidores de NTP de Internet

  //Inicializamos el display
  display.begin(pinCLK,pinDIO);
  //Ajustamos a un brillo medio
  display.setBrightness(4);
  //Definimos la hora de Internet
  //A la UTC le debemos añadir tantas horas como la zona horaria en la que estemos
  configTime((int32_t)timeZone * 3600, 0, "pool.ntp.org", "time.nist.gov");
  //Configuramos el pin del interruptor como entrada con resistencias de pullup activas
  //Nos permitirá cambiar de modo de visualización
  pinMode(pinSwitch,INPUT_PULLUP);
}

void loop()
{
  //Sincronizamos la hora de Internet si es necesario
  //Y obtenemos millis
  uint32_t localTime=syncTimeNTP();
  //Si hay que refrescar el display...
  if(localTime>nextRefresh)
  {
    //Obtenemos la hora real en base el reloj interno
    uint32_t realTime=localTime/1000+timeGap;
    //Creamos un objeto tm para descomponer la hora actual
    tm *t;
    //Convertimos la fecha real en segundos a formato tm
    t=localtime((time_t*)&realTime);

    //Dependiendo del modo de visualización...
    switch(displayMode)
    {
      case 0: //Modo __:SS
        //Dibujamos los puntos
        display.setDoubleDot(true);
        //Borramos los dos primeros caracteres
        display.set2DigitChar(0,16,16);
        //Escribimos los segundos
        display.set2DigitValue(2,(*t).tm_sec);
        break;
      case 1: //Modo HH:MM
        //Dibujamos los puntos si estamos en el primer medio segundo
        display.setDoubleDot(localTime%1000<500);
        //Escribimos la hora
        display.set2DigitValue(0,(*t).tm_hour);
        //Escribimos los minutos
        display.set2DigitValue(2,(*t).tm_min);
        break;
      case 2: //Modo DD MM
        //No dibujamos los puntos
        display.setDoubleDot(false);
        //Escribimos el dia
        display.set2DigitValue(0,(*t).tm_mday);
        //Escribimos el mes
        display.set2DigitValue(2,(*t).tm_mon+1);
        break;
      case 3: //Modo YY YY
        //No dibujamos los puntos
        display.setDoubleDot(false);
        //Escribimos los dos primeros dígitos del año
        display.set2DigitValue(0,((*t).tm_year+1900)/100);
        //Escribimos los dos últimos dígitos del año
        display.set2DigitValue(2,((*t).tm_year+1900)%100);
        break;
      default:
        break;
    }
    //Se refrescará de uevo en medio segundo
    nextRefresh+=500;
  }
  //Comprueba si se ha pulsado el interruptor integrado
  checkSwitch();
}
