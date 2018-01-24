/*
  Autor: Ramón Junquera
  Tema: Librería para display OLED I2C 0.96" 128x64 SSD1306
  Objetivo: Demo de librería RoJoSSD1306
  Fecha: 20180124
  Material: breadboard, cables, placa RPi, display OLED I2C SSD1306

  Descripción:
  Descripción y demostración de funcionalidades de la librería.

  I2C pinout:

  device  SCL SDA
  ------- --- ---
  ESP8266 D1  D2
  ESP32   22  21
  RPi 0   28  27 -> ID_SC & ID_SD    
  RPi 1    5   3 -> GPIO3 & GPIO2

  Nota:
  El sistema de archivos interno debe contener los archivos de la carpeta data para
  su correcto funcionamiento.
  Se puede utilizar el plugin integrado en el IDE para facilitar la subida.
 
  Resultado:
  Realizamos varios tests cíclicos
*/

#include <Arduino.h>
#include "RoJoSSD1306.cpp" //Librería de gestión del display SSD1306
#include "RoJoSprite.cpp" //Librería de gestión de sprites monocromos
#include "RoJoABC.cpp" //Gestión de fuentes

//Creamos objeto de gestión
RoJoSSD1306 display;

void Test1()
{
  //Función setPixel con color 1 = dibujar

  //Limpiamos pantalla
  display.clear();
  //Dibujamos una matriz de puntos
  for(int x=0;x<10;x++)
    for(int y=0;y<10;y++)
      display.setPixel(x*5,y*5,1);
  //Mostramos el resultado
  display.show();
}

void Test2()
{
  //Función setPixel con color 2 = invertir
  
  //Dibujamos un rectángulo relleno sobre la matriz anterior que invierte los pixels
  for(int x=0;x<46;x++)
    for(int y=0;y<46;y++)
      display.setPixel(x,y,2);
  //Mostramos el resultado
  display.show();
}

void Test3()
{
  //Función setPixel con color 0 = borrar
  
  //Dibujamos un rectángulo relleno en el interior que borra
  for(int x=10;x<36;x++)
    for(int y=10;y<36;y++)
      display.setPixel(x,y,0);
  //Mostramos el resultado
  display.show();
}

void Test4()
{
  //Función getPixel
  
  //Del rectángulo que contiene todo, invertimos los puntos
  for(int x=0;x<46;x++)
    for(int y=0;y<46;y++)
      display.setPixel(x,y,!display.getPixel(x,y));
  //Mostramos el resultado
  display.show();
}

void Test5()
{
  //Función lineV

  //Limpiamos pantalla
  display.clear();
  //Dibujamos líneas verticales de distintas longitudes
  for(byte i=0;i<50;i++) display.lineV(i,i/2,i,1);
  //Mostramos el resultado
  display.show();
}

void Test6()
{
  //Función lineH

  //Limpiamos pantalla
  display.clear();
  //Dibujamos líneas horizontales de distintas longitudes
  for(byte i=0;i<50;i++) display.lineH(i,i/2,i,1);
  //Mostramos el resultado
  display.show();
}

void Test7()
{
  //Rectángulos

  //Limpiamos pantalla
  display.clear();
  //Dibujamos un rectángulo relleno sin borde
  display.rect(0,0,20,20,1); //1=sólido
  //Dibujamos un rectángulo con borde inverso y sin relleno
  display.rect(10,10,30,30,3,2); //3=transparente,2=inverso

  //Dibujamos un rectángulo relleno sin borde
  display.rect(40,0,60,20,1); //1=sólido
  //Dibujamos un rectángulo relleno con borde inverso
  display.rect(50,10,70,30,1,2); //1=sólido,2=inverso
  
  //Dibujamos un rectángulo con borde y sin relleno
  display.rect(0,33,20,53,3,1); //3=transparente,1=sólido
  //Dibujamos un rectángulo relleno de inverso y sin borde
  display.rect(10,43,30,63,2); //2=inverso

  //Dibujamos un rectángulo relleno sin borde
  display.rect(40,33,60,53,1); //1=sólido
  //Dibujamos un rectángulo con borde vacío y sin relleno
  display.rect(50,43,70,63,3,0); //3=transparente,0=vacío=negro
  
  //Mostramos el resultado
  display.show();
}

void Test8()
{
  //Líneas

  //Limpiamos pantalla
  display.clear();
  for(byte x=0;x<128;x+=5) display.line(0,0,x,63,1);
  //Mostramos el resultado
  display.show();
}

