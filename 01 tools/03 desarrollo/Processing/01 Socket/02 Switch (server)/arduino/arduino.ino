#include <Arduino.h>
#include "RoJoSwitch.h" //Gestión de interruptores

//Cada familia de placa tiene sus propias librerías de gestión wifi
#ifdef ESP32
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif

//Si es una placa ESP32 genérica...
#ifdef ARDUINO_ESP32_DEV
  //...definimos el pin asociado al led integrado
  //const byte LED_BUILTIN=2; //Para ESP32 OLED TTGO 16Mb
  const byte LED_BUILTIN=22; //WeMosLolin32Lite
#endif

//Definimos el pin del interruptor
#ifdef ESP32
  //Seleccionamos el pin 0 porque algunas placas ESP32 ya tienen un interruptor integrado asignado a este pin
  const byte pinSwitch=0;
#elif defined(ESP8266)
  //Seleccionamos el pin D3 porque algunas placas ESP8266 ya tienen un interruptor integrado asignado a este pin
  const byte pinSwitch=D3;
#endif

//Los estados del led integrado dependen de la placa
#ifdef ARDUINO_ESP32_DEV //Placas ESP32 genéricas
  //Para ESP32 OLED TTGO 16Mb
  //const bool ledON=HIGH;
  //const bool ledOFF=LOW;
  //Para WeMosLolin32Lite
  const bool ledON=LOW;
  const bool ledOFF=HIGH;
#else //Resto de placas (ESP)
  const bool ledON=LOW;
  const bool ledOFF=HIGH;
#endif

//Definimos los detalles del punto de acceso
const char* wifiSSID = "***";
const char* wifiPassword = "***";
//Definimos los detalles del servidor web socket
const char* host = "192.168.1.196";
const uint16_t port=5204;

//Declaramos el objeto de gestión del interruptor
RoJoSwitch mySwitch(pinSwitch);
//Estado actual
bool currentStatus=false;
//Objeto de gestión de la conexión wifi
WiFiClient client;

void setup()
{
  Serial.begin(115200);
  //El pin del led integrado se configura de salida
  pinMode(LED_BUILTIN,OUTPUT);
  //Se aplica el estado inicial
  digitalWrite(LED_BUILTIN,currentStatus?ledON:ledOFF);
  
  //Inicialización de conexión wifi
  WiFi.mode(WIFI_STA); //Conectado como cliente a un punto de acceso
  WiFi.begin(wifiSSID,wifiPassword);
  Serial.print(F("Conectando a punto de acceso"));
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println(F("ok"));
}

void loop()
{
  //Si se ha pulsado el botón...
  if(mySwitch.pressed())
  {
    //...cambiamos el estado actual
    currentStatus=!currentStatus;
    //Aplicamos el nuevo estado el led integrado
    digitalWrite(LED_BUILTIN,currentStatus?ledON:ledOFF);
    //Si podemos conectar con el servidor...enviamos el estado actual
    if(client.connect(host,port))
    {
      //...enviamos el estado actual
      client.write(currentStatus?1:0);
      //Mostramos enviado
      Serial.println(currentStatus?"encendido":"apagado");
    }
    else Serial.println("Error al conectar con socket");
  }
  //Refrescamos los procesos de fondo para evitar un error de WatchDog
  yield();
}

