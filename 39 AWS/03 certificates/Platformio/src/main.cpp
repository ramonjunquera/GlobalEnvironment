/*
  Autor: Ramón Junquera
  Fecha: 20210114
  Tema: MQTT
  Objetivo: Interactuar con un broker Mosquitto con identificación por certificados
  Material: M5Stack Atom Lite
  Descripción:
    Publicación. Cada vez que se pulse el botón, se enviará el estado
    inverso del led al broker.
    Suscripción. Se aplicará el estado recibido al led.

    Utilizaremos usuarios distintos para la suscripción y la publicación.
    Como los certificados se aplican a la conexión, necesitaremos dos conexiones
    distintas. Una para cada acción.

    La conexión de suscripción debe mantenerse abierta siempre para poder recibir
    mensajes. Nos preocuparemos de que así sea comprobandola continuamente.
    Para la conexión de publicación no. Conectaremos sólo cuando vayamos a publicar.
    Después la volveremos a cerrar.

    Si tras pulsar el botón, el led cambia de estado, es que funciona
    tanto la publicación como la suscripción.

    La respuesta del led es mucho más lenta que en una conexión sin cifrar. Puede
    tardar hasta 5 segundos. Es normal. La mayor parte de ese tiempo está
    dedicado a establecer la conexión para la publicación.
    Si también mantenemos continuamente abierta la conexión de publicación el tiempo
    se reduce prácticamente a cero.

    Sólo recordar que podemos utilizar la misma conexión tanto para suscribirnos
    como para publicar (que es lo habitual).

    Si se implementa el mismo programa en varios dispositivos, todos
    cambiarán simultáneamente al estado inverso del que se pulsa.

    El broker sólo aceptará conexiones con certificado.
    Es obligatorio indicar el identificador de dispositivo para conectar.
    El identificador de dispositivo debe ser único, sino interferirá
    con los demás.
    Como realizamos dos conexiones utilizaremos el prefijo sub/pub + dirección MAC.

    Editamos el archivo de configuración:
sudo gedit /etc/mosquitto/mosquitto.conf
    El contenido del archivo mosquito.conf es:
pid_file /var/run/mosquitto.pid
persistence true
persistence_location /var/lib/mosquitto/
log_dest file /var/log/mosquitto/mosquitto.log
log_type all
include_dir /etc/mosquitto/conf.d
cafile /etc/mosquitto/ca_certificates/ca.crt
keyfile /etc/mosquitto/certs/server.key
certfile /etc/mosquitto/certs/server.crt
tls_version tlsv1.2
port 8883
require_certificate true
    Guardamos, salimos y reiniciamos el servicio:
sudo service mosquitto restart
    Lo único que añadimos respecto a una configuración limpia son:
    - La ubiicación del archivo de certificado de la entidad certificadora
    - La ubicación del archivo de claves del servidor
    - La ubicación del archivo de certificado del servidor
    - La versión de TLS
    - El puerto utilizado
    - La obligación de utilizar certificados para conectar
    No existe archivo de usuarios ni reglas, por lo tanto no hay restricciones en tópicos.

    Comprobamos que funciona con:
    Suscripción:
mosquitto_sub --cafile ~/Downloads/ca.crt --cert ~/Downloads/client2.crt --key ~/Downloads/client2.key -h 172.16.0.250 -t test
    Publicación:
mosquitto_pub --cafile ~/Downloads/ca.crt --cert ~/Downloads/client4.crt --key ~/Downloads/client4.key -h 172.16.0.250 -t test -m Hola
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
const char* mqtt_server="172.16.0.250"; //Dirección del broker
const uint16_t mqtt_port=8883; //Puerto estándar con seguridad
const String caCertFile="/ca.crt";
const String client2CertFile="/client2.crt";
const String client2KeyFile="/client2.key";
const String client4CertFile="/client4.crt";
const String client4KeyFile="/client4.key";

//Variables globales
RoJoNeoPixelAtomLite led; //Objeto de gestión de leds NeoPixel
RoJoSwitch button(39); //Objeto de gestión de pulsador. Pin 39 en Atom
bool ledStatus=false; //Inicialmente apagado
WiFiClientSecure clientSubWifi,clientPubWifi; //Conexiones wifi para suscripción y publicación
PubSubClient clientSub(clientSubWifi),clientPub(clientPubWifi); //Conexiones al broker
char *caCert,*client2Cert,*client2Key,*client4Cert,*client4Key; //Punteros

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
  if(!clientSub.connected()) { //Si no tenemos conexión de suscripción con el broker...
    //Si conseguimos conectar...volvemos a hacer la suscripción
    if(clientSub.connect(("sub"+WiFi.macAddress()).c_str())) clientSub.subscribe("test");
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

  f=SPIFFS.open(client2CertFile); //Intentamos abrir el achivo
  if(!f) return; //Si no se pudo abrir el archivo...terminamos
  client2Cert=new char[f.size()]; //Reservamos memoria para cargarlo
  f.read((byte*)client2Cert,f.size()); //Lo cargamos en memoria
  f.close(); //Cerramos el archivo

  f=SPIFFS.open(client2KeyFile); //Intentamos abrir el achivo
  if(!f) return; //Si no se pudo abrir el archivo...terminamos
  client2Key=new char[f.size()]; //Reservamos memoria para cargarlo
  f.read((byte*)client2Key,f.size()); //Lo cargamos en memoria
  f.close(); //Cerramos el archivo

  f=SPIFFS.open(client4CertFile); //Intentamos abrir el achivo
  if(!f) return; //Si no se pudo abrir el archivo...terminamos
  client4Cert=new char[f.size()]; //Reservamos memoria para cargarlo
  f.read((byte*)client4Cert,f.size()); //Lo cargamos en memoria
  f.close(); //Cerramos el archivo

  f=SPIFFS.open(client4KeyFile); //Intentamos abrir el achivo
  if(!f) return; //Si no se pudo abrir el archivo...terminamos
  client4Key=new char[f.size()]; //Reservamos memoria para cargarlo
  f.read((byte*)client4Key,f.size()); //Lo cargamos en memoria
  f.close(); //Cerramos el archivo

  //Asignamos centificados y clave a conexión de subscripción
  clientSubWifi.setCACert(caCert);
  clientSubWifi.setCertificate(client2Cert);
  clientSubWifi.setPrivateKey(client2Key);
  //Asignamos centificados y clave a conexión de publicación
  clientPubWifi.setCACert(caCert);
  clientPubWifi.setCertificate(client4Cert);
  clientPubWifi.setPrivateKey(client4Key);
  clientSub.setServer(mqtt_server,mqtt_port); //Config conexión de suscripción con broker
  clientPub.setServer(mqtt_server,mqtt_port); //Config conexión de publicación con broker
  clientSub.setCallback(callback); //Indicamos la función de suscripción
  led.draw({0,10,0}); //Conectado!. Led en verde para indicarlo
  delay(1000); //Esperamos para que se pueda ver
  led.draw(); //Apagamos el led
}

void loop() {
  reconnect(); //Mantenemos abierta la conexión de suscripción con el broker
  clientSub.loop(); //Comprobación de recepción de nuevos mensajes
  if(button.pressed()) { //Si se ha pulsado el botón...
    //Si conseguimos abrir conexión para publicación...
    if(clientPub.connect(("pub"+WiFi.macAddress()).c_str())) {
      //Publicamos en el tópico test 0 o 1 (estado inverso al del led)
      clientPub.publish("test",String(!ledStatus).c_str());
      clientPub.disconnect(); //Y desconectamos
      //No es necesario mantener abierta la conexión de publicación continuamente
    }
  }
  delay(1);
}
