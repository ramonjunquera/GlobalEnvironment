/*
  Autor: Ramón Junquera
  Tema: ArduCAM
  Versión: 20220108
  Objetivo: Demo de uso de varios dispositivos SPI
  Material adicional: placa ESP, ArduCAM-Mini-2MP, display SSD1331
  Descripción:
    El objetivo es mostrar las imágenes capturadas por la cámara en tiempo real en
    el display.
    Puesto que no sabemos descomprimir jpg, utilizaremos imágenes sin compresión (bmp).
    Puesto que queremos mostrar la imagen en un display de 96x64 es suficiente tomar
    la resolución más baja que nos permite la cámara (160x120).
    El método savePhoto escala automáticamente la imagen capturada al tamaño del
    sprite que se indica.

    Se muestran una colección de comandos disponibles por puerto serie para modificar
    las características de la captura y poder ver el resultado inmediatemente.

    Nota.
    Aunque el display sea capaz de utilizar la comunicación SPI a 80MHz, la cámara
    no permite más de 8MHz.
    Aunque las configuraciones SPI de ambos dispositivos son independientes y cada
    vez que se abre una nueva transmisión se indican las propiedades de conexión,
    el reloj de SPI (SCK) de la cámara afecta al display. Así que no tenemos más
    remedio que ajustar la frecuencia del SPI del display al de la cámara: 8MHz.

    Recordemos la disposición de pines de comunicaciones de los distintos modelos:

            ------SPI------- --I2C--
    Modelo  SCK MISO MOSI CS SCL SDA
    ------- --- ---- ---- -- --- ---
    ESP8266 D5   D6   D7  D8 D1  D2
    ESP32   18   19   23   5 22  21
  
  Resultado:
    Podemos ver en el display las imágenes capturadas por la cámara en tiempo real y
    modificar las propiedades de la captura.
*/

#include <Arduino.h>
#include <RoJoArduCAM.h> //Librería de gestión de ArduCAM
#include <RoJoSSD1331.h> //Librería de gestión del display SSD1331
#include <RoJoSprite3.h> //Gestión de sprites

////Definimos pines según el modelo de placa
#ifdef ESP32
  const byte pinCScamera=4;
  const byte pinDC=2;
  const byte pinRES=15;
  const byte pinCSdisplay=5;
#elif defined(ESP8266) //Si es ESP8266...
  const byte pinCScamera=D0;
  const byte pinDC=D4;
  const byte pinRES=D8;
  const byte pinCSdisplay=D3;
#endif

//Creamos los objetos de gestión
RoJoArduCAM camera;
RoJoSSD1331 display;

//Creamos un sprite de 16 bits para guardar la imagen capturada
//Lo creamos aquí para no tener que crearlo y destruirlo continuamente en el loop
RoJoSprite3 sprite(2);

//Muestra los comandos disponibles por el puerto serie
void help() {
  Serial.println(F("Ayuda. Comandos disponibles."));
  Serial.println(F(" 0: Iluminacion: auto"));
  Serial.println(F(" 1: Iluminacion: soleado"));
  Serial.println(F(" 2: Iluminacion: nublado"));
  Serial.println(F(" 3: Iluminacion: oficina"));
  Serial.println(F(" 4: Iluminacion: hogar"));
  Serial.println(F(" 5: Saturacion: -2"));
  Serial.println(F(" 6: Saturacion: -1"));
  Serial.println(F(" 7: Saturacion: 0"));
  Serial.println(F(" 8: Saturacion: +1"));
  Serial.println(F(" 9: Saturacion: +2"));
  Serial.println(F(" A: Brillo: -2"));
  Serial.println(F(" B: Brillo: -1"));
  Serial.println(F(" C: Brillo: 0"));
  Serial.println(F(" D: Brillo: +1"));
  Serial.println(F(" E: Brillo: +2"));
  Serial.println(F(" F: Contraste: -2"));
  Serial.println(F(" G: Contraste: -1"));
  Serial.println(F(" H: Contraste: 0"));
  Serial.println(F(" I: Contraste: +1"));
  Serial.println(F(" J: Contraste: +2"));
  Serial.println(F(" K: Efecto especial: ninguno"));
  Serial.println(F(" L: Efecto especial: antiguo"));
  Serial.println(F(" M: Efecto especial: azulado"));
  Serial.println(F(" N: Efecto especial: verdoso"));
  Serial.println(F(" O: Efecto especial: rojizo"));
  Serial.println(F(" P: Efecto especial: blanco y negro"));
  Serial.println(F(" Q: Efecto especial: negativo"));
  Serial.println(F(" h: Ayuda"));
}

