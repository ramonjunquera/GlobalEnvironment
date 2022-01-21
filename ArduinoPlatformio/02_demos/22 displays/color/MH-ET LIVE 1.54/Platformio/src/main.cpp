/*
  Tema: Gestión de display MH-ET LIVE E-paper 1.54"
  Fecha: 20220121
  Autor: Ramón Junquera

  Descripción:
    Ejecución de distintos ejemplos de la librería RoJoSSD1681
*/
#include <Arduino.h>
#include <RoJoSSD1681.h>
#include <RoJoSprite3.h>

#ifdef ESP8266
  byte pinRES=D4;
  byte pinDC=D3;
  byte pinCS=D0;
  byte pinBUSY=D2;
#elif defined(ESP32)
  byte pinRES=33;
  byte pinDC=27;
  byte pinCS=14;
  byte pinBUSY=35;
#elif defined(ARDUINO_AVR_UNO)
  byte pinRES=3;
  byte pinDC=5;
  byte pinCS=4;
  byte pinBUSY=6;
#else
  #error Device unknown
#endif

//Definición de variables globales
RoJoSSD1681 display;

//Carga de imagen desde bmp y la envía al display sin consumir memoria
void test01() {
  Serial.println("test01. begin");
  byte errorCode=display.drawBMP("/tractor.bmp");
  Serial.println("test01. errorCode="+String((int)errorCode));
  display.show();
  Serial.println("test01. end");
  delay(2000);
}

//Enviando sprites monocromos
//Sprites desplazados
void test02() {
  Serial.println("test02. begin");
  //Utilizamos un solo sprite del tamaño del display y con un disco centrado
  //Enviaremos el sprite a cada capa
  //En la capa blanca enviaremos un sprite con fondo a 1 y con circulo a 0
  //para que se corresponda a fondo blanco y disco negro
  //En la capa roja enviaremos un fondo 0 para que sea transparente y un
  //disco a 1 para que sea rojo
  //El sprite de la capa roja lo desplazaremos 8 pixels en horizontal y
  //verical para que aparezca con la sobra de la otra capa
  RoJoSprite3 sprite(0); //Sprite de capa
  sprite.begin(); //Inicializamos sprite
  sprite.setSize(200,200); //Dimensionamos sprite con tamaño de display
  display.clear(); //Borramos cualquier imagen. Dejamos todo en blanco
  //Sprite blanco
  sprite.clear(1); //Todo blanco
  sprite.disk(100,100,80,0); //Disco negro
  display.drawSprite(&sprite,0,0,false); //Enviamos sprite a capa blanca
  //Sprite rojo
  sprite.clear(); //Transparente
  sprite.disk(100,100,80,1); //Disco rojo
  display.drawSprite(&sprite,8,8,true); //Enviamos sprite a capa roja desplazado
  display.show(); //Mostramos imagen enviada
  sprite.end(); //Ya no necesitamos el sprite
  Serial.println("test02. end");
  delay(2000);
}

//Enviando sprites monocromos
//Imagen y texto generado en sprites de capas
void test03() {
  Serial.println("test03. begin");
  //Crearemos un par de sprites monocromos. Uno para cada capa.
  //Dibujaremos unas líneas, circulos y texto en ellos
  //Enviaremos primero una capa, después la otra y finalmente ambas a la vez.

  RoJoSprite3 spriteW(0),spriteR(0); //Sprites para cada capa
  spriteW.begin(); spriteR.begin(); //Inicializamos sprites
  spriteW.setSize(200,200); spriteR.setSize(200,200); //Dimensionamos sprites como display

  spriteW.clear(1); //Todo blanco
  for(byte i=0;i<200;i+=10) {
    spriteW.line(0,199,i,0,0); //Línea negra
    spriteR.line(199,199,i,0,1); //Línea roja
  }
  spriteW.circle(100,100,60,0); //Círculo negro
  spriteR.disk(100,100,30,1); //Disco rojo
  spriteW.printOver("/10x22.fon","Hola!",0,80,170);

  display.clear(); //Borramos cualquier imagen. Dejamos todo en blanco
  display.drawSprite(&spriteW,0,0,false); //Enviamos sprite a capa blanca
  display.show(); //Mostramos imagen enviada
  delay(1000);
  display.clear(); //Borramos cualquier imagen. Dejamos todo en blanco
  display.drawSprite(&spriteR,0,0,true); //Enviamos sprite a capa roja
  display.show(); //Mostramos imagen enviada
  delay(1000);
  display.clear(); //Borramos cualquier imagen. Dejamos todo en blanco
  display.drawSprite(&spriteW,&spriteR); //Enviamos las dos capas a la vez
  display.show(); //Mostramos imagen enviada
  spriteW.end(); spriteR.end(); //Ya no necesitamos los sprites
  Serial.println("test03. end");
  delay(2000);
}

//Imagen en sprite gris
void test04() {
  Serial.println("test04. begin");
  //Cargaremos un bmp color en un sprite gris y lo enviaremos al display
  RoJoSprite3 sprite(1); //Sprite de tonos de gris
  sprite.begin(); //Inicialización
  byte errorCode=sprite.loadBMP("/betty.bmp"); //Cargamos un bmp color que se transforma a grises
  Serial.println("test04. errorCode="+String((int)errorCode));
  display.drawSpriteG(&sprite); //Mostramos sprite gris
  sprite.end();
  display.show();
  Serial.println("test04. end");
  delay(2000);
}

void setup() {
  Serial.begin(115200);
  display.begin(pinRES,pinDC,pinCS,pinBUSY);
  Serial.println("setup begin");

  test01(); //Carga directa de bmp
  #ifndef ARDUINO_AVR_UNO //Arduino UNO no tiene memoria para los siguientes test
    test02(); //Sprites desplazados
    test03(); //Sprites a capas
    test04(); //Sprite gris
  #endif
  display.sleep(); //Hemos terminado. Hibernamos el display
  Serial.println("setup end");
}

void loop() {
  //Nada que hacer aquí
};
