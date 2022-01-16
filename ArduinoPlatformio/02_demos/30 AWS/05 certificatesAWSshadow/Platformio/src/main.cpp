/*
  Autor: Ramón Junquera
  Fecha: 20210118
  Tema: MQTT
  Objetivo: Interactuar con sombra de AWS IoT
  Material: M5Stack Atom Lite
  Descripción:
    La conexión al entorno de AWS IoT siemre se realiza mediante certificados.
    Los certificados se encuentran en la carpeta data y se deben subir a SPIFFS.
    Utilizaremos el mismo usuario tanto para suscripciones como publicaciones.

    El funcionamiento es simple. Cuando se pulsa el botón, el led cambia de estado.

    Sólo utilizaremos la sombra clásica.
    La sombra sólo tiene un item dentro del estado llamado led.
    Es de tipo booleano.

    Cuando se pulsa el botón se realiza una publicación solicitando el cambio de
    estado del led, al estado opuesto al actual.
    Tópico ./shadow/update e item /state/desired

    Cuando finaliza la inicialización, queda suscrito a ./shadow/update/delta
    para estar informado de discrepancias entre las solicitudes (desired) y
    el estado real (reported).
    Los mensajes recibidos se procesan, obteniendo el estado del led, aplicándolo
    e informando del nuevo estado en la sección reported.

    Por lo tanto, si pulsamos el botón y el estado del led cambia, es que todo
    está funcionando correctamente, porque la pulsación y el estado del led
    están relacionados a través de la sombra.

    La conexión con el broker debe mantenerse abierta para poder recibir los
    mensajes de los tópicos suscritos.
    Nos preocuparemos de que así sea comprobandola continuamente dentro de la
    función loop().

    Al arrancar, el led se queda en rojo mientras completas todas las tareas de
    inicialización. Cuando termina lo pone en verde durante un segundo para
    indicar que está preparado para comenzar.

    Al arrancar desconocemos el estado que debemos aplicar al led.
    Intentamos conseguirlo a través del tópico get.
    Tramitamos una publicación a ./shadow/get con la solicitud de estado y
    la recibimos por una suscripción a ./shadow/get/accepted.
    De esta manera, aunque haya un corte de suministro electrico, al arrancar
    de nuevo, recuperaremos el estado correcto.

    Si mantenemos el botón pulsado al arrancar, el led se volverá azul en
    vez de rojo, para indicar que arranca en modo reset.
    Este arranque borra la sombra actual con todos sus datos mediante una
    publicación al tópico ./shadow/delete y crea una nueva sólo con
    el estado actual con el led apagado.

    Es importante recordar que podemos realizar suscripciones a distintos
    tópicos. Todas ellas quedarán abiertas y se recepcionarán los mensajes
    de todos ellos. Pero sólo podemos definir una única función de gestión
    (callback).
    Si no estamos interesados en tener más de una suscripción abierta,
    debemos cancelar las que no queremos.
    En el ejemplo, al arrancar normal, se abre suscripción a get/acepted
    para obtener el estado de la sombra. Más adelante no suscribimos
    a update/delta para conocer las actualizaciones, pero ya no estamos
    interesados en la suscripción anterior. Por eso la cancelamos.

    Si se implementa el mismo programa en varios dispositivos, todos
    cambiarán simultáneamente al estado inverso del que se pulsa.

    Es obligatorio indicar el identificador de dispositivo para conectar.
    El identificador de dispositivo debe ser único, sino interferirá
    con los demás. Utilizaremos la dirección MAC como identificador.
*/

#include <Arduino.h>
#include <RoJoNeoPixelAtomLite.h> //Gestión de led NeoPixel en M5Stack Atom Lite
#include <RoJoSwitch.h> //Gestión de interruptor
#include <WiFiClientSecure.h> //Cliente de conexiones seguras
#include <PubSubClient.h> //Cliente de MQTT
#include <SPIFFS.h> //Gestión de archivos
#include <cJSON.h> //Gestión de archivo de texto en formato JSON

//Contantes globales
const char* wifiSSID="xxx"; //Nompre del punto de acceso wifi
const char* wifiPassword="xxx"; //Contraseña del punto de acceso wifi
const char* mqtt_server="apj0acah6jgct-ats.iot.us-east-1.amazonaws.com"; //Dirección del broker
const uint16_t mqtt_port=8883; //Puerto estándar con seguridad
const String caCertFile="/AmazonRootCA1.pem";
const String clientCertFile="/Atom-certificate.pem.crt";
const String clientKeyFile="/Atom-private.pem.key";
const String falsetrue[]={"false","true"}; //Para componer publicaciones

