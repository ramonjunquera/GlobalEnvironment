/*
  Autor: Ramón Junquera
  Fecha: 20181004
  Tema: ESP8266 OTA
  Objetivo: Ejemplo blink para OTA
  Material: ESP8266: ESP-12E, Wemos D1 Mini, Wemos D1 R2, Wemos D1 Mini Pro

  Descripción:
  Utilizando el ejemplo BasicOTA, le añadiremos el código del programa blink.
  Podemos transferir el programa por OTA y además el nuevo programa también 
  estará preparado para ser actualizado por OTA.

  Se deja de utilizar el puerto serie.
  Los mensajes han sido sustituidos por un código de parpadeos en el led integrado.
  Intenta conectar con el punto de acceso durante 10 segundos. Mientras tanto el led
  parpadea rápidamente.
  Si lo consigue, se apaga y continúa con la inicialización.
  Si no lo consigue, se resetea y comienza de nuevo.
  Hay que recordar que el reset por software no funcionará la primera vez. Es un problema
  de diseño. La primera vez se debe resetear manualmente.

  Si se detecta una actualización OTA, se configurará el led integrado como salida.
  Cada vez que cambie el porcentaje de carga OTA cambiará el estado del led.
  Cuando finalize se apagará.
  Si se produce algún error OTA, el led se encenderá tantas veces como el código de error
  durante 3 veces.
  Los códigos de error son los siguientes
   1 : OTA_AUTH_ERROR
   2 : OTA_BEGIN_ERROR
   3 : OTA_CONNECT_ERROR
   4 : OTA_RECEIVE_ERROR
   5 : OTA_END_ERROR

  Para facilitar las pruebas, se han definido como constantes el pin del led integrado,
  el tiempo de parpadeo y el valor correspondiente al led encendido, porque hay placas
  que el led integrado tiene un comportamiento inverso.

  Nota:
  El plugin de subida de archivos a SPIFFS no sabe gestionar contraseñas.
  Si queremos utilizarlo, anten debemos desactivar la contraseña de actualización.

  Resultado:
  Hacemos parpadear uno de los leds integrados, transfiriendo el programa por OTA
*/

#include <ESP8266WiFi.h> //Para gestión de wifi
#include <ArduinoOTA.h> //Para gestión de OTA
//#include <FS.h> //Gestión de partición para archivos

const char* ssid = "xxx";
const char* password = "xxx";
const uint32_t delayValue=1000;
const byte pinLed=LED_BUILTIN;
const bool ledON=LOW; //Ok para ESP-12E, Wemos D1 Mini, Wemos D1 R2, Wemos D1 Mini Pro

void try2connect()
{
  //Intenta conectar con el punto de acceso y contraseña definidas en constantes globales
  //Si lo consigue, devuelve el control.
  //Si no lo consigue en menos de 10 segundos, resetea y comienza de nuevo.

  //Nos conectaremos a un punto de acceso
  WiFi.mode(WIFI_STA);
  //Inicializamos conexión wifi con las credenciales
  WiFi.begin(ssid,password);
  //Configuramos el pin del led integrado como salida
  pinMode(pinLed,OUTPUT);
  //Variable para contar las veces que repetimos el bucle
  //Si el bucle dura 100ms -> 100*100=10000ms=10s
  byte loops=100;
  //Repetir mientras no se haya conectado ni haya superado el tiempo máximo
  while (WiFi.waitForConnectResult() != WL_CONNECTED && loops>0)
  {
    //Cambiamos el estado del led integrado
    digitalWrite(pinLed,!digitalRead(pinLed));
    //Esperamos un momento
    delay(100);
    //Ya tenemos un ciclo menos
    loops--;
  }
  //Si no hemos podido conectar...reseteamos
  if(WiFi.waitForConnectResult() != WL_CONNECTED) ESP.restart();
  //Hemos podido conectar!
  //Apagamos el led integrado
  digitalWrite(pinLed,!ledON);
  //Devolvemos el control
}

void setup()
{
  //Intentamos conectar
  try2connect();
  //Definimos las funciones de OTA
  ArduinoOTA.onStart([]()
  {
    //Se ha detectado el inicio de una recepción OTA
    //Configuramos el pin del led integrado como salida
    pinMode(pinLed,OUTPUT);
    //La petición OTA puede ser de dos tipos:
    // - Para actualizar el programa (ArduinoOTA.getCommand()==U_FLASH)
    // - Para actualizar SPIFFS (ArduinoOTA.getCommand()==U_SPIFFS)
    //Tenemos de distinguirlo, porque si es para actualizar SPIFFS, debemos
    //desmontar la partición SPIFFS para dejar que lo actualice sin problemas.
    //En este ejemplo no se utiliza SPIFFS, así que no tenemos que hacer nada.
    //Si fuese el caso tendríamos el #include <FS.h> y deberíamos ejecutar:
    //Si la petición es para actualizar SPIFFS...desmontamos la partición SPIFFS
    //if(ArduinoOTA.getCommand()==U_SPIFFS) SPIFFS.end();
  });
  ArduinoOTA.onEnd([]()
  {
    //Ha finalizado la transferencia OTA
    //Apagamos el led integrado
    digitalWrite(pinLed,!ledON);
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
  {
    //Ha cambiado el porcentaje de progreso OTA
    //Cambiamos el estado del led integrado
    digitalWrite(pinLed,!digitalRead(pinLed));
  });
  ArduinoOTA.onError([](ota_error_t error)
  {
    //Se ha producido un error en la recepción OTA

    //Led apagado
    digitalWrite(pinLed,!ledON);
    //Esperamos un segundo para comenzar con el código de error
    delay(1000);

    //Repetiremos 3 veces...
    for(byte i=0;i<3;i++)
    {
      //Repetimos tantas el doble de veces de error+1
      //Porque el primer error es el 0
      //Y porque el bucle cambia el estado del led
      //Suponemos que siempre comenzamos desde apagado
      for(byte e=0;e<2*(error+1);e++)
      {
        //Cambiamos el estado del led integrado
        digitalWrite(pinLed,!digitalRead(pinLed));
        //Esperamos medio segundo
        delay(500);
      }
      //Hemos mostrado el código de error
      //Esperamos un segundo para indicar que hemos terminado de emitir el código
      delay(1000);
    }
    //Hemos terminado
    //Tras un error OTA, siempre se reinicia automáticamente
  });

  //Asignamos una contraseña para impedir que cualquiera pueda actualizarlo
  //No es obligatorio
  //Atención!. Es incompatible con el plugin de subida de archivos a SPIFFS
  ArduinoOTA.setPassword("admin");
  //Asignamos un nombre al dispositivo para tenerlo identificado
  //Si no lo hacemos tomará como nombre por defecto: "esp8266-[ChipID]"
  ArduinoOTA.setHostname("BlinkOTA8266");
  //Iniciamos el servicio de OTA
  ArduinoOTA.begin();
}

//Tiempo de ejecución en milisegundos para siguiente cambio de estado del led
uint32_t changeTime=0;

void loop()
{
  //Camprobamos si hay alguna petición de actualización por OTA
  ArduinoOTA.handle();
  //Si ya hay que cambiar el estado del led...
  if(millis()>changeTime)
  {
    //...se cambia
    digitalWrite(pinLed,!digitalRead(pinLed));
    //Calculamos cuándo ocurrirá el siguiente cambio
    changeTime=millis()+delayValue;
  }
}
