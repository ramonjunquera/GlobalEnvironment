#include <Arduino.h>

//Cada familia de placa tiene sus propias librerías de gestión wifi
#ifdef ESP32
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif

//Si es una placa ESP32 genérica...
#ifdef ARDUINO_ESP32_DEV
  //...definimos el pin asociado al led integrado
  const byte LED_BUILTIN=2; //Para ESP32 OLED TTGO 16Mb
  //const byte LED_BUILTIN=22; //WeMosLolin32Lite
#endif

//Los estados del led integrado dependen de la placa
#ifdef ARDUINO_ESP32_DEV //Placas ESP32 genéricas
  //Para ESP32 OLED TTGO 16Mb
  const bool ledON=HIGH;
  const bool ledOFF=LOW;
  //Para WeMosLolin32Lite
  //const bool ledON=LOW;
  //const bool ledOFF=HIGH;
#else //Resto de placas (ESP)
  const bool ledON=LOW;
  const bool ledOFF=HIGH;
#endif

//Definimos los detalles del punto de acceso
const char* wifiSSID = "***";
const char* wifiPassword = "***";
//Definimos el puerto del servidor socket
const uint16_t port=5204;

//Estado actual
bool currentStatus=false;
//Creamos un servidor wifi en el puerto indicado
WiFiServer server(port);

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
  //Mostramos la dirección ip asignada
  Serial.println(WiFi.localIP());

  //Aplicamos el estado inicial al led
  digitalWrite(LED_BUILTIN,currentStatus?ledON:ledOFF);

  //Arrancamos el servidor
  server.begin();
}

void loop()
{
  //Anotamos si hay algún cliente
  WiFiClient client = server.available();
  //Si hay alguno...
  if(client)
  {
    //Esperamos hasta que haya datos pendientes por recibir o haya transcurrido
    //un tiempo determinado
    uint32_t maxMillis=millis()+10;
    while(!client.available() && millis()<maxMillis);
    
    //Si el tenemos información pendiente de recibir del cliente...
    if (client.available())
    {
      //Leemos el byte recibido
      char c=client.read();
      //Aplicamos el estado al led
      digitalWrite(LED_BUILTIN,c==0?ledOFF:ledON);
      //Informamos
      Serial.println(c==0?"apagado":"encendido");
    }
    //Hemos terminado de procesar a este cliente. Cerramos sus conexión
    client.stop();
  }
  //Refrescamos los procesos de fondo para evitar un error de WatchDog
  yield();
}

