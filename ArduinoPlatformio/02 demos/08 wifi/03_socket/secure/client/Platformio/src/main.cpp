/*
  Autor: Ramón Junquera
  Fecha: 20201021
  Descripción:
    Ejemplo de gestión de socket seguro. Cliente

  Secuencia de arranque:
  - Led rojo: Intentando conexión con el punto de acceso Wifi.
  - Led verde: Conexión con Wifi establecida.
  Secuencia de conexión segura:
  - Pulsación del botón
  - Led apagado: Intentando conectar con el servidor
  - Led azul: Conexión establecida
  - Led rojo: No se ha podido conectar con el servidor

  Comentarios:
  - Cada vez que se conecta al servidor seguro, le envía el valor de millis() como texto,
    espera dos segundos manteniendo la conexión abierta y después la cierra.
  - Cada vez que se pulsa el botón se intenta una nueva conexión. El tiempo de conexión
    depende de la longitud del certificado del servidor.
  - No parece funcionar con certificados de longitud inferior a 1024bits
*/

#include <Arduino.h>
#include <WiFiClientSecure.h> //Librería para gestión de conexiones https
#include <RojoNeopixel2.h>
#include <RoJoSwitch.h>

//Declaración de constantes globales
const char* wifiClientSSID = "SecureServer";
const char* wifiClientPassword = "SecureServer";
const char* serverHost = "192.168.4.1"; //Dirección del servidor
const uint16_t port=443; //Puerto
const uint32_t timeout=10000; //Tiempo de espera de nuevos caracteres para enviar
const byte pinLed=27; //Pin de led NeoPixel
const byte pinSwitch=39; //Pin del pulsador

//Declaración de variables globales
WiFiClientSecure client; //Objeto de conexión de cliente seguro
RoJoNeoPixel2 led;
//char c; //Carácter leido
RoJoSwitch button(pinSwitch);

void setup() {
  Serial.begin(115200);
  led.begin(1,pinLed); //Sólo tenemos un led
  led.videoMem[0].setColor(0x880000); led.show(); //Rojo
  delay(1000); //Damos tiempo a que inicialice el puerto serie
  WiFi.mode(WIFI_STA); //Conectado como cliente a un punto de acceso
  WiFi.begin(wifiClientSSID,wifiClientPassword);
  Serial.print("\nConectando a punto de acceso.");
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("ok");
  
  client.setInsecure(); //Aceptaremos conexión con servidores no seguros (como el nuestro)
  led.videoMem[0].setColor(0x008800); led.show(); //Verde
}

void loop() {
  if(button.pressed()) { //Si se pulsa el botón...
    Serial.print("Conectando con servidor...");
    led.videoMem[0].setColor(0x000000); led.show(); //Led apagado
    if(client.connect(serverHost,port)) { //Si hemos pedido conectar...
      Serial.print("OK : ");
      led.videoMem[0].setColor(0x000088); led.show(); led.show(); //Azul
      String str=String(millis()); //Convertimos los millis en String y lo guardamos
      client.print(str); //Enviamos el String al cliente
      client.flush(); //Forzamos el envío
      Serial.println(str); //Mostramos el valor enviado por el puerto serie
      delay(2000);
      client.stop();
    } else {
      Serial.println("ERROR");
      led.videoMem[0].setColor(0x880000); led.show(); led.show(); //Rojo
      delay(2000);
    }
    led.videoMem[0].setColor(0x008800); led.show(); //Verde
  }
  delay(1);
}
