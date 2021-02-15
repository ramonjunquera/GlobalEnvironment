/*
  Autor: Ramón Junquera
  Fecha: 20190910
  Tema: Librería para gestión de bots en Telegram
  Objetivo: Control remoto a distancia
  Material: placa ESP-12E o Sonoff S20
  Descripción:
    Gestión remota de interruptor Sonoff S20 a través de Telegram.
  Desarrollo:
    La conexión a Sonoff S20 se realiza a través de un FTDI, cuya velocidad de
    transmisión no debería exceder de 115200 baudios.
    Esta velocidad es demasiado baja como para desarrollar y probar con fluidez.
    Por esta razón se utiliza un ESP12-E para desarrollo en entorno Platformio.
    Una vez finalizado y probado, se debe descomentar la variable global SONOFF
    para que tome los pines correctos del S20.
  Incompatibilidades:
    La transferencia del programa al S20 no se realiza correctamente desde Platformio.
    El fuente se debe compilar y transferir desde el IDE de Arduino.
    Una vez transferido, de debe desconectar la alimentación y volver a conectar.
  Notas:
    Basado en el ejemplo de suscripción segura.
    Se incluye la definición de constantes del led, interruptor y relay.
    El estado actual del relay se guarda siempre en un archivo.
    Cuando el sistema es reseteado, siempre recupera el último estado que tenía el realy y lo comunica.
    El interruptor permite cambiar localmente el estado del relay. Esta acción también es comunicada
    a todos los suscriptores.
  Configuración de memoria en Arduino IDE:
    ESP8266 Generic. 1M (64K SPIFFS)
 */

//Si se define la siguiente constante se compilará para Sonoff S20
//Si no se define, se hará para un ESP12-E
#define SONOFF

#include <Arduino.h>
#include <ESP8266WiFi.h> //Librería para gestión de wifi
#include <FS.h> //Librería para gestión de archivos
#include <RoJoTelegramBot.h> //Librería para gestión de bots de Telegram
#include <RoJoFileDictionary.h> //Librería de gestión de diccionarios en archivo

//Definición de constantes globales
const char ssid[]="xxx"; //Nombre del punto de acceso (SSID)
const char password[]="xxx"; //Contraseña
const String botToken="xxx"; //Token del bot
const uint32_t checkingGap=1000; //Tiempo de espera en milisegundos para comprobación de nuevos mensajes
String defaultKeyb="[[\"/on\",\"/off\"],[\"/status\",\"/users\"]]"; //Teclado por defecto

#ifdef SONOFF
  //Definición de constantes de pin para Sonoff S20
  const byte pinLed=13; //Pin del led verde
  const byte pinRelay=12; //Pin relay y led azul
  const byte pinSwitch=0; //Pin interruptor
#else
  //Definición de constantes de pin para ESP-12E
  //Como interruptor se utiliza el botón FLASH y como relay el segundo led integrado (el de antena)
  const byte pinLed=LED_BUILTIN; //Pin del led LED_BUILTIN=D0=16
  const byte pinRelay=D4; //Pin relay D4=2
  const byte pinSwitch=D3; //Pin interruptor D3=0
#endif


//Definición de variables globales
const uint32_t maxWait=30000; //Tiempo máximo de espera = 30 segundos
const float factorWait=1.2; //Factor de la progresión geométrica para calcular el siguiente tiempo de espera
String statusRelayFile="/relaystatus.txt"; //Nombre del archivo que guarda el estado del relé
bool statusRelay; //Estado del relé
bool switchPressed=false; //El interruptor ha sido pulsado?
RoJoTelegramBot bot; //Creamos el objeto de gestión del bot
uint32_t startWait=1+1/(factorWait-1); //Tiempo de espera inicial
uint32_t currentWait=startWait; //Tiempo de espera actual
uint16_t subscriptionCode=0; //Código de suscripción
String subscriptionCodeGenerator=""; //Nombre del generador del código de suscripción
uint32_t subscriptionCodeTimeout=0; //Hora de caducidad del código de suscripción
RoJoFileDictionary subscribers; //Creamos objeto de gestión de diccionarios

