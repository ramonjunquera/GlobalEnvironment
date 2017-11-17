/*
  Autor: Ramón Junquera
  
  Tema: Librería para display OLED SPI 0.95" 96x64 SSD1331
  
  Objetivo: Demo de librería RoJoSSD1331
  
  Material adicional: breadboard, cables, placa NodeMCU v1.0/ESP-12E, display OLED SSD1331

  Descripción:
  Descripción y demostración de funcionalidades de la librería.

  Pinout SPI:

  Modelo        CLK MISO MOSI CS
  ------------  --- ---- ---- --
  NodeMCU v1.0  D5   D6   D7  D8
  
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

//Definimos las constantes de pines
const byte pinDC=D2;
const byte pinRES=D0;
const byte pinCS=D8;

//Creamos objeto de gestión
RoJoSSD1331 display;

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
      display.setPixel(x*5,y*5,blueColor);
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
      //Dibujamos el pixel
      display.setPixel(x*5,y*5,greenColor);
    }
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
      display.setPixel(display.getColor((y+1)*8-1,0,0));
    }
  }
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
  //Sólo se puede cargar el formato .spr16a que puede ser generado con
  //la aplicación RoJoImageConvert v1.03 o superior
  //También se han dejado las imágenes .bmp originales
  mySprite.load(F("/rose.spr16a"));
  //Lo mostramos
  display.drawSprite(0,0,&mySprite);
  //Borramos el sprite utilizado
  mySprite.clean();
}

void Test10()
{
  //Desplazar un sprite escribiendo directamente en pantalla
  
  //Creamos el sprite
  RoJoSprite16 mySprite;
  //Lo llenamos con una imagen
  mySprite.load(F("/rainbow.spr16a"));
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
  moveSprite.load(F("/rainbow.spr16a")); 
  //Definimos el recorrido
  for(int i=-70;i<70;i++)
  {
    //Borramos la imagen del sprite de fondo
    backSprite.clear();
    //Copiamos el sprite en movimiento sobre el de fondo
    backSprite.copy(i,i,&moveSprite);
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
  baseSprite.load(F("/rainbow.spr16a"));
  //Creamos el sprite que se moverá (pequeño)
  RoJoSprite16 smallSprite;
  //Le damos tamaño al sprite pequeño
  smallSprite.setSize(10,10);
  //Recortamos parte del sprite base para crear el sprite pequeño
  smallSprite.copy(-30,-10,&baseSprite);
  //Guardamos el sprite pequeño
  smallSprite.save(F("/small.spr16a"));
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
  moveSprite.load("/small.spr16a");
  //Creamos el sprite que usaremos de imagen de fondo
  RoJoSprite16 imageSprite;
  //Cargamos la imagen que queremos fijar de fondo
  imageSprite.load(F("/mountain.spr16a"));
  //Definimos el recorrido
  for(int16_t i=-10;i<70;i++)
  {
    //Ponemos la imagen sobre el sprite que enviaremos al display
    backSprite.copy(0,0,&imageSprite);
    //Copiamos el sprite en movimiento sobre la imagen
    backSprite.copy(i,i,&moveSprite);
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
  SPIFFS.remove(F("/small.spr16a"));
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
  //Si no podemos cargar la fuente desde el archivo...hemos terminado
  if(!font.load(F("/RoJoABC5x7digits.fon"))) return;
  //Definimos color de texto
  uint16_t color = display.getColor(0,255,0); //green
  //Creamos el sprite con el texto
  font.print(F("20170823"),&textSprite,color);
  //Lo mostramos
  display.drawSprite(0,0,&textSprite);

  //Utilizaremos otra fuente más grande
  //Reaprovechamos el objeto de gestión de fuentes
  //Si no podemos cargar la fuente desde el archivo...terminamos
  if(!font.load(F("/RoJoABC10x15digits.fon"))) return;  
  //Creamos el sprite con el texto
  font.print(F("20170823"),&textSprite,color);
  //Lo mostramos
  display.drawSprite(0,10,&textSprite);
  
  //Cambiamos el color del texto
  color = display.getColor(0,255,255); //yellow
  //Creamos el color de fondo
  uint16_t backColor=display.getColor(0,0,64); //Azul oscuro
  //Creamos otro texto más grande que la pantalla 
  //con color de fondo
  font.print(F("1234567890"),&textSprite,color,backColor);
  //Lo mostramos. No se verá el final
  display.drawSprite(0,26,&textSprite);
  //Cambiamos el color del texto del sprite
  textSprite.replaceColor(color,display.getColor(255,0,0)); //red
  //Lo mostramos de nuevo desplazado a la izquierda, comenzando desde una
  //posición fuera de pantalla
  display.drawSprite(-33,42,&textSprite);

  //Terminamos de utilizar el objeto de gestión de fuentes
  font.close();
  //Borramos el sprite utilizado
  textSprite.clean();
}

void Test14()
{
  //Mostrar texto con fondo transparente

  //Creamos el sprite de base
  RoJoSprite16 baseSprite;
  //Leemos la imagen
  baseSprite.load("/rose.spr16a");

  //Creamos el sprite de texto
  RoJoSprite16 textSprite;
  //Creamos objeto de gestión de fuentes
  RoJoABC16 font;
  //Si no podemos cargar la fuente desde el archivo...hemos terminado
  if(!font.load(F("/RoJoABC10x15digits.fon"))) return;  
  //Definimos color de texto
  uint16_t textColor = display.getColor(0,0,255); //blue
  //Definimos color de fondo que utilizaremos como color invisible (red)
  uint16_t backColor = display.getColor(255,0,255); //red
  //Creamos un texto con color de fondo
  font.print("8765",&textSprite,textColor,backColor);
  //Ponemos el sprite de texto sobre el sprite base
  baseSprite.copy(30,5,&textSprite);
  //Volvemos a poner el mismo sprite pero hacemos invisible el color de fondo
  baseSprite.copy(30,22,&textSprite,backColor);

  
  //Creamos el mismo texto pero con borde
  //Color de texto = textColor = blue
  //Color de fondo = backCOlor = red
  //Color de borde = 0 = black
  font.print(F("8765"),&textSprite,textColor,backColor,0);
  //Copiamos el nuevo sprite de texto con borde sobre el sprite base
  baseSprite.copy(29,40,&textSprite,backColor);
  
  //Mostramos el sprite base en pantalla
  display.drawSprite(0,0,&baseSprite);

  //Terminamos de utilizar el objeto de gestión de fuentes
  font.close();
  //Borramos los sprites utilizados
  baseSprite.clean();
  textSprite.clean();
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
  //Si no podemos cargar la fuente desde el archivo...hemos terminado
  if(!font.load(F("/RoJoABC5x7digits.fon"))) return;  
  //Creamos un sprite con texto de color blanco con fondo negro
  font.print("654",&normalSprite,0xFFFF,0x0000);
  //Terminamos de utilizar el objeto de gestión de fuentes
  font.close();
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
  normalSprite.load(F("/rainbow.spr16a"));
  //Reduciremos la imagen desde un 5% hasta un 100%
  for(uint16_t r=5;r<100;r++)
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
  //Si no podemos cargar la fuente desde el archivo...hemos terminado
  if(!font.load(F("/RoJoABC10x15digits.fon"))) return;  
  //Definimos color de texto
  uint16_t color = display.getColor(0,0,255); //blue
  //Creamos un texto sobre fondo negro
  font.print("20171016",&textSprite,color);
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

  //Terminamos de utilizar el objeto de gestión de fuentes
  font.close();
  //Borramos los sprites utilizados
  textSprite.clean();
}

void setup()
{
  //Inicializamos el display
  display.begin(pinDC,pinRES,pinCS);
}

void loop()
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
  Test11();
  Test12();
  Test13();
  delay(2000);
  Test14();
  delay(2000);
  Test15();
  delay(1000);
  Test16();
  delay(3000);
}
