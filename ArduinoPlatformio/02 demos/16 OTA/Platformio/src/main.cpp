/*
  Tema: ESP32 OTA en Platformio
  Fecha: 20211123
  Autor: Ramón Junquera

  Descripción:
  Ejemplo de programa con capacidades OTA.
  El programa principal es un simple blink.
  La primera vez que cargamos el programa en el dispositivo es necesario hacerlo
  fisicamente por el puerto USB.
  Las actualizaciones posteriores se hacen OTA (Over-The-Air).
  La librería principal de OTA es ArduinoOTA.h que ya incluye WiFi.h & ESPmDNS.h.

  Los pasos habituales para que una aplicación sea OTA son:
  - #include <ArduinoOTA.h>
  - setup. Conexión manual a la red WiFi (o creación del punto de acceso propio)
  - setup. Definición de funciones de ArduinoOTA: onStart, onEnd, onProgress & onError.
  - setup. Definición opcional de contraseña de actualización
  - setup. Definición opcional de hostname para facilitar su localización
  - setup. Inicialización de OTA
  - loop. Llamadas periódicas a ArduinoOTA.handle() para comprobación de solicitudes de
    actualización.

  Configuración de Platformio.ini
  Para actualizar un programa por OTA debemos añadir los siguientes parámetros:
  - upload_protocol con el valor espota. Si no lo hacemos lo pondrá él tras quejarse con
    un warning: upload_protocol = espota
  - upload_port con la dirección ip o el hostname del dispositivo: upload_port = BlinkOTA32.local
  - upload_flags con el flag de autorización y la contraseña, siempre que sea obligatoria:
    upload_flags = -aadmin

  Preguntas:
  P. Puedo transferir por OTA si el programa que contiene el dispositivo no está preparado para OTA?
  R. No

  P. Puedo transferir una actualización OTA si no recuerdo la contraseña actual?
  R. No. Debes transferir por USB y sobreescribir el programa actual.

  P. Puedo poner una contraseña para la actualización del programa y otra a la actualización del
     sistema de archivos?
  R. No. La contraseña es única.

  P. La subida de imágenes de sistemas de archivos es compatible con LittleFS?
  R. Sí.

  P. Qué puerto utiliza para la comunicación?
  R. 3232/tcp

  Funcionamiento del programa:
  Comienza intentando conectar con un punto de acceso WiFi. Si no lo consigue en 10 segundos,
  reinicia el dispositivo.
  Mientras está intentando conectar, hace parpadear el led rapidamente.
  Después pasa a parpadear con el periodo determinado.
  Si se detecta una solicitud de actualización de programa, mientras dure la transmisión, el led
  parpadea rápidamente.

  Identificación de errores.
  Los mensajes de error han sido sustituidos por un código de parpadeos en el led integrado.
  Si se produce algún error OTA, el led se encenderá tantas veces como el código de error
  durante 3 veces.
  Los códigos de error son los siguientes
   1 : OTA_AUTH_ERROR
   2 : OTA_BEGIN_ERROR
   3 : OTA_CONNECT_ERROR
   4 : OTA_RECEIVE_ERROR
   5 : OTA_END_ERROR

  Notas:
  - Debido a un error de diseño, el reset por software no funciona la primera vez que se utiliza,
    Si el dispositivo intenta conectar a WiFi y no lo consigue en el tiempo estipulado, la 
    primera vez que intente resetear, se quedará colgado.
*/

#include <Arduino.h>
#include <ArduinoOTA.h> //Ya incluye <WiFi.h/ESP8266WiFi.h> & <ESPmDNS.h>

//Constantes globales
const char* ssid = "xxx";
const char* password = "xxx";
const uint32_t delayValue=1000;
const byte pinLed=LED_BUILTIN;
const bool ledON=LOW;

