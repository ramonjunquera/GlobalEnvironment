/*
  Autor: Ramón Junquera
  Fecha: 20180409
  Tema: Librería para display RPI_ILI9486 SPI 3.5" 480*320
  Objetivo: Demo de librería RoJoILI9486
  Material: breadboard, cables, placa ESP, display ILI9486

  Descripción:
  Descripción y demostración de funcionalidades de la librería.

  Pinout SPI:

  Modelo   CLK MISO MOSI CS
  -------  --- ---- ---- --
  ESP32    18   19   23  5
  ESP8266  D5   D6   D7  D8
  RPi      11   9    10  8(CS0)
  
  En nuestro caso, aunque el dispositivo no devuelva nunca información (no necesita MISO),
  no podemos utilizar ese pin porque es utilizado por el protocolo SPI.

  Nota:
  El sistema de archivos interno debe contener los archivos de la carpeta data para
  su correcto funcionamiento.
  Se puede utilizar el plugin integrado en el IDE para facilitar la subida.
 
  Resultado:
  Realizamos varios tests cíclicos
*/

#include <Arduino.h>
#include "RoJoILI9486.h" //Librería de gestión del display ILI9486
#include "RoJoSprite16.h" //Gestión de sprites color
#include "RoJoABC16.h" //Gestión de fuentes color

//Definición de pines
#ifdef ESP8266 //Si es un ESP8266...
  const byte pinDC_display=D3;
  const byte pinRES_display=D4;
  const byte pinCS_display=D8; //CS por defecto
#elif defined(ESP32) //Si es un ESP32...
  const byte pinDC_display=5;
  const byte pinRES_display=22;
  const byte pinCS_display=21;
#endif

//Creamos objeto de gestión
RoJoILI9486 display;

void Test0()
{
  //Test de color

  //Limpiamos pantalla
  display.clear();
  //Definimos colores
  uint16_t redColor = display.getColor(255,0,0);
  uint16_t greenColor = display.getColor(0,255,0);
  uint16_t blueColor = display.getColor(0,0,255);
  uint16_t yellowColor = display.getColor(255,255,0);
  uint16_t magentaColor = display.getColor(255,0,255);
  uint16_t cyanColor = display.getColor(0,255,255);
  display.rect(0,0,100,100,redColor);
  display.rect(100,0,100,100,greenColor);
  display.rect(200,0,100,100,blueColor);
  display.rect(0,100,100,100,yellowColor);
  display.rect(100,100,100,100,magentaColor);
  display.rect(200,100,100,100,cyanColor);

  delay(1000);
}

void Test1()
{
  //Función getColor y drawPixel
  
  //Limpiamos pantalla
  display.clear();
  //Obtenemos el código de color
  uint16_t blueColor = display.getColor(0,0,255);
  //Dibujamos una matriz de puntos
  for(int x=0;x<320;x+=10)
    for(int y=0;y<480;y+=10)
      display.drawPixel(x,y,blueColor);

  delay(1000);
}

void Test2()
{
  //Cargar y mostrar un sprite
  
  //Limpiamos pantalla
  display.clear();
  
  //Creamos el sprite
  RoJoSprite16 mySprite;
  //Lo llenamos con una imagen
  //Las imágenes de ejemplo se guardan en la carpeta spr16
  //Los sprites color tienen extensión .s16
  //Pueden ser generados con la aplicación RoJoImageConvert
  //También se han dejado las imágenes .bmp originales
  mySprite.load(F("/rose.s16"));
  //Lo mostramos
  display.drawSprite(0,0,&mySprite);
  //Repetimos el proceso con otros dos, utilizando el mismo objeto
  mySprite.load(F("/mountain.s16"));
  display.drawSprite(100,0,&mySprite);
  mySprite.load(F("/rainbow.s16"));
  display.drawSprite(200,0,&mySprite);
  //Borramos el sprite utilizado
  mySprite.clean();

  //Repetimos la secuencia pero ahora utilizamos la función para dibujar
  //un sprite directamente desde un archivo, sin necesidad de cargarlo
  //en memoria.
  display.drawSprite(0,100,F("/rose.s16"));
  display.drawSprite(100,100,F("/mountain.s16"));
  display.drawSprite(200,100,F("/rainbow.s16"));

  delay(2000);
}