void Test9()
{
  //Sprites definidos en programa.
  //Función de display: drawSpritePage
  //Función de sprite: setPage
  
  //Limpiamos pantalla
  display.clear();
  //Creamos el sprite
  RoJoSprite mySprite;
  //Lo dimensionamos. Anchura=7. Páginas=1
  mySprite.setSize(7,1);
  //Lo dibujamos
  //void setPage(int16_t x,int16_t page,byte mask,byte color);
  mySprite.setPage(0,0,0b00111110,4); //4=escribir el valor tal cual
  mySprite.setPage(1,0,0b01000001,4);
  mySprite.setPage(2,0,0b01010101,4);
  mySprite.setPage(3,0,0b01010001,4);
  mySprite.setPage(4,0,0b01010101,4);
  mySprite.setPage(5,0,0b01000001,4);
  mySprite.setPage(6,0,0b00111110,4);

  //Lo dibujamos varias veces
  for(byte y=0;y<display.yMax;y+=8)
    for(byte x=0;x<display.xMax;x+=10)
      display.drawSpritePage(x,y/8,&mySprite,1);
  //Mostramos el resultado
  display.show();
  //Borramos el sprite utilizado
  mySprite.clean();
}

void Test10()
{
  //Función de sprite: setPixel y getPixel
  
  //Limpiamos pantalla
  display.clear();
  //Creamos el sprite
  RoJoSprite mySprite;
  //Lo dimensionamos. Anchura=7. Páginas=1
  mySprite.setSize(30,3);
  //Dibujamos el marco de grosor 2
  for(byte x=0;x<30;x++)
  {
    mySprite.setPixel(x,0,1);
    mySprite.setPixel(x,1,1);
    mySprite.setPixel(x,22,1);
    mySprite.setPixel(x,23,1); //Son 3 páginas de altura (3*8=24)
  }
  for(byte y=0;y<24;y++)
  {
    mySprite.setPixel(0,y,1);
    mySprite.setPixel(1,y,1);
    mySprite.setPixel(28,y,1);
    mySprite.setPixel(29,y,1);
  }

  //Lo dibujamos
  display.drawSpritePage(0,0,&mySprite,1);
  //Lo volvemos a dibujar desplazado y en modo inverso
  //Las intersecciones de ambos deberían estar vacías
  display.drawSpritePage(10,1,&mySprite,2);
  //Dibujamos una línea horizontal en el medio, pero tendremos en cuenta los pixel existentes
  //Si ya hay algo dibujado, lo borraremos (lo invertimos manualmente)
  for(byte x=0;x<30;x++)
  {
    //Si hay un pixel dibujado...
    if(mySprite.getPixel(x,10))
      //...lo borraremos
      mySprite.setPixel(x,10,0);
    else
      //..lo dibujaremos
      mySprite.setPixel(x,10,1);
  }
  //Dibujamos el nuevo sprite en pantalla
  display.drawSpritePage(50,0,&mySprite,1);
  //Mostramos el resultado
  display.show();
  //Borramos el sprite utilizado
  mySprite.clean();
}

void Test11()
{
  //Leer sprite desde un archivo
  //Modo inverso

  //En el sprite que vamos a leer el dibujo lo hacen los pixels activos
  //En dibujo original estaba hecho en negro sobre fondo blanco
  //Es justo al revés de lo que muestra el display
  //Lo normal es que una pantalla vacía sera de color negro
  //Por eso desactivaremso el modo inverso
  display.reverse(false);

  //Limpiamos pantalla
  display.clear();
  //Creamos el sprite
  RoJoSprite mySprite;
  //Lo leemos desde el archivo
  mySprite.load(F("spr/mickey.spr"));
  //Dibujamos el sprite en pantalla
  display.drawSpritePage(0,0,&mySprite,1);
  //Mostramos el resultado
  display.show();
  //Borramos el sprite utilizado
  mySprite.clean();
}

void Test12()
{
  //Mover sprite por pantalla = drawSprite

  //Seguiremos con el modo inverso desactivado (pixels en negro)
  display.reverse(false);
  //Creamoss un nuevo sprite
  RoJoSprite ball;
  //Leemos su contenido desde un archivo
  ball.load(F("spr/ball30x30.spr")); //Su tamaño es x=30,y=30,pages=4
  //Coordenadas
  byte x=0;
  byte y=0;
  //Deltas
  int8_t dx=1;
  int8_t dy=1;
  //Calculamos el tiempo máximo de duración de esta demo
  uint32_t tMax=millis()+5000; //5 segundos
  //Bucle principal
  while(millis()<tMax)
  {
    //Borramos la pantalla
    display.clear();
    //Dibujamos el sprite
    display.drawSprite(x,y,&ball,1);
    //Refrescamos pantalla
    display.show();
    //Calculamos las nuevas coordenadas
    x+=dx, y+=dy;
    //Si hemos tocado con un borde, cambiamos la dirección
    if(x==0 || x==97) dx=-dx; //97=128-30-1
    if(y==0 || y==33) dy=-dy; //33=64-30-1
  }
  //Borramos el sprite utilizado
  ball.clean();
}

