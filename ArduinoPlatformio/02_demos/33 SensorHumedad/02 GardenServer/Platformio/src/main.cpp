/*
  Tema: Servidor de jardín
  Fecha: 20221118
  Autor: Ramón Junquera

  Descripción:
  Dispositivo alimentado por batería externa y recargado por placa solar, que mide la humedad de la tierra
  y envía una alerta por Telegram cuando esté por debajo de un límite predefinido.
  También es capaz de medir la carga de la batería y avisar cuando esté baja.
  La mayor parte de los parámetros también pueden ser configurados por Telegram.
  Permite actualizaciones OTA.

  Dispositivo:
  - Para facilitar el desarrollo, se da de alta como dispositivo lolin32
  - El disposivo wemosbat funciona correctamente pero es bastante frágil. Se rompe con facilidad antes un
    golpe o forzado del conector. También tiene facilidad a oxidarse ante cualquier humedad. Para este
    proyecto se imprescindible una impermeabilización muy buena.

  Debug:
  - Se han dejado algunos mensajes informativos para debug cuando se tiene acceso al puerto serie.
*/

#include <Arduino.h>
#include <ArduinoOTA.h> //Ya incluye <WiFi.h/ESP8266WiFi.h> & <ESPmDNS.h>
#include <RoJoTelegramBot.h> //Librería para gestión de bots de Telegram
#include <RoJoFileDictionary.h> //Librería de gestión de diccionarios en archivo
#include <driver/rtc_io.h> //Para rtc_gpio_isolate

//Definición de constantes globales
const char ssid[]="xxx"; //Nombre del punto de acceso (SSID)
const char password[]="xxx"; //Contraseña
const String botToken="1234567890:AABBCCDD-EE-FFGGHHIIJJKKLLMMNNOOPPQ"; //Token del bot RoJoGardenBot
const byte pinBatteryADC=35; //Ya integrado
const byte pinHumidityADC=32;
const byte pinGND=25; //Pin GND del sensor de humedad
const byte pinVCC=33; //Pin VCC del sensor de humedad
const char *hostname="ESP32Garden";
const char *passwordOTA="Garden5";

//Definición de variables globales
RoJoTelegramBot bot; //Objeto de gestión del bot
RoJoFileDictionary subscribers; //Objeto de gestión de suscriptores
RoJoFileDictionary config; //Diccionario con parámetros de configuración

//Enviar el mensaje actual a todos los suscriptores
void broadcast(String text) {
  Serial.println("Envío de mensaje general: "+text);
  for(uint16_t i=0;i<subscribers.count();i++) { //Recorremos todos los suscriptores...
    bot.sendMessage(subscribers.key(i),text); //...y le enviamos el mensaje
  }
}

//Incluye al autor del mensaje del parámetro a la lista de suscriptores
void subscribe(TelegramMessage *msg) {
  subscribers.add(msg->chat_id,msg->from_name); //Lo añadimos
  broadcast(msg->from_name + " es un nuevo suscriptor"); //Informamos a todos
}

//Elimina un usuario de la lista de destinatarios
void unsubscribe(TelegramMessage *msg) {
  subscribers.remove(msg->chat_id); //Eliminamos el item del diccionario
  broadcast(msg->from_name + " se ha dado de baja"); //Informamos a todos
}

//Devuelve la lista de suscriptores actuales
String list() {
  String answerList=""; //Cadena con la lista. Inicialmente vacía
  for(uint16_t i=0;i<subscribers.count();i++) { //Recorremos todos los suscriptores...
    if(answerList.length()) answerList+=","; //Si no es el primero...añadimos una coma
    answerList+=subscribers.value(i); //Añadimos el nombre
  }
  return answerList; //Devolvemos la lista
}

//Devuelve el promedio del valor ADC de un pin
uint16_t getADC(byte pin) {
  uint16_t analog=0;
  for(byte i=16;i>0;i--) {
    analog+=analogRead(pin);
    delay(1);
  }
  return analog/16;
}

//Transforma valor ADC de batería a %
byte batPercent(uint16_t ADCvalue) {
  //ADC%=100*(ADC-Vmin)/(Vmax-Vmin)
  //El rango detectado en la práctica es: [2329,2437]
  //Suponemos que 2437 corresponde a 5V
  //Suponemos que funciona con una alimentación mínima de 3.3V -> 3.3*2437/5=1608
  return 100*(ADCvalue-1608)/(2437-1608);
}

