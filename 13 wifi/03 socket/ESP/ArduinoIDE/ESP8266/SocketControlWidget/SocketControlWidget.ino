/*
  Autor: Ramón Junquera
  Tema: wifi
  Fecha: 20180621
  Objetivo: Demostración de uso de la aplicación Socket Control Widget
  Material adicional: placa ESP8266

  Descripción:
  La aplicación Socket Control Widget para Android se puede descargar gratuitamente de Play Store.
  Es muy simple. No tiene una aplicación asociada. Lo único que hace es crear un widget.
  Cuando se pulsa en un widget se envía un comando a un servidor.
  
  Para los widgets que creamos podemos seleccionar las siguientes propiedades:
  - Nombre del widget
  - Hostname del servidor
  - Puerto
  - Comando a enviar

  Desarrollaremos un programa para que conecte nuestra placa a un punto de acceso wifi.
  Se creará un servidor que escuche por el puerto 7755/tcp.
  Fijaremos la dirección ip para no tener que buscarlo.
  Cuando se reciba el comando "0" se cambiará el estado del led integrado.

  El comando para configurar el widget será: 192.168.1.189:7755/0
  

  Nota:
  La cadena que se recibe en el servidor es simplemente "0". El comando enviado.
  No es lo mismo que escribir en el navegador http://192.168.1.189:7755/0
  Porque esto envía como primera línea de comando: "GET /0 HTTP/1.1"
  Por lo tanto es un buen sistema para evitar que pueda ser utilizado con navegadores.

  Resultado:
  Al pulsar el widget en el móvil cambiará el estado del led integrado
*/

#include <ESP8266WiFi.h>

//Definimos el nombre del punto de acceso y la constraseña que se utilizarán para conectar
const char* ssid = "RoJo";
const char* password = "ZAMPABOLLOS5";

//Definimos el pin que vamos a utilizar
const byte pin=BUILTIN_LED; //El del led integrado

//Creamos un servidor que escucha por el puerto 7755/tcp
WiFiServer server(7755);

void setup()
{
  //Inicializamos el puerto serie para debug
  Serial.begin(115200);
  delay(10);

  //Configuramos el pin como salida
  pinMode(pin, OUTPUT);

  //Conectando por wifi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  //Hemos conseguido conectar
  //Fijamos la dirección ip, dejando el resto de parámetros igual
  WiFi.config(IPAddress(192,168,1,189),WiFi.gatewayIP(),WiFi.subnetMask());
  
  //Arrancamos el servidor
  server.begin();
  Serial.println("Server started");

  //Mostramos la ip que que tiene actualmente el dispositivo
  //Debería ser la misma que hemos fijado
  Serial.println(WiFi.localIP());
}

void loop()
{
  //Anotamos los datos del cliente que ha conectado
  WiFiClient client = server.available();
  //Si nadie ha conectado...hemos terminado
  if (!client) return;
  
  //Indicamos que alguien se ha conectado
  Serial.println("new client");
  //Mientras no haya datos procedentes del cliente esperamos
  while(!client.available())
  {
    delay(1);
  }
  //Hemos recibido algún dato del cliente!
  
  //Leemos la primera línea de la información obtenida del cliente
  String req = client.readStringUntil('\n');
  //La mostramos
  Serial.println("First line read: " + req);
  Serial.printf("Length: %u\n",req.length());
  //Vaciamos el buffer de entrada del cliente procesado
  client.flush();

  //Si la línea no coincide con el valor esperado...
  if(req!="0")
  {
    //Indicamos que desconocemos el comando
    Serial.println("invalid request");
    //Finalizamos la conexión en el cliente
    client.stop();
    //Hemos terminado de procesar
    return;
  }
  //El comando es el correcto!

  //Cambiamos el estado del pin
  digitalWrite(pin,!digitalRead(pin));
  //Vaciamos el buffer de entrada del cliente procesado

  //Finalizamos la conexión en el cliente
  //Si no lo hacemos así, al terminar la función loop se destruirá automáticamente el objeto client
  //y también se finalizará la conexión
  client.stop();
}