//Variables globales
RoJoNeoPixelAtomLite led; //Objeto de gestión de leds NeoPixel
RoJoSwitch button(39); //Objeto de gestión de pulsador. Pin 39 en Atom
bool ledStatus=false; //Inicialmente apagado
WiFiClientSecure clientWifi; //Conexión wifi
PubSubClient client(clientWifi); //Conexión al broker
char *caCert,*clientCert,*clientKey; //Punteros
bool subDelta=true; //Estamos suscritos al tópico update/delta? (o a get/accepted)

//Aplica el estado de ledStatus al led
void setled() {
  if(ledStatus) {
    led.draw({30,30,30}); led.draw({30,30,30});
  } else {
    led.draw(); led.draw();
  }
}

//Función de gestión de mensajes de suscripción
void callback(char* topic,byte* payload,unsigned int length) {
  cJSON *msg=cJSON_Parse((char*)payload); //Cremos objeto de gestión JSON
  if(subDelta) { //Si este mensaje es debido a la suscripción a update/delta...
    //Obtenemos el valor de /state/led
    ledStatus=cJSON_GetObjectItem(cJSON_GetObjectItem(msg,"state"),"led")->valueint;
    setled(); //Directamente aplicamos estado de ledStatus al led
  } else { //Si el mensaje es debido a la suscripción a get/accepted...
    //Aun estamos inicializando. Acabamos de arrancar.
    //Hemos recibido el estado de la sombra y debemos aplicarlo al led
    //Tomaremos el estado de una de las siguientes secciones (en este orden):
    //- state/delta : por si hay cambios pendientes
    //- state/desired : por si nunca nunca se publicó un estado real (reported)
    //- state/reported : por si sólo tenemos el estado real (reported)
    if(cJSON_HasObjectItem(msg,"state")) { //Si tiene sección state...
      cJSON *stateItem=cJSON_GetObjectItem(msg,"state"); //Obtenemos el item state
      if(cJSON_HasObjectItem(stateItem,"delta")) { //Si tiene sección state/delta...
        //Obtenemos el valor de /state/delta/led
        ledStatus=cJSON_GetObjectItem(cJSON_GetObjectItem(stateItem,"delta"),"led")->valueint;
      } else if(cJSON_HasObjectItem(stateItem,"desired")) { //Si tiene sección state/desired...
        //Obtenemos el valor de /state/desired/led
        ledStatus=cJSON_GetObjectItem(cJSON_GetObjectItem(stateItem,"desired"),"led")->valueint;
      } else if(cJSON_HasObjectItem(stateItem,"reported")) { //Si tiene sección state/reported...
        //Obtenemos el valor de /state/reported/led
        ledStatus=cJSON_GetObjectItem(cJSON_GetObjectItem(stateItem,"reported"),"led")->valueint;
      }
      //Hemos recibido el estado de la sombra y hemos intentado extraer el estado del led
      //de la sección correcta a la variable ledStatus.
      //Si no lo hemos conseguido (porque el mensaje tiene un formato desconocido), nos
      //quedaremos con el valor que se inicializa por defecto (false).
      client.unsubscribe("$aws/things/Atom_obj/shadow/get/accepted"); //Cancelamos la suscripción de get/accepted
      subDelta=true; //A partir de ahora los mensajes recibidos procederán de update/delta
      client.subscribe("$aws/things/Atom_obj/shadow/update/delta"); //Suscripción a update/delta
    }
  }
  cJSON_Delete(msg); //Liberamos memoria de mensaje JSON
  //Ya sea porque hemos recibido un mensaje de update/delta o get/accepted, se ha aplicado
  //un estado al led y debemos informar.
  //Publicamos el estado actual del led como reported
  client.publish("$aws/things/Atom_obj/shadow/update",String("{\"state\":{\"reported\":{\"led\":"+falsetrue[ledStatus]+"}}}").c_str());
}

//Intenta conectar con broker
void reconnect() {
  if(client.connected()) return; //Si ya estamos conectados...hemos terminado
  if(client.connect(WiFi.macAddress().c_str())) //Si conseguimos conectar...
    //...intentamos suscribirnos al tópico update/delta
    client.subscribe("$aws/things/Atom_obj/shadow/update/delta"); 
}