void Test3()
{
  //Desplazar un sprite escribiendo directamente en pantalla

  //Crearemos un spritecon una fila y columna más que la imagen cargada
  //Puesto que moveremos el sprite en diagonal, esto servirá para que
  //tapen (borren) el rastro del sprote anterior

  //Creamos el sprite final
  RoJoSprite16 finalSprite;
  //Sabemos que la imagen que cargaremos tendrá un tamaño de 96x64
  //Así que dimensionamos el sprite final a 97x65
  finalSprite.setSize(97,65);
  //Creamos el sprite con la imagen
  RoJoSprite16 imageSprite;
  //Cargamos la imagen
  imageSprite.load(F("/heart.s16"));
  //Copiamos la imagen sobre el sprite final
  finalSprite.drawSprite(1,1,&imageSprite);
  //Borramos el sprite de imagen. Ya no lo necesitaremos
  imageSprite.clean();
  //Limpiamos pantalla
  display.clear();
  //Definimos el recorrido
  for(int16_t i=-98;i<display.xMax();i++)
  {
    //Dibujamos el sprite en la nueva posición
    display.drawSprite(i,i,&finalSprite);
  }
  //Borramos el sprite utilizado
  finalSprite.clean();
}

void Test4()
{
  //Líneas
  
  //Limpiamos pantalla
  display.clear();
  
  uint16_t greenColor = display.getColor(0,255,0);
  for(int16_t x=0;x<320;x+=10) display.line(0,0,x,479,greenColor);
  uint16_t blueColor = display.getColor(0,0,255);
  for(int16_t x=0;x<320;x+=10) display.line(0,479,x,0,blueColor);    
    
  delay(1000);
}

void Test5()
{
  //Mostrar texto

  //Limpiamos pantalla
  display.clear();

  //Definimos color de texto
  uint16_t color = display.getColor(0,255,0); //green
  //Dibujamos el texto
  display.print(F("/5x7d.fon"),F("20180330"),color,0,0);
  //Utilizaremos otra fuente más grande
  display.print(F("/10x15d.fon"),F("20180330"),color,0,8); 
  //Cambiamos el color del texto
  color = display.getColor(255,255,0); //yellow
  //Imprimimos fuera de pantalla
  display.print(F("/10x15d.fon"),F("1234567890"),color,-5,25); //No se ve el principio
  display.print(F("/10x15d.fon"),F("1234567890"),color,240,25); //No se ve el final
  //Fuente más grande
  display.print(F("/21x33d.fon"),F("1234567890"),color,0,45); //No se ve el principio
  //Texto
  color=display.getColor(255,0,0); //red
  display.print(F("/7x11.fon"),F("Hola, esto es una prueba!"),color,0,80);

  delay(3000);
}

void Test6()
{
  //Rotación de pantalla

  //Limpiamos pantalla
  display.clear();

  //Definimos color de texto
  uint16_t color = display.getColor(255,255,255); //white
  //Dibujamos el texto para cada rotación
  display.rotation(0);
  display.print(F("/7x11.fon"),F("Rotation 0"),color,0,0);
  display.rotation(1);
  display.print(F("/7x11.fon"),F("Rotation 1"),color,0,0);
  display.rotation(2);
  display.print(F("/7x11.fon"),F("Rotation 2"),color,0,0);
  display.rotation(3);
  display.print(F("/7x11.fon"),F("Rotation 3"),color,0,0);
  display.rotation(0);

  delay(3000);
}

void Test7()
{
  //Cargar y mostrar un sprite a pantalla completa

  //Mostramos varios sprites grandes (pantalla completa) con la función de cargar directamente de archivo
  //a display sin pasar por memoria 
  display.drawSprite(0,0,F("/arbol.s16")); delay(2000);
  display.drawSprite(0,0,F("/campo.s16")); delay(2000);
  display.drawSprite(0,0,F("/ghosts.s16")); delay(2000);
}

void setup()
{
  Serial.begin(115200);
  //Inicializamos el display
  display.begin(pinRES_display,pinDC_display,pinCS_display);
  //Test1();
  //Serial.println("SEND_16_BITS="+String(SEND_16_BITS));
}

void loop()
{
  Test0(); //Test de color
  Test1(); //Función getColor y drawPixel
  Test2(); //Cargar y mostrar un sprite
  Test3(); //Desplazar un sprite escribiendo sobre otro sprite
  Test4(); //Lineas
  Test5(); //Mostrar texto
  Test6(); //Rotación de pantalla
  Test7(); //Sprites a pantalla completa
}