//Enviar el mensaje actual a todos los suscriptores
void broadcast(String text) {
  //Recorremos todos los suscriptores...
  for(uint16_t i=0;i<subscribers.count();i++)
    //...y les enviamos el mensaje
    bot.sendMessage(subscribers.key(i),text,defaultKeyb,true);
}

//Incluye al autor del mensaje del parámetro a la lista de suscriptores
void subscribe(TelegramMessage *msg) {
  //Mensaje a responder
  String txt="Suscripción completada";
  //Lo añadimos. Y si ya existía...cambiamos el mensaje
  if(subscribers.add((*msg).chat_id,(*msg).from_name)) txt="Ya estabas suscrito";
  //Informamos a todos
  broadcast((*msg).from_name + " es un nuevo suscriptor invitado por " + subscriptionCodeGenerator);
}

//Elimina un usuario de la lista de destinatarios
void unsubscribe(TelegramMessage *msg) {
  //Mensaje de respuesta
  String txt="No estás suscrito";
  //Eliminamos el item del diccionario
  subscribers.remove((*msg).chat_id);
  //Informamos a todos
  broadcast((*msg).from_name + " se ha dado de baja");
}

//Devuelve la lista de suscriptores actuales
String list() {
  //Cadena con al lista. Inicialmente vacía
  String answerList="";

  //Recorremos todos los suscriptores...
  for(uint16_t i=0;i<subscribers.count();i++) {
    //Si no es el primero...añadimos una coma
    if(answerList.length()) answerList+=",";
    //Añadimos el nombre
    answerList+=subscribers.value(i);
  }
  //Devolvemos la lista
  return answerList;
}

//Aplica el estado actual (statusRelay) al relé y lo guarda
void setStatus() {
  //Aplicamos estado al relé
  #ifdef SONOFF
    digitalWrite(pinRelay,statusRelay);
  #else
    digitalWrite(pinRelay,!statusRelay);
  #endif
  //Abrimos el archivo del estado del relé para escritura
  File f=SPIFFS.open(statusRelayFile,"w");
  //Guardamos el estado del led en el archivo
  f.write((int)statusRelay);
  //Cerramos el archivo
  f.close();
    
  //Apagamos el led
  //Lo hacemos aquí porque al pulsar el interruptor físico,
  //encendemos el led para informar que tenemos pendiente esta
  //operación.
  digitalWrite(pinLed,HIGH);
}