//Devuelve el % de humedad
uint16_t getHumidityPercent(byte pin) {
  const uint16_t fullWet=896; //Valor ADC para humedad máxima
  const uint16_t fullDry=2742; //Valor ADC para todo seco
  uint16_t analog=getADC(pin);
  //Calculamos el % de humedad
  analog=100-100*(analog-fullWet)/(fullDry-fullWet);
  if(analog>200) analog=0;
  else if(analog>100) analog=100;
  return analog;
}

//Procesa todos los mensajes pendientes
void handleNewMessages() {
  TelegramMessage msg; //Creamos estructura de mensaje
  bot.getNextMessage(&msg); //Obtenemos el siguiente mensaje
  while(msg.text.length()) { //Mientras haya un mensaje...
    Serial.println("Mensaje recibido: "+msg.text);
    if(subscribers.count()==0) { //Si no hay suscriptores...
      if(msg.text=="/start") { //Si se trata del comando /start...
        String message = "RoJo Telegram Garden\n";
        message += "/subscribe : Añadirse a la lista\n";
        bot.sendMessage(msg.chat_id,message); //Enviamos el mensaje
      } else if(msg.text=="/subscribe") { //Si quire suscribirse...
        subscribe(&msg); //...ya tenemos la primera suscripción
      }
    } else { //Hay algún suscriptor
      //Sólo atenderemos peticiones de suscriptores
      //Lo primero que necesitamos saber es si el autor del mensaje recibido es suscriptor
      if(subscribers.containsKey(msg.chat_id)) { //Si el autor es suscriptor...
        //Podemos hacerle caso
        if(msg.text=="/start") {
          //Componemos el mensaje a enviar en una sola cadena (es más rápido)
          String message = "RoJo Telegram Garden\n";
          message += "/generate : Genera un código de suscripción\n";
          message += "/unsubscribe : Borrarse de la lista\n";
          message += "/list : Mostrar la lista de suscriptores\n";
          message += "/setperiod x : Nuevo periodo en segundos\n";
          message += "/getperiod : Muestra el periodo actual en segundos\n";
          message += "/setbatalarm x : Límite mínimo de % bateria para alarma\n";
          message += "/getbatalarm : Mostrar nivel de alarma de batería\n";
          message += "/getbat : Mostrar el nivel de batería actual\n";
          message += "/sethumalarm x : Límite mínimo de % humedad para alarma\n";
          message += "/gethumalarm : Mostrar nivel de alarma de humedad\n";
          message += "/gethum : Mostrar el nivel de humedad actual\n";
          message += "/setotaon : Activa modo OTA\n";
          message += "/setotaoff : Desactiva modo OTA\n";
          message += "/getip : Muestra dirección ip\n";
          bot.sendMessage(msg.chat_id,message); //Enviamos el mensaje
        } else if(msg.text=="/generate") {
          //Genera un código de suscripción
          String subscriptionCode=String(random(65535)+1); //Generámos código entre 1 y 65535
          config.add("code",subscriptionCode); //Guardamos el código de suscripción
          bot.sendMessage(msg.chat_id,"Código: "+subscriptionCode); //Informamos del código
        } else if(msg.text=="/unsubscribe") {
          //El usuario actual quiere eliminar la suscripción
          unsubscribe(&msg);
          //Informamos al usuario
          bot.sendMessage(msg.chat_id,"Suscripción borrada");
        } else if(msg.text=="/list") { //Solicitan la lista de suscriptores
          bot.sendMessage(msg.chat_id,"Suscriptores: "+list());
        } else if(msg.text.startsWith("/setperiod ")) { //Solicitan nuevo periodo
          String period=String(msg.text.substring(11).toInt()); //Extraemos el valor del periodo
          config.add("period",period); //Guardamos el nuevo periodo
          broadcast(msg.from_name+" cambia el periodo a "+period+"s");
        } else if(msg.text=="/getperiod") { //Solicitan mostrar el periodo
          bot.sendMessage(msg.chat_id,"Period: "+config.value("period")+"s");
        } else if(msg.text.startsWith("/setbatalarm ")) { //Solicitan nuevo límite de alarma para batería
          String batAlarm=String(msg.text.substring(13).toInt()); //Extraemos el valor del % de batería
          config.add("batAlarm",batAlarm); //Lo guardamos
          broadcast(msg.from_name+" cambia mínimo de batería para alarma a "+batAlarm+"%");
        } else if(msg.text.startsWith("/sethumalarm ")) { //Solicitan nuevo límite de alarma para humedad
          String humAlarm=String(msg.text.substring(13).toInt()); //Extraemos el valor del % de humedad
          config.add("humAlarm",humAlarm); //Lo guardamos
          broadcast(msg.from_name+" cambia mínimo de humedad para alarma a "+humAlarm+"%");
        } else if(msg.text=="/getbat") { //Solicitan el % de batería
          bot.sendMessage(msg.chat_id,"Battery: "+String((int)batPercent(getADC(pinBatteryADC)))+"%");
        } else if(msg.text=="/gethum") { //Solicitan el % de humedad
          bot.sendMessage(msg.chat_id,"Humidity: "+String(getHumidityPercent(pinHumidityADC))+"%");
        } else if(msg.text=="/getbatalarm") { //Solicitan el nivel de alarma de batería
          bot.sendMessage(msg.chat_id,"Battery alert: "+config.value("batAlarm","0")+"%");
        } else if(msg.text=="/gethumalarm") { //Solicitan el nivel de alarma de humedad
          bot.sendMessage(msg.chat_id,"Humidity alert: "+config.value("humAlarm","0")+"%");
        } else if(msg.text=="/setotaon") { //Solicitan activar modo OTA
          config.add("OTA","1"); //Lo guardamos
          broadcast(msg.from_name+" activa modo OTA");
        } else if(msg.text=="/setotaoff") { //Solicitan desactivar modo OTA
          config.add("OTA","0"); //Lo guardamos
          broadcast(msg.from_name+" desactiva modo OTA");
        } else if(msg.text=="/getip") { //Solicitan mostrar la ip
          bot.sendMessage(msg.chat_id,WiFi.localIP().toString());
        }

      } else { //El autor no es suscriptor
        if(msg.text.startsWith("/subscribe ")) { //Si parece que alguien quiere suscribirse...
          String subscriptionCode=config.value("code","0"); //Obtenemos el código de suscripción (si existe)
          //Si tenemos algún código válido y además coincide...
          if(subscriptionCode!="0" && msg.text.substring(11)==subscriptionCode) { 
            subscribe(&msg); //...incluimos el nuevo usuario como suscriptor
          } else { //El código no es el correcto
            bot.sendMessage(msg.chat_id,"Código incorrecto"); //Informamos
          }
          //Con independencia de si existía código o se ha utilizado correctamente...se elimina
          config.add("code","0");
        }
      } 
    }
    //Hemos terminado de procesar el mensaje actual
    //Leemos el siguiente
    bot.getNextMessage(&msg);
  }
  Serial.println("Procesados todos los mensajes");
}

