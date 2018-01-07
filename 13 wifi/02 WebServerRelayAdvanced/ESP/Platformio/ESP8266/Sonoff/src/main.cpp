/*
  Autor: Ramón Junquera
  Tema: Conexión WiFi
  Objetivo: Crear un interruptor (relé) con control remoto por Internet
  Material adicional: Sonoff, cables, FTDI

  Descripción:
  Gestionaremos un relé de manera remota a través de un servidor web integrado en Sonoff.

  Para facilitar la identificación del servidor, se le asignará una ip fija.

  Guardamos en el sistema de archivos dos archivo: log y config.
  En log se guardan las última acciones realizadas en el dispositivo.
  En config se guarda el estado actual de relé y el horario.
  Gracias a ello somos capaces de recuperar el estado que tenía la última vez
  que se apagó.

  El estado del relé se puede cambiar a través de la web o pulsando el botón fisico.

  El interface web permite ver el estado actual del relé, el tipo de horario y las
  última acciones guardadas en el log.

  El dispositivo no depende de Internet para su correcto funcionamiento.
  Si cambiamos el estado del relé pulsando el interruptor y no tenemos conexión a 
  Internet que nos permita obtener la hora actual, se guarda el número de segundos
  transcurridos desde el inicio del dispositivo y cuando se recupera la conexión
  a Internet se calcula y actualiza la fecha y hora correctas del evento.

  El led del dispositivo refleja en todo momento el estado del relé.

  Se han deshabilitado las URLs no permitidas.
  El punto de acceso correcto es http://192.168.1.199:8199/refresh

  Con la configuración adecuada del router, puede ser accesible desde el exterior.

  Pinout:
    
    FTDI Sonoff
    ---- ------
    3.3V 3.3V (pin más cerca del pulsador)
    TX   RX
    RX   TX
    GND  GND (pin más lejos del pulsador)

  El log se guarda en el archivo "log".
  Al arrancar se lee su contenido y se mantiene en memoria.
  Tras cualquier cambio se sobreescribe.
  La estructura de un registro la siguiente:
    4 bytes = fecha/hora en segundos transcurridos desde 1900 (uint32_t)
    1 byte  = contenido, tipo, estado y origen.
      bit 0 = tiene contenido?: no/sí = 0/1
      bit 1 = started: no/si = 0/1
      bit 2 = status changed: no/si = 0/1
      bit 3 = time changed: no/si = 0/1
      bit 4 = estado: OFF/ON = 0/1, time: invierno/verano = 0/1
      bit 5 = origen: switch/web = 0/1
      bit 6 = no utilizado
      bit 7 = no utilizado
  Tamaño total de un registro = 5 bytes.
  El número de registros viene definido por la constante maxLogLines

  La configuración se guarda en el archivo "config".
  Sólo contiene un byte con la siguiente estructura:
    bit 0 = estado actual: apagado/encendido = 0/1
    bit 1 = horario de verano: no/sí = 0/1
    bit 2 = no utilizado
    bit 3 = no utilizado
    bit 4 = no utilizado
    bit 5 = no utilizado
    bit 6 = no utilizado
    bit 7 = no utilizado
  En el horario de verano se debe añadir una hora más.
*/

#include <Arduino.h>
#include <FS.h> //Librería de gestión de SPIFFS
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <time.h>

//Creamos una instancia de un servidor escuchando por el puerto 80 llamado server
ESP8266WebServer server(8199);

//Definimos como constantes el nombre del punto de acceso y su contraseña
const char *wifiSSID = "RoJo";
const char *wifiPassword = "ZAMPABOLLOS5";

//Definición de pines
const byte relayPin = 12; //Pin del relé
const byte ledPin = 13; //Pin del led integrado
const byte switchPin = 0; //Pin del interruptor (pulsador)
//const byte relayPin = 2; //Simula pin del relé = led antena = D4
//const byte ledPin = 14; //Simula led Sonoff = led integrado = D5
//const byte switchPin = D1; //Simula pin del interruptor (pulsador)

//Nombres de los archivos
const char *configFilename = "/config";
const char *logFilename = "/log";

//Número de registros del log
const byte logRegMax = 20;

//Zona horaria = GMT+1
const int8_t timeZone = 1;

//Estructura de un registro del log
struct logReg
{
  uint32_t datetime = 0;
  byte flags = 0;
};

byte configByte = 0;        //Byte del archivo config: estado actual y horario de verano
logReg logArray[logRegMax]; //Array de registros de log

bool switchPressed=false; //Se ha pulsado el interruptor?

void saveConfig()
{
  //Guarda el byte de configuración en el archivo
  File f = SPIFFS.open(configFilename, "w");
  f.write(configByte);
  f.close();
}

