/*
  Autor: Ramón Junquera
  Fecha: 20210113
  Tema: MQTT
  Objetivo: Interactuar con un broker Mosquitto sin seguridad
  Material: M5Stack Atom Lite
  Descripción:
    Publicación. Cada vez que se pulse el botón, se enviará el estado
    inverso del led al broker.
    Suscripción. Se aplicará el estado recibido al led.

    Si tras pulsar el botón, el led cambia de estado, es que funciona
    tanto la publicación como la suscripción.

    Si se implementa el mismo programa en varios dispositivos, todos
    cambiarán simultáneamente al estado inverso del que se pulsa.

    El broker no tendrá ningún tipo de seguridad.
    Las conexiones para suscripción y publicación serán anónimas.
    Es obligatorio indicar el identificador de dispositivo para conectar.
    El identificador de dispositivo debe ser único, sino interferirá
    con los demás.
    Para conseguirlo utilizamos la dirección MAC.

    Editamos el archivo de configuración:
sudo gedit /etc/mosquitto/mosquitto.conf
    El contenido del archivo mosquito.conf es:
pid_file /var/run/mosquitto.pid
persistence true
persistence_location /var/lib/mosquitto/
log_dest file /var/log/mosquitto/mosquitto.log
log_type all
include_dir /etc/mosquitto/conf.d
    Guardamos, salimos y reiniciamos el servicio:
sudo service mosquitto restart

    Comprobamos que funciona con:
    Suscripción:
mosquitto_sub -h 172.16.0.250 -t test
    Publicación:
mosquitto_pub -h 172.16.0.250 -t test -m Hola
*/

#include <Arduino.h>
#include <RoJoNeoPixelAtomLite.h> //Gestión de led NeoPixel en M5Stack Atom Lite
#include <RoJoSwitch.h> //Gestión de interruptor
#include <WiFi.h>
#include <PubSubClient.h> //Cliente de MQTT

//Contantes globales
const char* wifiSSID="xxx"; //Nompre del punto de acceso wifi
const char* wifiPassword="xxx"; //Contraseña del punto de acceso wifi
const char* mqtt_server="172.16.0.250"; //Dirección del broker
const uint16_t mqtt_port=1883; //Puerto estándar sin seguridad

//Variables globales
RoJoNeoPixelAtomLite led; //Objeto de gestión de leds NeoPixel
RoJoSwitch button(39); //Objeto de gestión de pulsador. Pin 39 en Atom
bool ledStatus=false; //Inicialmente apagado
WiFiClient espClient;
PubSubClient client(espClient);

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
  if(!client.connected()) { //Si no tenemos conexión con el broker...
    //Si conseguimos conectar...volvemos a hacer la suscripción
    if(client.connect(WiFi.macAddress().c_str())) client.subscribe("test");
  }
}

void setup() {
  led.begin(); //Inicialización de led NeoPixel en M5Stack Atom Lite o Echo
  led.draw({10,0,0}); //Comenzamos en rojo mientras conectamos a wifi
  WiFi.mode(WIFI_STA); //Conectado como cliente a un punto de acceso
  WiFi.begin(wifiSSID,wifiPassword); //Solicitamos conexión
  while(WiFi.status()!=WL_CONNECTED) delay(100); //Esperamos a que conecte
  client.setServer(mqtt_server,mqtt_port); //Config conexión a broker
  client.setCallback(callback); //Indicamos la función de suscripción
  led.draw({0,10,0}); //Conectado!. Led en verde para indicarlo
  delay(1000); //Esperamos para que se pueda ver
  led.draw(); //Apagamos el led
}

void loop() {
  reconnect(); //Comprobamos conexión al broker
  client.loop(); //Comprobación de recepción de nuevos mensajes
  if(button.pressed()) { //Si se ha pulsado el botón...
    //Publicamos en el tópico test 0 o 1 (estado inverso al del led)
    client.publish("test",String(!ledStatus).c_str());
  }
  delay(1);
}