//Procesa todos los mensajes pendientes
void handleNewMessages() {
  //Creamos objeto de mensaje
  TelegramMessage msg;
  //Leemos el siguiente mensaje
  bot.getNextMessage(&msg);
  //Mientras haya mensaje...
  while(msg.text.length()) {
    Serial.println("Mensaje recibido: "+msg.text); //DEBUG
    //Reseteamos el contador de espera porque se ha recibido un mensaje
    currentWait=startWait;
    //Si no hay suscriptores...
    if(subscribers.count()==0) {
      //Si se trata del comando /start...
      if(msg.text=="/start") {
        //Componemos el mensaje a enviar en una sola cadena (es más rápido)
        String message = "RoJo Remote Switch\n";
        message += "/subscribe : Añadirse a la lista\n";
        //Enviamos el mensaje
        bot.sendMessage(msg.chat_id,message);
      }
      else if(msg.text=="/subscribe") {
        //El primer usuario en suscribirse
        subscriptionCodeGenerator=" él mismo";
        subscribe(&msg);
      }
    }
    else { //Hay algún suscriptor
      //Sólo atenderemos peticiones de suscriptores
      //Lo primero que necesitamos saber es si el autor del mensaje recibido es suscriptor

      //Si el autor es suscriptor...
      if(subscribers.containsKey(msg.chat_id)) {
        //Podemos hacerle caso
        //Si se trata del comando /start...
        if(msg.text=="/start") {
          //Componemos el mensaje a enviar en una sola cadena (es más rápido)
          String message = "RoJo Remote Switch\n";
          message += "/generate : Genera un código\n";
          message += "/subscribe code : Añadirse a la lista\n";
          message += "/unsubscribe : Borrarse de la lista\n";
          message += "/list : Suscriptores\n";
          message += "/on : Encender\n";
          message += "/off : Apagar\n";
          message += "/status : Estado actual\n";
          //Enviamos el mensaje
          bot.sendMessage(msg.chat_id,message,defaultKeyb,true);
        }
        else if(msg.text=="/users") {
          //Sólo presentaremos las opciones y el menú de selección
          String message = "RoJo Remote Switch\n";
          message += "/generate : Genera un código de suscripción\n";
          message += "/list : Mostrar la lista de suscriptores\n";
          message += "/unsubscribe : Borrarse de la lista\n";
          String keyb="[[\"/generate\",\"/list\",\"/unsubscribe\"]]";
          //Enviamos el mensaje con menú de selección
          bot.sendMessage(msg.chat_id,message,keyb,true);
        }
        else if(msg.text=="/generate") {
          //Genera un código de suscripción que será válido durante el próximo minuto
          subscriptionCode=random(65535)+1; //Entre 1 y 65535
          //Anotamos el nombre del generador
          subscriptionCodeGenerator=msg.from_name;
          //Anotamos la hora en la que caduca. Dentro de 60 segundos
          subscriptionCodeTimeout=millis()+60000;
          //Informamos del código
          bot.sendMessage(msg.chat_id,"Código: " + String(subscriptionCode),defaultKeyb,true);
        }
        else if(msg.text=="/unsubscribe") {
          //El usuario actual quiere eliminar la suscripción
          unsubscribe(&msg);
          //Informamos al usuario
          bot.sendMessage(msg.chat_id,"Suscripción borrada");
        }
        else if(msg.text=="/list") {
          //Solicitan la lista de suscriptores
          bot.sendMessage(msg.chat_id,"Suscriptores:"+list(),defaultKeyb,true);
        }
        else if(msg.text=="/on") {
          //Si el led ya está encendido...
          if(statusRelay) {
            //Informamos al usuario
            bot.sendMessage(msg.chat_id,"Ya estaba encendido",defaultKeyb,true);
          }
          else { //Si estaba apagado...
            //Cambiamos el estado a apagado
            statusRelay=true;
            //Aplicamos el estado
            setStatus();
            //Informamos a todos
            broadcast(msg.from_name + " ha encendido");
          }
        }
        else if(msg.text=="/off") {
          //Si ya estaba apagado...
          if(!statusRelay) {
            //Informamos al usuario
            bot.sendMessage(msg.chat_id,"Ya estaba apagado",defaultKeyb,true);
          }
          else { //Si estaba encendido...
            //Cambiamos el estado a apagado
            statusRelay=false;
            //Aplicamos el estado
            setStatus();
            //Informamos a todos
            broadcast(msg.from_name + " ha apagado");
          }
        }
        else if(msg.text=="/status") {
          //Obtenemos el texto del estado
          String statusText = statusRelay?"Encendido":"Apagado";
          //Enviamos mensaje informado del estado
          bot.sendMessage(msg.chat_id,statusText,defaultKeyb,true);
        }
        else { //Si no es ningún comando reconocido...
          //...enviamos el mensaje a todos los suscriptores
          broadcast(msg.from_name + " dijo: " + msg.text);
        }
      }
      else { //Al autor no es suscriptor
        if(msg.text.substring(0,11)=="/subscribe ") {
          //Si parece que alguien quiere suscribirse...
          //Si tenemos algún código válido...
          if(subscriptionCode) {
            //Si el código indicado en el correcto...
            if(msg.text.substring(11)==String(subscriptionCode)) {
              //Incluimos el nuevo usuario como suscriptor
              subscribe(&msg);
            }
            //Desactivamos el código. Ya ha sido utilizado
            subscriptionCode=0;
          }
          else { //El código indicado no es el correcto
            //Informamos
            bot.sendMessage(msg.chat_id,"Código incorrecto");
          }
        }
      }
    }
    //Refrescamos los procesos de fondo
    yield();
    //Hemos terminado de procesar el mensaje actual
    //Leemos el siguiente
    bot.getNextMessage(&msg);
  }
}

