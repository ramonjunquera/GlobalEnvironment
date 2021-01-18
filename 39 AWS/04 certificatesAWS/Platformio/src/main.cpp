/*
  Autor: Ramón Junquera
  Fecha: 20210114
  Tema: MQTT
  Objetivo: Interactuar con AWS IoT con identificación por certificados
  Material: M5Stack Atom Lite
  Descripción:
    Publicación. Cada vez que se pulse el botón, se enviará el estado
    inverso del led al broker.
    Suscripción. Se aplicará el estado recibido al led.

    Utilizaremos el mismo usuario para la suscripción y la publicación.

    La conexión de suscripción debe mantenerse abierta siempre para poder recibir
    mensajes. Nos preocuparemos de que así sea comprobandola continuamente.
    Como utilizamos la misma conexión para publicar, no perderemos tiempo estableciendo
    comunicación cada vez que queramos publicar.

    Si tras pulsar el botón, el led cambia de estado, es que funciona
    tanto la publicación como la suscripción.

    Si se implementa el mismo programa en varios dispositivos, todos
    cambiarán simultáneamente al estado inverso del que se pulsa.

    El broker de AWS IoT acepta conexiones con certificado.
    Es obligatorio indicar el identificador de dispositivo para conectar.
    El identificador de dispositivo debe ser único, sino interferirá
    con los demás. Utilizaremos la dirección MAC como identificador.

    Podemos comprobar que funciona con los sripts de Mosquitto:
    Suscripción:
mosquitto_sub --cafile ~/Downloads/AmazonRootCA1.pem --cert ~/Downloads/Atom-certificate.pem.crt --key ~/Downloads/Atom-private.pem.key -h apj0acah6jgct-ats.iot.us-east-1.amazonaws.com -t "test"
    Publicación:
mosquitto_pub --cafile ~/Downloads/AmazonRootCA1.pem --cert ~/Downloads/Atom-certificate.pem.crt --key ~/Downloads/Atom-private.pem.key -h apj0acah6jgct-ats.iot.us-east-1.amazonaws.com -t "test" -m "{\"message\":\"Hola\"}"
    
    Tabien podemos comprobarlo con el cliente MQTT que integra AQS IoT:
    - Abrimos la consola de AWS: https://console.aws.amazon.com/console/home?region=us-east-1
    - Localizamos IoT Core
    - Seleccionamos en el menú del panel izquierdo: Prueba/Cliente de prueba MQTT
    - .....----ESTOY AQUI-----
*/

#include <Arduino.h>
#include <RoJoNeoPixelAtomLite.h> //Gestión de led NeoPixel en M5Stack Atom Lite
#include <RoJoSwitch.h> //Gestión de interruptor
#include <WiFiClientSecure.h> //Cliente de conexiones seguras
#include <PubSubClient.h> //Cliente de MQTT
#include <SPIFFS.h> //Gestión de archivos

//Contantes globales
const char* wifiSSID="xxx"; //Nompre del punto de acceso wifi
const char* wifiPassword="xxx"; //Contraseña del punto de acceso wifi
const char* mqtt_server="apj0acah6jgct-ats.iot.us-east-1.amazonaws.com"; //Dirección del broker
const uint16_t mqtt_port=8883; //Puerto estándar con seguridad
const String caCertFile="/AmazonRootCA1.pem";
const String clientCertFile="/Atom-certificate.pem.crt";
const String clientKeyFile="/Atom-private.pem.key";

//Variables globales
RoJoNeoPixelAtomLite led; //Objeto de gestión de leds NeoPixel
RoJoSwitch button(39); //Objeto de gestión de pulsador. Pin 39 en Atom
bool ledStatus=false; //Inicialmente apagado
WiFiClientSecure clientWifi; //Conexión wifi
PubSubClient client(clientWifi); //Conexión al broker
char *caCert,*clientCert,*clientKey; //Punteros

//Función de gestión de suscripción
void callback(char* topic,byte* payload,unsigned int length) {
  //No tendremos en cuenta el tópico porque suponemos que es test
  //No tendremos en cuenta la longitud porque suponemos que es 1
  if(payload[0]=='1') { //Si hay que encender el led...
    led.draw({30,30,30}); led.draw({30,30,30});
    ledStatus=true;
  } else { //Si hay que apagar el led...
    led.draw(); led.draw();
    ledStatus=false;
  }
}

//Recupera la conexión si se pierde
void reconnect() {
  if(!client.connected()) { //Si no tenemos conexión de suscripción con el broker...
    //Si conseguimos conectar...volvemos a hacer la suscripción
    if(client.connect(("sub"+WiFi.macAddress()).c_str())) client.subscribe("test");
  }
}

void setup() {
  led.begin(); //Inicialización de led NeoPixel en M5Stack Atom Lite o Echo
  led.draw({10,0,0}); //Comenzamos en rojo mientras conectamos a wifi
  WiFi.mode(WIFI_STA); //Conectado como cliente a un punto de acceso
  WiFi.begin(wifiSSID,wifiPassword); //Solicitamos conexión
  while(WiFi.status()!=WL_CONNECTED) delay(100); //Esperamos a que conecte
  if(!SPIFFS.begin()) return; //Si no podemos inicializar SPIFFS...terminamos
  //Cargamos cada uno de los archivos de certificados y claves en memoria...

  File f=SPIFFS.open(caCertFile); //Intentamos abrir el achivo
  if(!f) return; //Si no se pudo abrir el archivo...terminamos
  caCert=new char[f.size()]; //Reservamos memoria para cargarlo
  f.read((byte*)caCert,f.size()); //Lo cargamos en memoria
  f.close(); //Cerramos el archivo

  f=SPIFFS.open(clientCertFile); //Intentamos abrir el achivo
  if(!f) return; //Si no se pudo abrir el archivo...terminamos
  clientCert=new char[f.size()]; //Reservamos memoria para cargarlo
  f.read((byte*)clientCert,f.size()); //Lo cargamos en memoria
  f.close(); //Cerramos el archivo

  f=SPIFFS.open(clientKeyFile); //Intentamos abrir el achivo
  if(!f) return; //Si no se pudo abrir el archivo...terminamos
  clientKey=new char[f.size()]; //Reservamos memoria para cargarlo
  f.read((byte*)clientKey,f.size()); //Lo cargamos en memoria
  f.close(); //Cerramos el archivo

  //Asignamos centificados y clave a conexión segura
  clientWifi.setCACert(caCert);
  clientWifi.setCertificate(clientCert);
  clientWifi.setPrivateKey(clientKey);
  client.setServer(mqtt_server,mqtt_port); //Config conexión de suscripción con broker
  client.setCallback(callback); //Indicamos la función de suscripción
  led.draw({0,10,0}); //Conectado!. Led en verde para indicarlo
  delay(1000); //Esperamos para que se pueda ver
  led.draw(); //Apagamos el led
}

void loop() {
  reconnect(); //Mantenemos abierta la conexión de suscripción con el broker
  client.loop(); //Comprobación de recepción de nuevos mensajes
  if(button.pressed()) { //Si se ha pulsado el botón...
    //Publicamos en el tópico test 0 o 1 (estado inverso al del led)
    client.publish("test",String(!ledStatus).c_str());
  }
  delay(1);
}