void Test13()
{
  //Mover sprite por pantalla manteniendo una imagen de fondo
  //Nota: los pixel apagados del sprite en movimiento aparecen transparentes

  //Seguiremos con el modo inverso desactivado (pixels en negro)
  display.reverse(false);
  //Creamos un nuevo sprite para el fondo
  RoJoSprite backSprite;
  //Leemos su contenido desde un archivo
  backSprite.load(F("spr/mickey.spr"));
  //Creamos un nuevo sprite para el objeto en movimiento
  RoJoSprite ball;
  //Leemos su contenido desde un archivo
  ball.load(F("spr/ball30x30.spr")); //Su tamaño es x=30,y=30,pages=4
  //Coordenadas
  byte x=0;
  byte y=0;
  //Deltas
  int8_t dx=1;
  int8_t dy=1;
  //Calculamos el tiempo máximo de duración de esta demo
  uint32_t tMax=millis()+4000; //4 segundos
  //Bucle principal
  while(millis()<tMax)
  {
    //Dibujamos el sprite de fondo sobreescribiendo lo que haya
    display.drawSpritePage(0,0,&backSprite,4); //4=sobreescribir
    //Dibujamos el sprite en movimiento
    display.drawSprite(x,y,&ball,1);
    //Refrescamos pantalla
    display.show();
    //Calculamos las nuevas coordenadas
    x+=dx, y+=dy;
    //Si hemos tocado con un borde, cambiamos la dirección
    if(x==0 || x==97) dx=-dx; //97=128-30-1
    if(y==0 || y==33) dy=-dy; //33=64-30-1
  }
  //Borramos los sprites utilizados
  ball.clean();
  backSprite.clean();
}

void Test14()
{
  //Mover sprite por pantalla manteniendo una imagen de fondo
  //y que no sea transparente
  //Para conseguir esto, necesitamos una imagen que nos haga de máscara
  //Esta máscara permitirá borrar antes de dibujar el sprite definitivo

  //Seguiremos con el modo inverso desactivado (pixels en negro)
  display.reverse(false);
  //Creamos un nuevo sprite para el fondo
  RoJoSprite backSprite;
  //Leemos su contenido desde un archivo
  backSprite.load(F("spr/mickey.spr"));
  //Creamos un nuevo sprite para el objeto en movimiento
  RoJoSprite ball;
  //Leemos su contenido desde un archivo
  ball.load(F("spr/ball30x30.spr")); //Su tamaño es x=30,y=30,pages=4
  //Creamos un nuevo sprite para la máscara del objeto en movimiento
  RoJoSprite ballMask;
  //Leemos su contenido desde un archivo
  ballMask.load(F("spr/ball30x30mask.spr")); //Su tamaño es x=30,y=30,pages=4
  //Coordenadas
  byte x=0;
  byte y=0;
  //Deltas
  int8_t dx=1;
  int8_t dy=1;
  //Calculamos el tiempo máximo de duración de esta demo
  uint32_t tMax=millis()+4000; //4 segundos
  //Bucle principal
  while(millis()<tMax)
  {
    //Dibujamos el sprite de fondo sobreescribiendo lo que haya
    display.drawSpritePage(0,0,&backSprite,4); //4=sobreescribir
    //Dibujamos la márcara del sprite en movimiento borrando
    display.drawSprite(x,y,&ballMask,0);
    //Dibujamos el sprite en movimiento
    display.drawSprite(x,y,&ball,1);
    //Refrescamos pantalla
    display.show();
    //Calculamos las nuevas coordenadas
    x+=dx, y+=dy;
    //Si hemos tocado con un borde, cambiamos la dirección
    if(x==0 || x==97) dx=-dx; //97=128-30-1
    if(y==0 || y==33) dy=-dy; //33=64-30-1
  }
  //Borramos los sprites utilizados
  ballMask.clean();
  ball.clean();
  backSprite.clean();
}