//Lee el estado desde el archivo y lo aplica
void readStatus() {
  //Abrimos el archivo del estado del relé como sólo lectura
  File f=SPIFFS.open(statusRelayFile,"r");
  //Si no se pudo abrir...
  if(!f) {
    //..se crea
    f=SPIFFS.open(statusRelayFile,"w");
    //Se guarda el estado "apagado", por defecto
    f.write(0);
    //Se cierra
    f.close();
    //Se vuelve a abrir como sólo lectura
    f=SPIFFS.open(statusRelayFile,"w");
  }
  //Tenemos el archivo abierto
  
  //Leemos el estado guardado
  //Sólo puede ser 0=off o 1=on
  statusRelay=f.read();
  //Cerramos el archivo
  f.close();
  //Aplicamos el estado leido
  setStatus();
}

//Función de gestión de interrupciones del interruptor
//Necesario el parámetro ICACHE_RAM_ATTR en las últimas versiones las
//librerías de ESP8266 para obligar a la función a la que llama la 
//interrupción a que se cargue en IRAM
void ICACHE_RAM_ATTR interruptSwitch() {
  //Si no se había anotado la pulsación...
  if(!switchPressed) {
    //Lo anotamos ahora
    switchPressed=true;
    //Y para que se de cuenta, encendemos el led (estados intercambiados)
    digitalWrite(pinLed,LOW);
  }
}

void setup() {
  //Serial.begin(115200); //DEBUG
  //Inicializamos el bot
  bot.begin(botToken);
  //Configuramos el pin del led como salida
  pinMode(pinLed,OUTPUT);
  //Configuramos el pin del relé como salida
  pinMode(pinRelay,OUTPUT);
  //Configuramos el pin del interruptor como entrada con las resistencias de pullup activas
  pinMode(pinSwitch,INPUT_PULLUP);
  //Fijamos el modo de conexión a un punto de acceso
  WiFi.mode(WIFI_STA);
  //Forzamos a su desconexión (por si estaba conectado anteriormente)
  WiFi.disconnect();
  //Esperamos un momento
  delay(100);
  //Pedimos conexión al punto de acceso
  WiFi.begin(ssid,password);
  //Mientras no estemos conectados...
  while (WiFi.status() != WL_CONNECTED) {
    //Cambiaremos el estado del led
    digitalWrite(pinLed,!digitalRead(pinLed));
    delay(100);
  }
  //Hemos conseguido conectar

  //Inicializamos el diccionario de suscriptores
  subscribers.begin("/subscribers.txt");
  //Leemos el estado desde el archivo y lo aplicamos
  readStatus();
  //Informamos del reinicio
  broadcast("Dispositivo reiniciado");
  //Inicializamos la semilla de números aleatorios
  randomSeed(millis());
  //Activamos las interrupciones para el pin del interruptor sólo para pulsaciones
  attachInterrupt(pinSwitch,interruptSwitch,FALLING);
  Serial.println("setup() end");
}

void loop() {
  //Procesa todos los nuevos mensajes
  handleNewMessages();
  //Calculamos el tiempo máximo de espera
  uint32_t maxTime=millis()+currentWait;
  //Mientras no lleguemos al máximo tiempo de espera...
  while(millis()<maxTime) {
    //Si se ha pulsado el interruptor...
    if(switchPressed) {
      //Cambiamos el estado actual
      statusRelay=!statusRelay;
      //Aplicamos el estado actual al led y al relé
      setStatus();
      //Informamos
      String s = statusRelay?"encendido":"apagado";
      broadcast("Se ha " + s + " con el interruptor");
      //Reseteamos el contador de espera porque ha habido un cambio
      currentWait=startWait;
      //Quitamos el flag de interruptor pulsado
      switchPressed=false;
    }
    //Si tenemos un código de suscripción válido...
    if(subscriptionCode) {
      //Si ha caducado...
      if(millis()>subscriptionCodeTimeout) {
        //...lo eliminamos
        subscriptionCode=0;
      }
    }
    //Refrescamos los procesos de fondo
    //Si no lo hacemos, se reseteará periódicamente debido a WDT
    yield();
  }
  //Aumentamos el tiempo de espera para el siguiente ciclo
  currentWait*=factorWait;
  //Si sobrepasamos el máximo...lo limitamos
  if(currentWait>maxWait) currentWait=maxWait;
}