void initSPIFFS()
{
  //Inicializa el sistema de archivo
  //Carga el archivo config y lo aplica
  //Carga el archivo de log

  File f;

  //Inicializamos el acceso a archivos de SPIFFS
  SPIFFS.begin();

  //Gestión de archivo config
  //Si no existe el archivo config...lo creamos
  if (!SPIFFS.exists(configFilename)) saveConfig();
  //Lo abrimos como sólo lectura
  f = SPIFFS.open(configFilename, "r");
  //Leemos su contenido
  f.readBytes((char *)&configByte, 1);
  //Ya podemos cerrarlo
  f.close();

  //Gestión de archivo log
  //Si no existe al archivo log...no hacemos nada
  if (!SPIFFS.exists(logFilename))
    return;
  //Seguro que existe el archivo log
  //Lo abrimos como sólo lectura
  f = SPIFFS.open(logFilename, "r");
  //Calculamos cuántos registros contiene
  byte logRegCount = f.size() / sizeof(logReg);
  //Si es más de los permitidos...lo limitamos
  if (logRegCount > logRegMax)
    logRegCount = logRegMax;
  //Recorremos todos los registros a leer
  for (byte i = 0; i < logRegCount; i++)
  {
    //Leemos su contenido en la posición correcta del array
    f.readBytes((char *)(&logArray[i]), sizeof(logReg));
  }
  //Hemos terminado con el archivo log
  f.close();
}

void saveLogArray()
{
  //Guarda el array de log en el archivo
  File f = SPIFFS.open(logFilename, "w");
  f.write((byte *)logArray, sizeof(logReg) * logRegMax);
  f.close();
}

void addLogArray(logReg *newLogReg)
{
  //Añade un nuevo registro al principio de log
  //Tanto al array como al archivo

  //Movemos todos los registros una posición adelante
  for (byte i=logRegMax-1;i>0;i--) logArray[i]=logArray[i-1];
  //Anotamos los detalles al primer registro
  logArray[0].datetime = (*newLogReg).datetime;
  logArray[0].flags = (*newLogReg).flags;
  //Guardamos el array de log en el archivo
  saveLogArray();
}

uint32_t getTime()
{
  //Calcula la fecha y hora actuales
  //Si no puede obtenerlo de Internet, devuelve el número de segundos
  //que lleva el dispositivo en marcha

  //Obtenemos la fecha y hora actuales
  time_t now = time(nullptr);
  //Si lo hemos conseguido...
  if (now)
  {
    //Si debemos sumarle la hora del horario de verano...lo hacemos
    if (configByte & 0b00000010) now += 3600;
  }
  else //No lo hemos conseguido
  {
    //Devolvemos los segundos que lleva en marcha el dispositivo
    now=millis()/1000;
  }
  return now;
}

void setRelay()
{
  //Aplica el estado actual al relé
  digitalWrite(relayPin,configByte & 0b00000001);
  //Aplica el estado actual al led
  digitalWrite(ledPin,!(configByte & 0b00000001));
}

void intSwitch()
{
  //El interruptor ha cambiado de estado

  //Tiempo mínimo para admitir cambios de estado
  const uint32_t minTimeChange=1000; //un segundo
  //Hora del último cambio de estado del interruptor
  static uint32_t lastChange=0;

  //Anotamos la hora actual
  uint32_t now=millis();
  //Si ha pasado el tiempo suficiente para tener en cuenta el cambio
  //y el interruptor está pulsado...lo anotamos
  if(now-lastChange>minTimeChange && !digitalRead(switchPin)) switchPressed=true;
  //El último cambio será ahora
  lastChange=now;
}

String formatDate(uint32_t s)
{
  //Convierte el número de segundos transcurridos desde 1900 a un String
  //con el formato de fecha correcto: DD/MM/YYYY HH:MM:SS

  struct tm *t;
  char buffer[20]; //No necesitamos más de 20 caracteres

  //Convertimos la fecha en segundos a formato tm
  t=localtime((time_t*)&s);
  //Aplicamos formato
  sprintf(buffer,"%02d/%02d/%04d %02d:%02d:%02d",(*t).tm_mday,(*t).tm_mon+1,(*t).tm_year+1900,(*t).tm_hour,(*t).tm_min,(*t).tm_sec);
  return buffer;
}