//Intenta conectar a punto wifi
bool connectWifi() {
  Serial.print("Conectando wifi");
  byte wifiTry=3; //Máximo de veces que intentaremos la conexión
  do {
    Serial.print("\nIntento "+String(4-(int)wifiTry)+".");
    WiFi.mode(WIFI_STA); //Fijamos el modo de conexión a un punto de acceso
    WiFi.disconnect(); //Forzamos a su desconexión (por si estaba conectado anteriormente)
    delay(100); //Esperamos un momento a que desconecte
    WiFi.begin(ssid,password); //Pedimos conexión al punto de acceso
    //Si no hay problemas, se tarda un máximo de 3.7s en establecer conexión con el punto WiFi.
    //Ampliaremos este tiempo por precaución hasta 15s
    byte wifiCheck=150; //Comprobaremos si estamos conectados 150 veces (150 veces * 100ms/vez = 15000ms = 15s)
    //Mientras no hayamos conseguido conexión y no hayamos consumido todos los intentos...seguimos intentándolo
    while ((WiFi.status() != WL_CONNECTED) && ((wifiCheck--)>0)) {
      Serial.print(".");
      delay(100);
    } 
    //Hemos terminado el número de comprobaciones o hemos conectado
    Serial.println((WiFi.status()==WL_CONNECTED)?"Ok":"KO");
  } while(WiFi.status() != WL_CONNECTED && (wifiTry--)>0);
  return WiFi.status() == WL_CONNECTED;
}

