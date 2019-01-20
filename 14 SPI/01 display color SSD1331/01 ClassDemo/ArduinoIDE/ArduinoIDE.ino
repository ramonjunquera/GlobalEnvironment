/*
  Autor: Ramón Junquera
  Fecha: 20180524
  Tema: Librería para display OLED SPI 0.95" 96x64 SSD1331
  Objetivo: Demo de librería RoJoSSD1331
  Material: breadboard, cables, placa ESP, display OLED SSD1331

  Descripción:
  Descripción y demostración de funcionalidades de la librería.

  Pinout SPI:

  Modelo   CLK MISO MOSI CS
  -------  --- ---- ---- --
  ESP32    18   19   23  5
  ESP8266  D5   D6   D7  D8
  RPi      11   9    10  8(CS0)
  
  Cuando se programa un ESP en el entorno de Arduino IDE, se respetan los pines
  seleccionados por hardware. Incluido CS que se gestiona internamente.

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
#include "RoJoSSD1331.h" //Librería de gestión del display SSD1331
#include "RoJoSprite16.h" //Gestión de sprites color
#include "RoJoABC16.h" //Gestión de fuentes

//Definición de pines
#ifdef ESP8266 //Si es un ESP8266...
  const byte pinDC_display=D2;
  const byte pinRES_display=D0;
  const byte pinCS_display=D8; //CS por defecto
#elif defined(ESP32) //Si es un ESP32...
  const byte pinDC_display=22;
  const byte pinRES_display=2;
  const byte pinCS_display=5; //CS por defecto
#endif

//Creamos objeto de gestión
RoJoSSD1331 display;

void Test0()
{
  //Test de color
  
  //Limpiamos pantalla
  display.clear();
  //Definimos colores
  uint16_t whiteColor = display.getColor(255,255,255);
  uint16_t redColor = display.getColor(255,0,0);
  uint16_t greenColor = display.getColor(0,255,0);
  uint16_t blueColor = display.getColor(0,0,255);
  uint16_t yellowColor = display.getColor(255,255,0);
  uint16_t magentaColor = display.getColor(255,0,255);
  uint16_t cyanColor = display.getColor(0,255,255);
  display.rect(0,0,30,30,whiteColor,redColor);
  display.rect(30,0,60,30,whiteColor,greenColor);
  display.rect(60,0,90,30,whiteColor,blueColor);
  display.rect(0,30,30,60,whiteColor,yellowColor);
  display.rect(30,30,60,60,whiteColor,magentaColor);
  display.rect(60,30,90,60,whiteColor,cyanColor);

  delay(1000);
}

void Test1()
{
  //Función GetColor y SetPixel
  
  //Limpiamos pantalla
  display.clear();
  //Obtenemos el código de color
  uint16_t blueColor = display.getColor(0,0,255);
  //Dibujamos una matriz de puntos
  for(int x=0;x<10;x++)
    for(int y=0;y<10;y++)
      display.drawPixel(x*5,y*5,blueColor);

  delay(1000);
}

void Test2()
{
  //Función de cursor
  
  //Limpiamos pantalla
  display.clear();
  //Obtenemos el código de color
  uint16_t greenColor = display.getColor(0,255,0);
  //Dibujamos una matriz de puntos
  for(int x=0;x<10;x++)
    for(int y=0;y<10;y++)
    {
      //Posicionamos el cursor
      display.setCursorRangeX(x*5,x*5);
      display.setCursorRangeY(y*5,y*5);
      //Dibujamos el pixel
      display.drawPixel(greenColor);
    }

  delay(1000);
}

void Test3()
{
  //Rangos de cursor
  
  //Limpiamos pantalla
  display.clear();
  //Definimos un rango horizontal para el cursor de anchura 10
  display.setCursorRangeX(10,19);
  //Definimos un rango vertical para el cursor de anchura 32
  display.setCursorRangeY(10,41);
  //Recorremos las 32 filas
  for(int y=0;y<32;y++)
  {
    //Recorremos las 10 columnas
    for(int x=0;x<10;x++)
    {
      //Dibujamos el pixel sin posicionarlo, porque se moverá
      //dentro del rango definido
      //El color siempre será rojo, pero irá¡ variando en intensidad
      display.drawPixel(display.getColor((y+1)*8-1,0,0));
    }
  }

  delay(1000);
}

void Test4()
{
  //Líneas
  
  //Limpiamos pantalla
  display.clear();
  
  uint16_t greenColor = display.getColor(0,255,0);
  for(byte x=0;x<96;x+=3) display.line(0,0,x,63,greenColor);
  uint16_t blueColor = display.getColor(0,0,255);
    for(byte x=0;x<96;x+=3) display.line(0,63,x,0,blueColor);    
    
  delay(1000);
}

void Test5()
{
  //Rectángulos
  
  //Limpiamos pantalla
  display.clear();
  
  uint16_t borderColor = display.getColor(255,255,255); //white
  uint16_t fillColor = display.getColor(0,0,255); //blue
  //Dibujamos una matriz de 5x5 rectángulos con borde y relleno
  for(int x=0;x<50;x+=10)
    for(int y=0;y<50;y+=10)
      display.rect(x,y,x+8,y+8,borderColor,fillColor);
  
  borderColor = display.getColor(255,0,0); //red
  //Dibujamos un rectángulo sin relleno
  display.rect(15,15,35,35,borderColor);

  delay(1000);
}

void Test6()
{
  //Función Copy
  
  //Limpiamos pantalla
  display.clear();
  //Dibujamos un rectángulo
  uint16_t borderColor = display.getColor(255,255,255); //white
  uint16_t fillColor = display.getColor(0,0,255); //blue
  display.rect(0,0,8,8,borderColor,fillColor);
  //Lo copiamos 5 veces
  for(int i=0;i<5;i++) display.copy(0,0,8,8,i*10,10);

  delay(1000);
}

void Test7()
{
  //Función Darker
  
  //Limpiamos pantalla
  display.clear();
  //Dibujamos un rectángulo
  uint16_t borderColor = display.getColor(255,255,255); //white
  uint16_t fillColor = display.getColor(0,0,255); //blue
  display.rect(0,0,50,50,borderColor,fillColor);
  //Oscurecemos dos areas que se solapan
  display.darker(10,10,30,30);
  display.darker(20,20,40,40);
  //Comprobamos que una zona oscurecida ya no se oscurece más

  delay(1000);
}

void Test8()
{
  //Función Clear
  
  //Limpiamos pantalla
  display.clear();
  //Dibujamos un rectángulo
  uint16_t borderColor = display.getColor(255,255,255); //white
  uint16_t fillColor = display.getColor(0,0,255); //blue
  display.rect(0,0,40,40,borderColor,fillColor);
  //Borramos parte de su interior dibujando un rectángulo relleno negro
  display.rect(10,10,30,30,0,0);

  delay(1000);
}

void Test9()
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
  //Borramos el sprite utilizado
  mySprite.clean();

  delay(1000);
}

void Test10()
{
  //Desplazar un sprite escribiendo directamente en pantalla
  
  //Creamos el sprite
  RoJoSprite16 mySprite;
  //Lo llenamos con una imagen
  mySprite.load(F("/rainbow.s16"));
  //Definimos el recorrido
  for(int i=-70;i<70;i++)
  {
    //Limpiamos pantalla
    display.clear();
    //Dibujamos el sprite en la nueva posición
    display.drawSprite(i,i,&mySprite);
    //Esperamos un momento
    delay(5);
  }
  //Borramos el sprite utilizado
  mySprite.clean();
}

void Test11()
{
  //Desplazar un sprite escribiendo sobre otro sprite

  //Creamos el sprite de fondo
  RoJoSprite16 backSprite;
  //Con el mismo tamaño que la pantalla
  backSprite.setSize(display.xMax,display.yMax);
  //Creamos el sprite que se moverá
  RoJoSprite16 moveSprite;
  //Lo llenamos con una imagen
  moveSprite.load(F("/rainbow.s16")); 
  //Definimos el recorrido
  for(int i=-70;i<70;i++)
  {
    //Borramos la imagen del sprite de fondo
    backSprite.clear();
    //Copiamos el sprite en movimiento sobre el de fondo
    backSprite.drawSprite(i,i,&moveSprite);
    //Mostramos el sprite de fondo
    display.drawSprite(0,0,&backSprite);
    //Esta vez no esperamos, porque el desplazamiento no es muy rápido
    //Sí permitimos que se lancen los procesos de fondo para que no se bloquee
    yield();
  }
  //Borramos los sprites utilizados
  backSprite.clean();
  moveSprite.clean();
}

void Test12()
{
  //Función de guardado de sprites

  //Creamos el sprite base del que recortaremos un trozo
  RoJoSprite16 baseSprite;
  //Leemos la imagen
  baseSprite.load(F("/rainbow.s16"));
  //Creamos el sprite que se moverá (pequeño)
  RoJoSprite16 smallSprite;
  //Le damos tamaño al sprite pequeño
  smallSprite.setSize(20,20);
  //Recortamos parte del sprite base para crear el sprite pequeño
  smallSprite.drawSprite(-30,-10,&baseSprite);
  //Guardamos el sprite pequeño
  smallSprite.save(F("/small.s16"));
  //Eliminamos ambos sprites
  baseSprite.clean();
  smallSprite.clean();

  //Creamos el sprite que usaremos para enviar al dislay
  RoJoSprite16 backSprite;
  //Con el mismo tamaño que la pantalla
  backSprite.setSize(display.xMax,display.yMax);
  //Creamos el sprite que se moverá
  RoJoSprite16 moveSprite;
  //Lo llenamos con la imagen recortada antes
  moveSprite.load("/small.s16");
  //Creamos el sprite que usaremos de imagen de fondo
  RoJoSprite16 imageSprite;
  //Cargamos la imagen que queremos fijar de fondo
  imageSprite.load(F("/mountain.s16"));
  //Definimos el recorrido
  for(int16_t i=-10;i<70;i++)
  {
    //Ponemos la imagen sobre el sprite que enviaremos al display
    backSprite.drawSprite(0,0,&imageSprite);
    //Copiamos el sprite en movimiento sobre la imagen
    backSprite.drawSprite(i,i,&moveSprite);
    //Mostramos el sprite de fondo
    display.drawSprite(0,0,&backSprite);
    //Esta vez no esperamos, porque el desplazamiento no es muy rápido
    //Sí permitimos que se lancen los procesos de fondo para que no se bloquee
    yield();
  }
  //Borramos los sprites utilizados
  backSprite.clean();
  moveSprite.clean();
  imageSprite.clean();
  //Borramos el archivo del sprite pequeño
  SPIFFS.remove(F("/small.s16"));
}

void Test13()
{
  //Mostrar texto

  //Limpiamos pantalla
  display.clear();
  
  //Creamos el sprite que contendrá el texto
  RoJoSprite16 textSprite;
  //Creamos objeto de gestión de fuentes
  RoJoABC16 font;
  //Definimos color de texto
  uint16_t color = display.getColor(0,255,0); //green
  //Creamos el sprite con el texto
  //Si no podemos crear el sprite de texto...hemos terminado
  if(!font.print(F("/5x7d.fon"),F("20171028"),&textSprite,color)) return;
  //Lo mostramos
  display.drawSprite(0,0,&textSprite);

  //Utilizaremos otra fuente más grande
  //Reaprovechamos el objeto de gestión de fuentes
  //Creamos el sprite con el texto
  //Si no podemos cargar la fuente desde el archivo...terminamos
  if(!font.print(F("/10x15d.fon"),F("20171028"),&textSprite,color)) return;
  //Lo mostramos
  display.drawSprite(0,10,&textSprite);
  
  //Cambiamos el color del texto
  color = display.getColor(255,255,0); //yellow
  //Creamos el color de fondo
  uint16_t backColor=display.getColor(0,0,64); //Azul oscuro
  //Creamos otro texto más grande que la pantalla 
  //con color de fondo
  if(!font.print(F("/10x15d.fon"),F("1234567890"),&textSprite,color,backColor)) return;
  //Lo mostramos. No se verá el final
  display.drawSprite(0,26,&textSprite);
  //Cambiamos el color del texto del sprite
  textSprite.replaceColor(color,display.getColor(255,0,0)); //red
  //Lo mostramos de nuevo desplazado a la izquierda, comenzando desde una
  //posición fuera de pantalla
  display.drawSprite(-33,42,&textSprite);

  //Borramos el sprite utilizado
  textSprite.clean();

  delay(2000);
}

void Test14()
{
  //Mostrar texto con fondo transparente

  //Creamos el sprite de base
  RoJoSprite16 baseSprite;
  //Leemos la imagen
  baseSprite.load("/rose.s16");

  //Creamos el sprite de texto
  RoJoSprite16 textSprite;
  //Creamos objeto de gestión de fuentes
  RoJoABC16 font;
  //Definimos color de texto
  uint16_t textColor = display.getColor(0,0,255); //blue
  //Definimos color de fondo que utilizaremos como color invisible (red)
  uint16_t backColor = display.getColor(255,0,255); //red
  //Creamos un texto con color de fondo
  //Si no podemos crear sprite de texto...hemos terminado  
  if(!font.print(F("/10x15d.fon"),F("8765"),&textSprite,textColor,backColor)) return;
  //Ponemos el sprite de texto sobre el sprite base
  baseSprite.drawSprite(30,5,&textSprite);
  //Volvemos a poner el mismo sprite pero hacemos invisible el color de fondo
  baseSprite.drawSprite(30,22,&textSprite,backColor);

  
  //Creamos el mismo texto pero con borde
  //Color de texto = textColor = blue
  //Color de fondo = backColor = red
  //Color de borde = 0 = black
  if(!font.print(F("/10x15d.fon"),F("8765"),&textSprite,textColor,backColor,0)) return;
  //Copiamos el nuevo sprite de texto con borde sobre el sprite base
  baseSprite.drawSprite(29,40,&textSprite,backColor);
  
  //Mostramos el sprite base en pantalla
  display.drawSprite(0,0,&baseSprite);

  //Borramos los sprites utilizados
  baseSprite.clean();
  textSprite.clean();

  delay(2000);
}

void Test15()
{
  //Resize

  //Limpiamos pantalla
  display.clear();

  //Ejemplos de ampliación de imagen
  
  //Creamos el sprite de tamaño normal
  RoJoSprite16 normalSprite;
  //Creamos objeto de gestión de fuentes
  RoJoABC16 font;
  //Creamos un sprite con texto de color blanco con fondo negro
  //Si no podemos crear el sprite de texto...hemos terminado
  if(!font.print(F("/5x7d.fon"),F("654"),&normalSprite,0xFFFF,0x0000)) return;
  //Creamos un nuevo sprite para el redimensionado
  RoJoSprite16 resizeSprite;
  //Redimensionamos el sprite de texto. Lo hacemos 4 veces más grande
  resizeSprite.resize(normalSprite.width()*4,normalSprite.height()*4,&normalSprite);
  //Mostramos el sprite redimensionado en pantalla
  display.drawSprite(0,0,&resizeSprite);
  delay(2000);

  //Redimensionamos el sprite de texto
  //El tamaño final será de la mitad superior de pantalla
  //Quedará un poco deformado (estirado en horizontal)
  resizeSprite.resize(display.xMax,display.yMax/2,&normalSprite);
  //Mostramos el sprite redimensionado en pantalla. En la parte superior
  display.drawSprite(0,0,&resizeSprite);
  //En la parte inferior
  display.drawSprite(0,display.yMax/2,&resizeSprite);
  delay(2000);

  //Redimensionamos el sprite de texto
  //El tamaño final será de la mitad izquierda de pantalla
  //Quedará deformado (estirado en vertical)
  resizeSprite.resize(display.xMax/2,display.yMax,&normalSprite);
  //Mostramos el sprite redimensionado en pantalla. En la parte izquierda
  display.drawSprite(0,0,&resizeSprite);
  //En la parte derecha
  display.drawSprite(display.xMax/2,0,&resizeSprite);
  delay(2000);

  //Redimensionamos el sprite de texto
  //El tamaño final será la pantalla completa
  resizeSprite.resize(display.xMax,display.yMax,&normalSprite);
  //Mostramos el sprite redimensionado en pantalla
  display.drawSprite(0,0,&resizeSprite);
  delay(2000);

  //Ejemplo de reducción de imagen
  
  //Limpiamos pantalla
  display.clear();
  
  //Cargamos una imagen en el sprite normal
  normalSprite.load(F("/rainbow.s16"));
  //Reduciremos la imagen desde un 5% hasta un 100%
  for(uint16_t r=5;r<=100;r++)
  {
    //Calculamos el sprite redimensionado al factor actual
    resizeSprite.resize((display.xMax*r)/100,(display.yMax*r)/100,&normalSprite);
    //Mostramos el sprite redimensionado en pantalla
    display.drawSprite(0,0,&resizeSprite);
    //Esperamos un momento
    delay(10);
  }
  //Borramos los sprites utilizados
  resizeSprite.clean();
  normalSprite.clean();

  delay(1000);
}

void Test16()
{
  //Ejemplo de funciones para dibujar directamente en un sprite

  //Limpiamos pantalla
  display.clear();

  //Creamos el sprite de texto
  RoJoSprite16 textSprite;
  //Creamos objeto de gestión de fuentes
  RoJoABC16 font;
  //Definimos color de texto
  uint16_t color = display.getColor(0,0,255); //blue
  //Creamos un texto sobre fondo negro
  //Si no podemos crear el sprite de texto...hemos terminado
  if(!font.print(F("/10x15d.fon"),F("20171028"),&textSprite,color)) return;
  //Definimos un nuevo color
  color = display.getColor(0,255,0); //green
  //El sprite tiene 16 filas porque el texto tiene esa altura
  //Dibujamos varias líneas inclinadas del color definido
  for(int x=-7;x<textSprite.width()+7;x+=6)
    textSprite.line(x,0,x-5,15,color);
  //Cambiamos de color
  color = display.getColor(255,0,0); //red
  //Dibujamos un rectángulo como borde del sprite
  textSprite.rect(0,0,textSprite.width()-1,textSprite.height()-1,color);
  //Dibujamos un rectángulo negro
  textSprite.clear(5,5,10,10);
  //Nuevo color
  color = display.getColor(255,255,255); //white
  //Dibujamos un rectángulo de color
  textSprite.clear(15,5,20,10,color);
    
  //Mostramos el sprite en pantalla
  display.drawSprite(0,0,&textSprite);

  //Borramos los sprites utilizados
  textSprite.clean();

  delay(2000);
}

void Test17()
{
  //Ejemplo de funciones para dibujar triángulos en sprites

  //Creamos el sprite de fondo
  RoJoSprite16 backSprite;
  //Con el mismo tamaño que la pantalla
  backSprite.setSize(display.xMax,display.yMax);
  //Definimos color
  uint16_t color = display.getColor(0,0,255); //blue
  //Dibujamos un triángulo relleno
  backSprite.triangleFill(40,5,70,30,10,60,color);
  //Cambiamos de color
  color = display.getColor(255,255,0); //yellow
  //Dibujamos un triángulo sin relleno
  backSprite.triangle(30,10,10,25,70,50,color);
  //Mostramos el sprite en pantalla
  display.drawSprite(0,0,&backSprite);
  //Borramos el sprite utilizado
  backSprite.clean();

  delay(2000);
}

void setup()
{
  //Inicializamos el display
  display.begin(pinRES_display,pinDC_display,pinCS_display);
}

void loop()
{
  Test0(); //Test de color
  Test1(); //Función GetColor y SetPixel
  Test2(); //Función de cursor
  Test3(); //Rangos de cursor
  Test4(); //Líneas
  Test5(); //Rectángulos
  Test6(); //Función Copy
  Test7(); //Función Darker
  Test8(); //Función Clear
  Test9(); //Cargar y mostrar un sprite
  Test10(); //Desplazar un sprite escribiendo directamente en pantalla
  Test11(); //Desplazar un sprite escribiendo sobre otro sprite
  Test12(); //Función de guardado de sprites
  Test13(); //Mostrar texto
  Test14(); //Mostrar texto con fondo transparente
  Test15(); //Resize
  Test16(); //Ejemplo de funciones para dibujar directamente en un sprite
  Test17(); //Triángulos
}