String composeHTML()
{
  //Devuelte un String con el html a enviar al cliente
  
  //Cabecera
  String a = "<h1>Sonoff</h1>";
  //Horario
  a+="<p><h2>Horario de ";
  a+=(configByte & 0b00000010)?"verano":"invierno";
  a+="</h2></p>";
  //Estado
  a+="<p><h2>Estado actual: ";
  a+=(configByte & 0b00000001)?"ON":"OFF";
  a+="</h2></p>";
  //Botones
  a+="<p><a href=\"refresh\"><button style=""font-size:300%"">Actualizar</button></a>";
  a+="&nbsp;<a href=\"changeStatus\"><button style=""font-size:300%"">Cambiar estado</button></a>";
  a+="&nbsp;<a href=\"changeTime\"><button style=""font-size:300%"">Cambiar horario</button></a></p>";
  //Recorremos todos los registros del array
  for (byte i = 0; i < logRegMax; i++)
  {
    //Inicio de párrafo
    a+="<p>";
    //Anotamos las características del registro
    byte flags = logArray[i].flags;
    //Si el registro está vacío...hemos terminado de recorrer los registros del log
    if (!(flags & 0b00000001)) break;
    //El registro contiene algo
    //Mostramos la fecha con formato
    a+=formatDate(logArray[i].datetime);
    a+=" ";
    //Si es de arranque...
    if (flags & 0b00000010) a+="iniciado";
    //Si es de cambio de horario
    else if(flags & 0b00000100)
    {
      a+="horario ";
      a+=(flags & 0b00001000)?"verano":"invierno";
    }
    else //Tiene que ser de cambio de estado
    {
      a+=(flags & 0b00001000)?"ON":"OFF";
      a+=" por ";
      a+=(flags & 0b00010000)?"web":"interruptor";
    }
    //Fin de párrafo
    a+="</p>\n";
  }
  //Fin de html
  a+="</html>\n";
  //Devolvemos la cadena que hemos compuesto
  return a;
}

void changeTime()
{
  //Cambio de horario verano/invierno

  //Cambiamos de horario
  configByte^=0b00000010;
  //Guardamos la configuración
  saveConfig();
  //Debemos añadir una nueva línea al log que indique que el cambio de horario
  //Creamos un nuevo registro de log
  logReg newLogReg;
  //Anotamos que es un registro con contenido, de cambio de horario
  // bit 0 = tiene contenido?: sí = 1
  // bit 1 = started: no = 0
  // bit 2 = changed: time = 1
  // bit 3 = time: invierno/verano = 0/1
  // bit 4 = origen: switch/web = 0/1 (irrelevante)
  // bit 5 = no utilizado
  // bit 6 = no utilizado
  // bit 7 = no utilizado
  newLogReg.flags=0b00000101;
  //Si el horario actual es verano...lo anotamos
  if(configByte & 0b00000010) newLogReg.flags|=0b00001000;
  //Obtenemos la fecha actual y la guardamos en el registro
  newLogReg.datetime=getTime();;
  //Añadimos el registro al log
  addLogArray(&newLogReg);
}

void changeStatus(bool byWeb)
{
  //Cambia el estado del relé y anota el cambio en el log.
  //Permite indicar si el cambio es con el interruptor local o por web.

  //Cambia el estado actual
  configByte^=0b00000001;
  //Guardamos la configuración
  saveConfig();
  //Debemos añadir una nueva línea al log que indique que el usuario local
  //ha cambiado el estado
  //Creamos un nuevo registro de log
  logReg newLogReg;
  //Anotamos que es un registro con contenido, de cambio de estado, por el interruptor local
  // bit 0 = tiene contenido?: no/sí = 0/1
  // bit 1 = started: no/si = 0/1
  // bit 2 = changed: status/time = 0/1
  // bit 3 = estado: OFF/ON = 0/1, time: invierno/verano = 0/1
  // bit 4 = origen: switch/web = 0/1
  // bit 5 = no utilizado
  // bit 6 = no utilizado
  // bit 7 = no utilizado
  newLogReg.flags=0b00000001;
  //Si el cambio es por web...lo anotamos
  if(byWeb) newLogReg.flags|=0b00010000;
  //Si el estado actual es encendido...anotamos que se ha cambiado a encendido
  if(configByte & 0b00000001) newLogReg.flags|=0b00001000;
  //Obtenemos la fecha actual y la guardamos en el registro
  newLogReg.datetime=getTime();
  //Añadimos el registro al log
  addLogArray(&newLogReg);
  //Aplicamos el estado actual al relé
  setRelay();
}

void handleRefresh()
{
  //Gestiona una petición de refresco en página web

  //Simplemente mostramos el html de la web
  server.send(200,"text/html",composeHTML());
}

void handleChangeStatus()
{
  //Gestiona una petición de cambio de estado en página web

  //Cambiamos el estado del relé indicando que se ha realizado por la web
  changeStatus(true);
  //Refrescamos la página
  handleRefresh();
}

void handleChangeTime()
{
  //Gestiona una petición de cambio de horario en página web

  //Cambiamos el horario
  changeTime();
  //Refrescamos la página
  handleRefresh();
}

void handleNotFound()
{
  //Gestiona una petición de una URL desconocida

  //No hacemos nada. No devolvemos respuesta
  //Al cliente le aparecerá un mensaje del tipo: Esta página no funciona porque no envía datos
}