void setup() {
  Serial.begin(115200);
  //Inicializamos el diccionario de configuración
  config.begin("/config.txt");
  if(!config.containsKey("batAlarm")) { //Si nunca se ha configurado...
    Serial.println("Inicializando diccionario por primera vez");
    config.add("batAlarm","75"); //Añadimos valor por defecto de alarma por batería baja
    config.add("humAlarm","0"); //Añadimos valor por defecto de alarma por humedad baja
    config.add("period","5"); //Añadimos valor por defecto de periodo de siesta en segundos
    config.add("code",""); //Añadimos valor por defecto de código de autorización
    config.add("OTA","0"); //Añadimos valor por defecto de modo OTA
  }

  bot.begin(botToken); //Inicializamos el bot
  if(connectWifi()) { //Si hemos conseguido conectar...
    subscribers.begin("/subscribers.txt"); //Inicializamos el diccionario de suscriptores
    randomSeed(millis()); //Inicializamos la semilla de números aleatorios

    //Activamos el sensor de humedad por si se solicita el nivel actual de humedad
    //Configuramos el pin de GND para que quede a GND antes de tomar las medidas
    pinMode(pinGND,OUTPUT); digitalWrite(pinGND,LOW);
    //Configuramos el pin de VCC a HIGH para que alimente al sensor de humedad
    pinMode(pinVCC,OUTPUT); digitalWrite(pinVCC,HIGH);

    handleNewMessages(); //Atendemos las peticiones pendientes recibidas por Telegram

    //Leeremos el nivel de batería y lo convertimos a porcentaje
    //uint16_t value=batPercent(getADC(pinBatteryADC));
    uint16_t valueADC=getADC(pinBatteryADC);
    uint16_t value=batPercent(valueADC);

    rtc_gpio_isolate((gpio_num_t)pinBatteryADC); //Desactivamos pinADC de batería

    //Si la batería está por debajo el límite de alarma...informamos
    if(value<config.value("batAlarm","0").toInt()) broadcast("Battery low: "+String(value)+"%");

    //Leeremos el nivel de humedad y lo convertimos a porcentaje
    value=getHumidityPercent(pinHumidityADC);
    rtc_gpio_isolate((gpio_num_t)pinHumidityADC); //Desactivamos pinADC de sensor de humedad
    rtc_gpio_isolate((gpio_num_t)pinGND); //Desactivamos el pin de GND de ADC
    //Si la humedad está por debajo el límite de alarma...informamos
    if(value<config.value("humAlarm","0").toInt()) broadcast("Humidity low: "+String(value)+"%");

    if(config.value("OTA","0")=="1") { //Si está activo el modo OTA...
      //Definimos las funciones de OTA
      ArduinoOTA.onStart([]() { //Cuando inicia
        //Si se solicita actualizar el sistema de archivos...lo desactivamos
        if(ArduinoOTA.getCommand()==U_SPIFFS) {
          broadcast("Actualización de sistema de archivos iniciada");
          SPIFFS.end();
        } else broadcast("Actualización de aplicación iniciada");
      });
      ArduinoOTA.onEnd([]() { //Ha finalizado la transferencia OTA
        broadcast("Actualización finalizada correctamente");
        //Nota. Si la actualización es de sistema de archivos, no enviará el mensaje de broadcast
        //porque necesita el archivo del diccionario de suscriptores
      });
      ArduinoOTA.onProgress([](unsigned int progress,unsigned int total) { //Ha cambiado el porcentaje de progreso OTA
      });
      ArduinoOTA.onError([](ota_error_t error) { //Se ha producido un error en la recepción OTA
        broadcast("Actualización finalizada con error "+String(error));
        //Nota. Si la actualización es de sistema de archivos, no enviará el mensaje de broadcast
        //porque necesita el archivo del diccionario de suscriptores
        //Tras un error OTA, siempre se reinicia automáticamente al finalizar esta función
      });
      //Asignamos una contraseña para impedir que cualquiera pueda actualizarlo
      //No es obligatorio
      ArduinoOTA.setPassword(passwordOTA);
      //Asignamos un nombre al dispositivo para tenerlo identificado
      //Si no lo hacemos tomará como nombre por defecto: "esp32-[ChipID]"
      ArduinoOTA.setHostname(hostname);
      ArduinoOTA.begin(); //Iniciamos el servicio de OTA
      //Ahora que tenemos configurado el modo OTA, escucharemos peticiones durante un rato
      broadcast("Modo OTA activado durante 15 segundos");
      uint16_t timeOTA=15000;
      while(timeOTA>0) {
        ArduinoOTA.handle();
        delay(1);
        timeOTA--;
      }
      //No se ha utilizado el modo OTA
    }
  }
  //Desactivamos Wifi
  WiFi.disconnect(true,true); //Desconectamos WiFi, desactivamos WiFi, eliminamos punto de acceso
  Serial.println("Desactivando wifi y preparando hibernación");
  
  //Nos preparamos para volver a dormir
  //Recuperamos el periodo de siesta y lo convertimos a microsegundos
  uint64_t period=1000000*((uint64_t)config.value("period","0").toInt());
  esp_sleep_enable_timer_wakeup(period); //Tiempo de siesta en microsegundos
  esp_deep_sleep_start(); //Entramos en modo Deep Sleep
  //Cuando salgamos del Deep Sleep lo haremos reiniciando
  //El programa nunca superará este punto
}

void loop() {
  //Nada especial que hacer. Nunca llegaremos a este punto
}