void setup() {
  Serial.begin(115200);
  while(!Serial) yield();
  byte errorCode = camera.begin(pinCScamera); //Inicializamos la cámara
  if(errorCode) { //Si tenemos algún error en la inicialización...
    Serial.print("Camara inicializada con error ");
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
  Serial.println("Camara inicializada correctamente");
  //Inicializamos el display
  //Indicamos que la frecuencia del bus SPI es de 8Mhz
  //para hacerlo compatible con la cámara
  display.begin(pinRES,pinDC,pinCSdisplay,8000000);
  camera.setFormatBMP(); //Capturaremos imágenes sin comprimir
  camera.setResolution(0); //Utilizaremos la resolución más baja (160x120)
  help(); //Mostramos la ayuda
}

void loop() {
  byte errorCode;
  
  camera.takePhoto(); //Capturamos una imagen
  do {
    delay(5);
    //Pedimos que la imagen capturada se guarde en el sprite, redimensionado
    //a las medidas del display y anotamos el código de error
    errorCode=camera.savePhoto(display.xMax(),display.yMax(),&sprite);
  } while(errorCode==4); //Repetimos mientras no se haya terminado la captura
  //La captura ha terminado
  
  //Si tenemos algún otro error...lo mostramos
  if(errorCode) Serial.println("errorCode="+String((int)errorCode));
  //No hay errores...mostramos el sprite redimensionado en pantalla
  else display.drawSprite(&sprite);
  
  if (Serial.available()) { //Si hay algún carácter en el buffer de entrada del puerto serie...
    switch (Serial.read()) { //Dependiendo del carácter...
      case '0':
        camera.setLight(0);
        Serial.println(F("Iluminacion: auto"));
        break;
      case '1':
        camera.setLight(1);
        Serial.println(F("Iluminacion: soleado"));
        break;
      case '2':
        camera.setLight(2);
        Serial.println(F("Iluminacion: nublado"));
        break;
      case '3':
        camera.setLight(3);
        Serial.println(F("Iluminacion: oficina"));
        break;
      case '4':
        camera.setLight(4);
        Serial.println(F("Iluminacion: hogar"));
        break;
      case '5':
        camera.setSaturation(-2);
        Serial.println(F("Saturacion: -2"));
        break;
      case '6':
        camera.setSaturation(-1);
        Serial.println(F("Saturacion: -1"));
        break;
      case '7':
        camera.setSaturation(0);
        Serial.println(F("Saturacion: 0"));
        break;
      case '8':
        camera.setSaturation(1);
        Serial.println(F("Saturacion: +1"));
        break;
      case '9':
        camera.setSaturation(+2);
        Serial.println(F("Saturacion: +2"));
        break;
      case 'A':
        camera.setBrightness(-2);
        Serial.println(F("Brillo: -2"));
        break;
      case 'B':
        camera.setBrightness(-1);
        Serial.println(F("Brillo: -1"));
        break;
      case 'C':
        camera.setBrightness(0);
        Serial.println(F("Brillo: 0"));
        break;
      case 'D':
        camera.setBrightness(1);
        Serial.println(F("Brillo: +1"));
        break;
      case 'E':
        camera.setBrightness(+2);
        Serial.println(F("Brillo: +2"));
        break;
      case 'F':
        camera.setContrast(-2);
        Serial.println(F("Contraste: -2"));
        break;
      case 'G':
        camera.setContrast(-1);
        Serial.println(F("Contraste: -1"));
        break;
      case 'H':
        camera.setContrast(0);
        Serial.println(F("Contraste: 0"));
        break;
      case 'I':
        camera.setContrast(1);
        Serial.println(F("Contraste: +1"));
        break;
      case 'J':
        camera.setContrast(+2);
        Serial.println(F("Contraste: +2"));
        break;
      case 'K':
        camera.setEffect(0);
        Serial.println(F("Efecto: ninguno"));
        break;
      case 'L':
        camera.setEffect(1);
        Serial.println(F("Efecto: antiguo"));
        break;
      case 'M':
        camera.setEffect(2);
        Serial.println(F("Efecto: azulado"));
        break;
      case 'N':
        camera.setEffect(3);
        Serial.println(F("Efecto: verdoso"));
        break;
      case 'O':
        camera.setEffect(4);
        Serial.println(F("Efecto: rojizo"));
        break;
      case 'P':
        camera.setEffect(5);
        Serial.println(F("Efecto: blanco y negro"));
        break;
      case 'Q':
        camera.setEffect(6);
        Serial.println(F("Efecto: negativo"));
        break;
      case 'h':
        help();
        break;
      default:
        break;
    }
  }
}