void setup() {
  //Anotamos si estamos arrancando con el botón pulsado
  //Si es así, después resetearemos la sombra
  bool reset=button.pressing();
  led.begin(); //Inicialización de led NeoPixel en M5Stack Atom Lite o Echo
  if(reset) led.draw({0,0,10}); //Si estamos reseteando, la luz será azul
  else led.draw({10,0,0}); //Si no estamos reseteando, la luz será roja
  WiFi.mode(WIFI_STA); //Conectado como cliente a un punto de acceso
  WiFi.begin(wifiSSID,wifiPassword); //Solicitamos conexión
  while(WiFi.status()!=WL_CONNECTED) delay(100); //Esperamos a conectar
  if(!SPIFFS.begin()) return; //Si no podemos inicializar SPIFFS...terminamos

  //Cargamos cada uno de los archivos de certificados y claves en memoria...
  File f=SPIFFS.open(caCertFile); //Intentamos abrir el achivo
  if(!f) return; //Si no se pudo abrir el archivo...terminamos dejando el led encendido
  caCert=new char[f.size()]; //Reservamos memoria para cargarlo
  f.read((byte*)caCert,f.size()); //Lo cargamos en memoria
  f.close(); //Cerramos el archivo

  f=SPIFFS.open(clientCertFile); //Intentamos abrir el achivo
  if(!f) return; //Si no se pudo abrir el archivo...terminamos dejando el led encendido
  clientCert=new char[f.size()]; //Reservamos memoria para cargarlo
  f.read((byte*)clientCert,f.size()); //Lo cargamos en memoria
  f.close(); //Cerramos el archivo

  f=SPIFFS.open(clientKeyFile); //Intentamos abrir el achivo
  if(!f) return; //Si no se pudo abrir el archivo...terminamos dejando el led encendido
  clientKey=new char[f.size()]; //Reservamos memoria para cargarlo
  f.read((byte*)clientKey,f.size()); //Lo cargamos en memoria
  f.close(); //Cerramos el archivo

  clientWifi.setCACert(caCert); //Asignamos certificado de entidad certificadora
  clientWifi.setCertificate(clientCert); //Asignamos certificado de cliente
  clientWifi.setPrivateKey(clientKey); //Asignamos clave privada de cliente
  client.setServer(mqtt_server,mqtt_port); //Configuramos conexión de suscripción con broker
  client.setCallback(callback); //Indicamos la función de gestión de mensajes de suscripción
  //Intentamos conectar con el broker hasta conseguirlo
  while(!client.connect(WiFi.macAddress().c_str())) delay(1);
  if(reset) { //Si estamos reseteando...
    client.publish("$aws/things/Atom_obj/shadow/delete",String("").c_str()); //Borramos la sombra
    //Informamos del estado actual: apagado
    client.publish("$aws/things/Atom_obj/shadow/update",String("{\"state\":{\"reported\":{\"led\":false}}}").c_str());
    client.subscribe("$aws/things/Atom_obj/shadow/update/delta"); //Suscripción a update/delta
  } else { //Si no estamos reseteando (inicio normal)...
    //Debemos conseguir el estado actual de la sombra
    subDelta=false; //La función de callback procesará mensajes procedentes de get/accepted
    client.subscribe("$aws/things/Atom_obj/shadow/get/accepted"); //Suscripción a get/accepted
    while(!subDelta) { //Mientras no se haya procesado el estado de la sombra...
      //Publicamos la solicitud de estado de la sombra
      client.publish("$aws/things/Atom_obj/shadow/get",String("").c_str());
      uint32_t maxTime=millis()+1000; //Esperaremos un máximo de un segundo
      //Mientras no se haya procesado el estado de la sombra y no se haya terminado el tiempo...
      while(!subDelta && millis()<maxTime) {
        client.loop(); //Comprobamos si tenemos nuevos mensajes de suscripciones
        delay(1);
      }
    }
    //Hemos terminado de procesar el estado de la sombra, porque subDelta a cambiado a true.
    //La gestión se ha realizado en la función de callback
  }
  led.draw({0,10,0}); //Inicialización finalizada correctamente. Led en verde para indicarlo
  delay(1000); //Esperamos para que se pueda ver
  setled(); //Aplicamos estado de ledStatus al led
}

void loop() {
  client.loop(); //Comprobamos si tenemos nuevos mensajes de suscripciones
  if(button.pressed()) { //Si se ha pulsado el botón...
    //Publicamos la solicitud de cambio de estado con el opuesto del estado actual
    client.publish("$aws/things/Atom_obj/shadow/update",String("{\"state\":{\"desired\":{\"led\":"+falsetrue[!ledStatus]+"}}}").c_str());
  }
  reconnect(); //Mantenemos abierta la conexión con el broker
  delay(1);
}