void Test15()
{
  //Obtener sprite de la pantalla

  //Seguiremos con el modo inverso desactivado (pixels en negro)
  display.reverse(false);
  //Creamos un nuevo sprite
  RoJoSprite mySprite;
  //Leemos su contenido desde un archivo
  mySprite.load(F("spr/mickey.spr"));
  //Dibujamos el sprite de fondo sobreescribiendo lo que haya
  display.drawSpritePage(0,0,&mySprite,4); //4=sobreescribir
  //Recortamos parte de la pantalla actual y lo guardamos en el mismo sprite
  display.getSprite(50,2,70,5,&mySprite);
  //Dibujamos el sprite recortado
  display.drawSpritePage(0,0,&mySprite,4); //4=sobreescribir
  //Refrescamos pantalla
  display.show();
  //Borramos el sprite utilizado
  mySprite.clean();
}

void Test16()
{
  //Guardar un sprite

  //Seguiremos con el modo inverso desactivado (pixels en negro)
  display.reverse(false);
  //Creamos un nuevo sprite
  RoJoSprite mySprite;
  //Leemos su contenido desde un archivo
  mySprite.load(F("spr/mickey.spr"));
  //Dibujamos el sprite de fondo sobreescribiendo lo que haya
  display.drawSpritePage(0,0,&mySprite,4); //4=sobreescribir
  //Recortamos parte de la pantalla actual y lo guardamos en el mismo sprite
  display.getSprite(50,2,70,5,&mySprite);
  //Guardamos el sprite en un archivo
  mySprite.save(F("spr/mick2.spr"));
  //Borramos el sprite utilizado
  mySprite.clean();
  //Creamos un nuevo sprite
  RoJoSprite mySprite2;
  //Leemos su contenido desde el archivo
  mySprite2.load(F("spr/mick2.spr"));
  //Borramos la pantalla
  display.clear();
  //Dibujamos el sprite varias veces
  for(byte x=0;x<6;x++)
    for(byte page=0;page<2;page++)
      display.drawSpritePage(x*20,page*4,&mySprite2,4);
  //Refrescamos pantalla
  display.show();
  //Borramos el sprite utilizado
  mySprite2.clean();
  //Borramos el archivo
  SPIFFS.remove(F("spr/mick2.spr"));
}

void Test17()
{
  //Resize

  //Seguiremos con el modo inverso desactivado (pixels en negro)
  display.reverse(false);
  //Creamos un nuevo sprite
  RoJoSprite mySprite;
  //Leemos su contenido desde un archivo
  mySprite.load(F("spr/mickey.spr"));
  //Creamos el sprite utilizado para el redimensionado
  RoJoSprite resizeSprite;
  //Limpiamos pantalla
  display.clear();
  //Reduciremos la imagen desde un 5% hasta un 100%
  for(uint16_t r=5;r<=100;r++)
  {
    //Calculamos el sprite redimensionado al factor actual
    resizeSprite.resize((display.xMax*r)/100,(display.yMax*r)/100,&mySprite);
    //Dibujamos el sprite redimensionado en pantalla
    display.drawSpritePage(0,0,&resizeSprite,4); //4=sobreescribir
    //Refrescamos pantalla
    display.show();
    //Esperamos un momento
    delay(10);
  }
  //Borramos los sprites utilizados
  resizeSprite.clean();
  mySprite.clean();
}

void Test18()
{
  //Líneas dibujadas en sprite

  //Activamos el modo inverso, que se utiliza por defecto (pixel = blanco)
  display.reverse(true);
  //Borramos la pantalla
  display.clear();
  //Creamoss un nuevo sprite
  RoJoSprite mySprite;
  //Será tan grande como la pantalla
  mySprite.setSize(128,8);
  //Dibujamos unas líneas
  for(byte x=0;x<128;x+=5) mySprite.line(0,0,x,63,1);
  //Dibujamos el sprite en pantalla
  display.drawSprite(0,0,&mySprite,1);
  //Refrescamos pantalla
  display.show();
  //Borramos el sprite utilizado
  mySprite.clean();
}

