/*
  Autor: Ramón Junquera
  Tema: ArduCAM
  Versión: 20220108
  Objetivo: Transferir imagen en tiempo real por wifi
  Aplicación: cliente (cámara).
*/

#include <Arduino.h>
#include <WiFi.h>
#include <RoJoArduCAM.h> //Librería de gestión de ArduCAM
#include <RoJoSprite3.h> //Gestión de sprites color

//Pinout
#ifdef ARDUINO_LOLIN32
  const byte pinCS=4;
  const byte pinSDA=255; //Por defecto
  const byte pinSCL=255; //Por defecto
#elif defined(ARDUINO_LOLIN32LITE)
  const byte pinCS=17;
  //Esta placa no tiene pines por defecto I2C. Debemos definirlos
  const byte pinSDA=4;
  const byte pinSCL=13;
  #define BUILTIN_LED 2
#else
  #error Device unknown
#endif

//Definimos el punto de acceso con el que conectaremos
const char* wifiSSID = "displayHost";
const char* wifiPassword = "displayHost";
//Definimos el nombre (o ip) de servidor al que enviaremos las imágenes
const char* host = "192.168.4.1";

RoJoArduCAM camera; //Objeto de gestión de la cámara
RoJoSprite3 sprite(2); //Creamos el sprite que utlizaremos para guardar la imagen
WiFiClient client; //Objeto de gestión de la conexión wifi

void setup() {
  Serial.begin(115200);
  while(!Serial) delay(1);
  Serial.println();
  
  byte errorCode = camera.begin(pinCS,pinSDA,pinSCL); //Inicializamos la cámara
  if(errorCode) { //Si tenemos algún error en la inicialización...
    Serial.print(F("Camara inicializada con error "));
    Serial.println(errorCode);
    pinMode(BUILTIN_LED,OUTPUT); //Led integrado configurado como salida
    digitalWrite(BUILTIN_LED,HIGH); //Led integrado apagado
    while(1) { //Bucle infinito
      for(byte i=0;i<errorCode;i++) { //Repetimos tantas veces como código de error
        digitalWrite(BUILTIN_LED,LOW); //Encendido
        delay(300);
        digitalWrite(BUILTIN_LED,HIGH); //Apagado
        delay(300);
      }
      delay(1000);
    };    
  }
  
  camera.setFormatBMP(); //Capturaremos imágenes sin comprimir
  camera.setResolution(0); //Utilizaremos la resolución más baja (160x120)
  //Conexión wifi
  WiFi.mode(WIFI_STA); //Conectado como cliente a un punto de acceso
  WiFi.begin(wifiSSID,wifiPassword);
  Serial.print(F("Conectando a punto de acceso"));
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println(F("ok"));
  //Conexión con el servidor
  if(!client.connect(host,8266)) {
    Serial.println(F("Error al conectar con el servidor"));
    while(1) yield(); //Paramos para siempre
  }
  //Hemos conseguido conectar con el servidor
  Serial.println("Conectado con el servidor");
}

//Envía un byte al servidor
void sendByte(byte b) {
  //El carácter 0xFF se considera código de escape
  //En el caso que se tenga que enviar el valor 0xFF se tendrá
  //que enviar el código de escape seguido de otro byte con 
  //valor 0x00

  client.write(b);
  if(b==0xFF) client.write((byte)0);
}

void loop() {
  byte errorCode;
  
  camera.takePhoto(); //Capturamos una imagen
  do {
    delay(5);
    //Pedimos que la imagen capturada se guarde en el sprite, redimensionado
    //a las medidas del display y anotamos el código de error
    errorCode=camera.savePhoto(96,64,&sprite);
  } while(errorCode==4); //Repetimos mientras no se haya terminado la captura
  //La captura ha terminado
  if(errorCode) { //Si tenemos algún otro error...
    //...lo mostramos
    Serial.print(F("errorCode="));
    Serial.println(errorCode);
    return; //...y terminamos
  }
  //No hay errores
  //Enviamos el contenido del sprite al servidor

  //Enviamos marca de inicio de imagen
  client.write(0xFF); //Código de escape
  client.write(0x01); //Inicio de imagen
  
  for(byte y=0;y<64;y++) { //Recorremos filas
    for(byte x=0;x<96;x++) { //Recorremos columnas
      uint16_t color=sprite.getPixel(x,y); //Obtenemos color de pixel
      //Descomponemos en bytes
      byte lb=color & 0xFF;
      byte hb=color >> 8;
      //Enviamos cada byte
      sendByte(lb);
      sendByte(hb);
    }
  }
}

