/*
  Autor: Ramón Junquera
  
  Tema: Sonoff
  
  Objetivo: Ejemplo sencillo de wevidor web
  
  Material adicional: Sonoff, cables, FTDI

  Utilizaremos la librería ESP8266WebServer.h especialmente creada para el ESP8266, que tiene
  ciertas mejoras respecto a la sibrería de gestión de wifi normal.

  Crearemos un servidor web simple que muestra un par de botones para enceder o apagar el relé.

  Gracias a la librería de gestión de servidores wifi de ESP8266 podemos definir ditintas funciones
  para cada URL indicada.

  En nuestro caso sólo tendremos en cuenta 3 URLs:
    / (sin path): sólo muestra la web
    /on: enciende el relé
    /off: apaga el relé
  Los botones llaman de nuevo a la misma URL en la que se encuentran cambiando el path    

  No asignaremos una dirección ip fija al dispositivo.
  Aunque la mostraremos, para que no sea necesario tener el puerto serie conectado, le asignaremos
  un nombre al dispositivo. En nuestro caso será esp8266.local
  Este nombre sólo servirá para la red local. Es como el WINS de Windows.
*/

#include <Arduino.h>
#include <ESP8266WebServer.h> //Librería de gestión de wifi del ESP8266 (mejor que la estándar)
#include <ESP8266mDNS.h> //Librería de gestión de nombres locales


//Definimos los parámetros de la conexión a utilizar
const char* ssid = "RoJo"; //Nombre del punto de acceso
const char* password = "ZAMPABOLLOS5"; //Contraseña

//Definimos los pines a utilizar
const byte pinLed = 13;
const byte pinRelay = 12;

//Creamos una instancia de un servidor escuchando por el puerto 80 llamado server
ESP8266WebServer server(80);

//Creamos un objeto que MDNSResponder que nos permite asignar un nombre a la ip del
//dispositivo. Será más cómodo llamarlo.
MDNSResponder mdns;

//En esta cadena guardaremos el html que formará la página web
String webPage = "<h1>SONOFF Web Server</h1><p><a href=\"on\"><button>ON</button></a>&nbsp;<a href=\"off\"><button>OFF</button></a></p>";


void setup(void)
{
  //El pin del relé también será de salida
  pinMode(pinRelay, OUTPUT);
  //Inicialmente estárá conectado
  digitalWrite(pinRelay, HIGH);
  
  //El pin del led será de salida
  pinMode(pinLed, OUTPUT);
  //Inicialmente estará encendido
  digitalWrite(pinLed, LOW);

  //Inicializamos el puerto seria para mensahes de debug 
  Serial.begin(115200);
  //Esperamos 5 segundos para que le de tiempo al puerto serie a inicializar
  delay(5000);
  //Nos conectamos como cliente a un punto de acceso wifi
  WiFi.begin(ssid, password);
  
  //Informamos que vamos a conectar
  Serial.println();
  Serial.print("Conectando");
  //Máximo tiempo que esperaremos para conectar (10 segundos más que ahora)
  unsigned long int millisMax = millis()+10000;
  //Mientras no hayamos obtenido conexión...
  while(WiFi.status()!=WL_CONNECTED)
  {
    //Cambiamos el estado del led
    digitalWrite(pinLed,!digitalRead(pinLed));
    //Refrescamos los procesos de fondo
    yield();
    //Esperamos un momento
    delay(200);
    //Si hemos alcanzado el tiempo máximo...directamente reiniciamos
    if(millis()>millisMax) ESP.restart();
    //Mostramos un punto para indicar que la conexión está en progreso
    Serial.print(".");
  }
  //Informamos que hemos conseguido conectar
  Serial.println("Conectado!");
  //Mostramos la ip que nos han asignado
  Serial.println(WiFi.localIP());
  //Nos aseguramos que el led quede encendido
  digitalWrite(pinLed, LOW);

  //Intentamos iniciar el servicio de dns local
  //A nuestro dispositivo le asignaremos el nombre "esp8266.local"
  //Que estará asociado a la dirección ip que se le ha asignado
  //Siempre se la añade el dominio .local porque sólo sirve para dominios
  //locales. Como el WINS de Windows.
  //Si la asignación de nombre a ip se ha completado correctamente...
  if (mdns.begin("esp8266", WiFi.localIP()))
  {
    //...informamos
    Serial.println("Nombre asignado correctamente");
  }

  //Crearemos algunas funciones que se llamarán cuando se reciba una conexión
  //en función de los parámetros de la URL

  //Si no tiene parámetros...
  server.on("/", []()
  {
    //Simplemente mostramos el html de la web
    server.send(200, "text/html", webPage);
  }
  );

  //Si tiene el parámetro on...
  server.on("/on", []()
  {
    //Mostramos el parámetro de la web
    server.send(200, "text/html", webPage);
    //Encendemos el led
    //Recordemos que como todos los leds integrados de los ESP8266, los estados
    //están cambiados!
    digitalWrite(pinLed, LOW);
    //Activamos el relé
    digitalWrite(pinRelay, HIGH);
    //Esperamos un segundo para dar tiempo a que se complete
    delay(1000);
  }
  );

  //Si tiene el parámetro off...
  server.on("/off", []()
  {
    //Mostramos el parámetro de la web
    server.send(200, "text/html", webPage);
    //Apagamos el led
    digitalWrite(pinLed, HIGH);
    //Desactivamos el relé
    digitalWrite(pinRelay, LOW);
    //Esperamos un segundo para dar tiempo a que se complete
    delay(1000); 
  }
  );

  //Arrancamos el servidor web que definimos al principio
  server.begin();
  //Informamos
  Serial.println("Servidor web arrancado");
}
 
//Bucle principal
void loop(void)
{
  //Sólo nos preocuparemos de que se gestionen las conexiones cliente que podamos recibir
  server.handleClient();
}
