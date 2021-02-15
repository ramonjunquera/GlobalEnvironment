/*
  Autor: Ramón Junquera
  Tema: ESP8266 OTA
  Fecha: 20181004
  Objetivo: Descripción y análisis del ejemplo base
  Material: placa ESP
  Descripción:
  Una de librerías más interesantes de la familia ESP es la OTA.
  OTA permite transferir programas a nuestra placa a través del WiFi que tiene incorporado.
  OTA = Over The Air
  Gracias a esto, ya no sería necesaria la conexión con cable a un ordenador para actualizar el
  programa que se ejecuta.
  Esto facilita mucho la actualización de aplicaciones en dispositivos que se encuentran en zonas de
  dificil acceso.

  La librería OTA pertenece a la colección de librerías que se entregan con un ESP.
  Es la misma para ESP8266 y ESP32.

  Para que nuestra placa pueda ser actualizada por OTA tendrá que estar ejecutando un programa que
  esté preparado para ello.
  No es complicado, pero debemos añadir un poco de código a nuestras aplicaciones.

  A continuación se muestra el ejemplo BasicOTA. No tiene ninguna modificación. Sólo se han incluido
  comentarios explicativos.

  Este programa es la base de OTA. Es el código que debemos añadir a nuestras aplicaciones.

  El programa sólo permite la actualización por OTA. No tiene más funcionalidades.

  Para poder acceder a nuestro dispositivo a través de wifi, lo primero es activar wifi y dejarlo
  accesible.
  La manera más sencilla es conectándonos a una red wifi existente. Por eso se definen un par de constantes
  al principio del programa para el nombre del punto de acceso y su contraseña.
  No es obligatorio que el acceo sea así. Podríamos configurar la placa como punto de acceso independiente
  del resto y obligar al cliente a conectarse.

  A continuación se activa el puerto serie para notificar del estado y mensajes a través del monitor serie.
  Rewalmente no es necesario, porque la placa tendrá desconectado el puerto serie. Pero para hacer pruebas
  la primera vez está bien.

  Nos intentamos conectar al punto de acceso con la contraseña indicada. Si no se puede se resetea la placa
  y se volverá a intentar indefinidamente.

  Después definimos unas funcionas propias de OTA que informarán del estado, porcentaje de la transfeencia 
  y mensajes de error a través del puerto serie.

  Para finalizar las instrucciones de la función setup() se inicia OTA y se muestra la ip asignada.

  La parte más importante está en la función loop().
  Es imprescindible incluir periódicamente una llamada a ArduinoOTA.handle() para que se gestione
  cualquier petición de actualización.

  En ejercicios posteriores veremos ejemplos de programas adaptados a OTA.

  Hay unas líneas que están comentadas.
  Hacen referencia al puerto utilizado para OTA. Por defecto es el 8266.
  A la contraseña. Por defecto no tiene. Cualquiera podría actualizar el programa.
  E nombre del dispositivo. Si no lo hacemos quedará con un nombre finalizado por el identificador del chip.

  Hasta aquí la explicación del programa.
  Pero qué debo hacer para poder transferir?

  Lo primero es instalar Python.
  La documentación recomienda la última release de la versión 2, pero la versión 3 también funciona 
  perfectamente. A fecha de hoy 04/10/2018 es la 3.7.0.

  Después haremos la transferencia de este programa a través de cable. La primera vez no tenemos más
  posibilidades.
  A partir de este instante deberíamos ver un nuevo puerto de conexión bajo el menú Herramientas/Puerto.
  Es posible que la primera vez no aparezca. La solución es cerrar el IDE y volverlo a abrir.

  Esto es todo lo que necesitamos.
  A partir de ahora la placa se puede actualizar por puerto serie o por OTA. Con la ventaja de que por
  OTA, la placa sólo necesita alimentación. No un ordenador.

  Resultado:
  El IDE tiene un nuevo puerto remoto para la transferencia de programas
*/

//Incluimos las librerías necesarias para OTA: gestión de wifi (dependiendo de la placa) y la librería
//de OTA para el entorno de desarrollo de Arduino.
#ifdef ESP8266
  #include <ESP8266WiFi.h>
#elif defined(ESP32)
  #include <WiFi.h>
#endif
//#include <ESP8266mDNS.h>
//#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char* ssid = "xxx";
const char* password = "xxx";

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.println("Connection succesfull!");

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  ArduinoOTA.handle();
}