void Test19()
{
  //Mostrar texto

  //Limpiamos pantalla
  display.clear();
  
  //Creamos el sprite que contendrá el texto
  RoJoSprite textSprite;
  //Creamos objeto de gestión de fuentes
  RoJoABC font;
  //Creamos el sprite con el texto
  //Si no podemos crear el sprite de texto...hemos terminado  
  if(!font.print(F("fon/RoJoABC5x7digits.fon"),F("20171101"),&textSprite)) return;
  //Lo mostramos
  display.drawSpritePage(0,0,&textSprite,4);

  //Utilizaremos otra fuente más grande
  //Reaprovechamos el objeto de gestión de fuentes
  //Creamos el sprite con el texto
  //Si no podemos crear el sprite de texto...hemos terminado
  if(!font.print(F("fon/RoJoABC10x15digits.fon"),F("20171101"),&textSprite)) return;
  //Lo mostramos
  display.drawSpritePage(0,2,&textSprite,4);
  
  //Creamos otro texto más grande que la pantalla
  //Si no podemos crear el sprite de texto...hemos terminado
  if(!font.print(F("fon/RoJoABC10x15digits.fon"),F("1234567890123456"),&textSprite)) return;
  //Lo mostramos. No se verá el final
  display.drawSpritePage(0,4,&textSprite,4);
  //Lo mostramos de nuevo desplazado a la izquierda, comenzando desde una
  //posición fuera de pantalla
  display.drawSpritePage(-35,6,&textSprite,4);
  //Refrescamos pantalla
  display.show();
  
  //Borramos el sprite utilizado
  textSprite.clean();
}

void Test20()
{
  //Redimensionar texto

  //Limpiamos pantalla
  display.clear();
  //Creamos el sprite de tamaño normal
  RoJoSprite normalSprite;
  //Creamos objeto de gestión de fuentes
  RoJoABC font;
  //Creamos un sprite con texto
  //Si no podemos crear el sprite de texto...hemos terminado
  if(!font.print(F("fon/RoJoABC5x7digits.fon"),F("2017"),&normalSprite)) return;
  //Creamos un nuevo sprite para el redimensionado
  RoJoSprite resizeSprite;
  //Redimensionamos el sprite de texto. Lo hacemos 4 veces más grande
  resizeSprite.resize(normalSprite.width()*4,normalSprite.heightPages()*4*8,&normalSprite);
  //Mostramos el sprite redimensionado en pantalla
  display.drawSpritePage(0,0,&resizeSprite,4);
  //Refrescamos pantalla
  display.show();
  delay(1000);

  //Redimensionamos el sprite de texto
  //El tamaño final será de la mitad superior de pantalla
  //Quedará un poco deformado (estirado en horizontal)
  resizeSprite.resize(display.xMax,display.yMax/2,&normalSprite);
  //Mostramos el sprite redimensionado en pantalla. En la parte superior
  display.drawSpritePage(0,0,&resizeSprite,4);
  //En la parte inferior
  display.drawSpritePage(0,display.yMax/2/8,&resizeSprite,4);
  //Refrescamos pantalla
  display.show();
  delay(1000);

  //Redimensionamos el sprite de texto
  //El tamaño final será de la mitad izquierda de pantalla
  //Quedará deformado (estirado en vertical)
  resizeSprite.resize(display.xMax/2,display.yMax,&normalSprite);
  //Mostramos el sprite redimensionado en pantalla. En la parte izquierda
  display.drawSpritePage(0,0,&resizeSprite,4);
  //En la parte derecha
  display.drawSpritePage(display.xMax/2,0,&resizeSprite,4);
  //Refrescamos pantalla
  display.show();
  delay(1000);

  //Redimensionamos el sprite de texto
  //El tamaño final será la pantalla completa
  resizeSprite.resize(display.xMax,display.yMax,&normalSprite);
  //Mostramos el sprite redimensionado en pantalla
  display.drawSpritePage(0,0,&resizeSprite,4);
  //Refrescamos pantalla
  display.show();
  delay(1000);

  //Borramos los sprites utilizados
  resizeSprite.clean();
  normalSprite.clean();
}

int main(int argc, char **argv)
{
  //Inicializamos el display
  display.begin();
	
  while(1)
  {
	  Test1();
	  delay(1000);
	  Test2();
	  delay(1000);
	  Test3();
	  delay(1000);
	  Test4();
	  delay(1000);
	  Test5();
	  delay(1000);
	  Test6();
	  delay(1000);
	  Test7();
	  delay(1000);
	  Test8();
	  delay(1000);
	  Test9();
	  delay(1000);
	  Test10();
	  delay(1000);
	  Test11();
	  delay(1000);
	  Test12();
	  Test13();
	  Test14();
	  Test15();
	  delay(1000);
	  Test16();
	  delay(1000);
	  Test17();
	  delay(1000);
	  Test18();
	  delay(1000);
	  Test19();
	  delay(1000);
	  Test20();	  
  }
}