void setup()
{
  //Configuramos el pin del relé como salida
  pinMode(relayPin, OUTPUT);
  //Definimos el pin del led integrado como salida
  pinMode(ledPin, OUTPUT);
  //Configuramos el pin del interruptor como entrada con las resistencias de pullup activas
  pinMode(switchPin, INPUT_PULLUP);
  //Inicializamos el sistema de archivos y leemos config y log
  initSPIFFS();
  //Aplicamos el estado actual al relé
  setRelay();
  //Creamos un nuevo registro para indicar el actual arranque
  logReg logRegStart;
  //Obtenemos la fecha actual y gardamos en el registro
  //La fecha no será válida porque aun no hemos inicializado las comunicaciones
  //Aun así se guarda el número de segundos transcurridos desde el último reinicio de dispositivo,
  //que probablemente sea 0.
  //Ya actualizaremos a la hora correcta cuando consigamos comunicación
  logRegStart.datetime = getTime();
  //Los detalles del actual registro son:
  // bit 0 = tiene contenido?: sí = 1
  // bit 1 = started: si = 1
  // bit 2 = changed: status/time = 0/1 (irrelevante)
  // bit 3 = estado: OFF/ON = 0/1, time: invierno/verano = 0/1 (irrelevante)
  // bit 4 = origen: switch/web = 0/1 (irrelevante)
  // bit 5 = no utilizado
  // bit 6 = no utilizado
  // bit 7 = no utilizado
  logRegStart.flags = 0b00000011;
  //Anotamos el nuevo registro en el log
  addLogArray(&logRegStart);
  //Definimos la hora de Internet
  //A la UTC le debemos añadir tantas horas como la zona horaria en la que estemos
  configTime((int32_t)timeZone * 3600, 0, "pool.ntp.org", "time.nist.gov");

  //Definimos algunas funciones para gestionar las peticiones cliente dependiendo de la URL
  server.on("/refresh", handleRefresh);
  server.on("/changeStatus", handleChangeStatus);
  server.on("/changeTime", handleChangeTime);
  server.onNotFound(handleNotFound);
  //Arrancamos el servidor web
  server.begin();

  //Configuramos la función de interrupción del switch
  attachInterrupt(switchPin,intSwitch,CHANGE);
};

void wifiConnect()
{
  //Intenta obtener conexión wifi si es que no la tiene

  //Se ha asignado la dirección Iip estática?
  static bool staticIpAssigned=false;
  //Último intento de conexión wifi que hicimos
  static uint32_t lastTry = 0; //Inicialmente nunca

  //Si no estamos conectados a wifi...
  if (WiFi.status() != WL_CONNECTED)
  {
    //...y han pasado más de 15 segundos desde el último intento...
    if (millis() - lastTry > 15000)
    {
      //...lo intentamos de nuevo
      WiFi.mode(WIFI_STA);
      WiFi.begin(wifiSSID, wifiPassword);
      //Anotamos que lo acabamos de intentar
      lastTry = millis();
    }
  }
  else //Estamos conectados a wifi
  {
    //Si no se ha asignado la dirección ip estática...lo hacemos ahora
    if(!staticIpAssigned) WiFi.config(IPAddress(192,168,1,199),WiFi.gatewayIP(),WiFi.subnetMask());
  }
}

void setLogDatetime()
{
  //Comprobamos si hay registros de log sin fecha definida
  //Nota: si alguna vez hay registros sin fecha, habitualmente sólo será uno.

  //Recorremos todos los registros de log
  for (byte r = 0; r < logRegMax; r++)
  {
    //Si el registro tiene contenido...
    if (logArray[r].flags & 0b00000001)
    {
      //Si la fecha del registro es inferior a un año (60*60*24*365=31536000)...
      if (logArray[r].datetime < 31536000)
      {
        //...consideramos que la fecha es incorrecta
        //Obtenemos la fecha y hora actuales
        uint32_t now = getTime();
        //Si es una fecha válida...
        if(now > 31536000)
        {
          //Calculamos la hora en la que se guardó este registro
          now = now - millis()/1000 + logArray[r].datetime;
          //Anotamos la hora correcta
          logArray[r].datetime = now;
          //Guardamos el log en el archivo
          saveLogArray();
        }
      }
    }
  }
}

void loop()
{
  //Comprobamos que mantenemos conexión wifi
  wifiConnect();
  //Comprobamos si hay registros de log sin fecha definida
  setLogDatetime();
  //Si se ha pulsado el interruptor...
  if(switchPressed)
  {
    //Estado cambiado con el interruptor local
    changeStatus(false);
    //Ya hemos gestionado la pulsación. La borramos
    switchPressed=false;
  }

  //Gestionamos el servidor web
  server.handleClient();
}