//Intenta conectar con el punto de acceso y contraseña definidas en constantes globales
//Si lo consigue, devuelve el control.
//Si no lo consigue en menos de 10 segundos, resetea y comienza de nuevo.
void try2connect() {
  //Variable para contar las veces que repetimos el bucle
  //Si el bucle dura 100ms -> 100*100=10000ms=10s
  byte loops=100;
  pinMode(pinLed,OUTPUT); //Configuramos el pin del led integrado como salida
  WiFi.mode(WIFI_STA); //Conectado como cliente a un punto de acceso
  WiFi.begin(ssid,password); //Inicializamos conexión wifi con las credenciales
  //Repetir mientras no se haya conectado ni haya superado el tiempo máximo
  while (WiFi.status() != WL_CONNECTED && loops>0) {
    digitalWrite(pinLed,!digitalRead(pinLed)); //Cambiamos el estado del led integrado
    delay(100); //Esperamos un momento
    loops--; //Ya tenemos un ciclo menos
  }
  //Si no hemos podido conectar...reseteamos
  if(WiFi.waitForConnectResult() != WL_CONNECTED) ESP.restart();
  //Hemos podido conectar!
  digitalWrite(pinLed,!ledON); //Apagamos el led integrado
  //Devolvemos el control
}

void setup() {
  try2connect(); //Intentamos conectar
  //Definimos las funciones de OTA
  ArduinoOTA.onStart([]() { //Cuando inicia
    pinMode(pinLed,OUTPUT); //Configuramos el pin del led integrado como salida
    //La petición OTA puede actualizar la partición de:
    //- Programa (U_FLASH)
    //- Sistema de archivos (U_SPIFFS)
    //La distinguiremos por el valor de ArduinoOTA.getCommand()
    //Es importante porque para actualizar la partición de sistema de archivos es
    //imprescindible que lo tengamos desmontado.
    //El ejemplo del código sería:
    //  if(ArduinoOTA.getCommand()==U_SPIFFS) SPIFFS.end();
  });
  ArduinoOTA.onEnd([]() { //Ha finalizado la transferencia OTA
    digitalWrite(pinLed,!ledON); //Apagamos el led integrado
  });
  ArduinoOTA.onProgress([](unsigned int progress,unsigned int total) { //Ha cambiado el porcentaje de progreso OTA
    digitalWrite(pinLed,!digitalRead(pinLed)); //Cambiamos el estado del led integrado
  });
  ArduinoOTA.onError([](ota_error_t error) { //Se ha producido un error en la recepción OTA
    digitalWrite(pinLed,!ledON); //Led apagado
    delay(1000); //Esperamos un segundo para comenzar con el código de error
    for(byte i=3;i>0;i--) { //Repetiremos 3 veces...
      //Repetimos el siguiente bucle tantas veces como error+1 (porque el primer error es el 0)
      for(byte e=0;e<2*(error+1);e++) {
        digitalWrite(pinLed,!digitalRead(pinLed)); //Cambiamos el estado del led integrado
        delay(500); //Esperamos medio segundo
      }
      //Hemos mostrado el código de error
      delay(1000); //Esperamos un segundo para indicar que hemos terminado de emitir el código
    }
    //Hemos terminado
    //Tras un error OTA, siempre se reinicia automáticamente el finalizar esta función
  });
  //Asignamos una contraseña para impedir que cualquiera pueda actualizarlo
  //No es obligatorio
  ArduinoOTA.setPassword("admin");
  //Asignamos un nombre al dispositivo para tenerlo identificado
  //Si no lo hacemos tomará como nombre por defecto: "esp32-[ChipID]"
  ArduinoOTA.setHostname("BlinkOTA32");
  ArduinoOTA.begin(); //Iniciamos el servicio de OTA
}

void loop() {
  static uint32_t changeTime=0; //Tiempo de ejecución en milisegundos para siguiente cambio de estado del led
  ArduinoOTA.handle(); //Camprobamos si hay alguna petición de actualización por OTA
  if(millis()>changeTime) { //Si ya hay que cambiar el estado del led...
    digitalWrite(pinLed,!digitalRead(pinLed)); //...se cambia
    changeTime=millis()+delayValue; //Calculamos cuándo ocurrirá el siguiente cambio
  }
}
