/*
  Autor: Ramón Junquera
  Fecha: 20180225
  Tema: Librería para display OLED SPI 0.95" 96x64 SSD1331
  Objetivo: Demo de librería RoJoSSD1331
  Material: breadboard, cables, cualquier placa, display OLED SSD1331

  Descripción:
  Descripción y demostración de funcionalidades de la librería.

  Pinout SPI:

  Modelo   CLK MISO MOSI CS
  -------  --- ---- ---- --
  ESP32    18   19   23  5
  ESP8266  D5   D6   D7  D8

  Nota:
  La SD debe contener los archivos de la carpeta data para su correcto funcionamiento.  
  
  Resultado:
  Realizamos varios tests cíclicos
*/

#include <Arduino.h>
#include "RoJoSSD1331SD.h" //Librería de gestión del display SSD1331
#include "RoJoSprite16SD.h" //Gestión de sprites color
#include "RoJoABC16SD.h" //Gestión de fuentes

//Definición de pines
#ifdef ESP8266 //Si es un ESP8266...
  const byte pinDC_display=D2;
  const byte pinRES_display=D0;
  const byte pinCS_display=D8; //CS por defecto
  const byte pinCS_SD=D3;
#elif defined(ESP32) //Si es un ESP32...
  const byte pinDC_display=22;
  const byte pinRES_display=21;
  const byte pinCS_display=5; //CS por defecto
  const byte pinCS_SD=4;
#elif defined(ARDUINO_AVR_MEGA2560) //Si es una Mega...
  const byte pinDC_display=5;
  const byte pinRES_display=3;
  const byte pinCS_display=4;
  const byte pinCS_SD=2;  
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
  RoJoSprite16 mySprite(pinCS_SD);
  //Lo llenamos con una imagen
  //Las imágenes de ejemplo se guardan en la carpeta graphics
  //Los sprites color tienen extensión .s16 para mantener la nomenclatura 8.3 que requiere
  //una SD.
  //Pueden ser generados con la aplicación RoJoImageConvert
  //También se han dejado las imágenes .bmp originales
  mySprite.load(F("/RPi.s16"));
  //Lo dibujamos varias veces para llenar el fondo
  for(byte x=0;x<display.xMax;x+=mySprite.width())
    for(byte y=0;y<display.yMax;y+=mySprite.height())
      display.drawSprite(x,y,&mySprite);
  //Podríamos haber simplificado los bucles sabiendo que el sprite es de 32x32 pixels
  //Borramos el sprite utilizado
  mySprite.clean();

  delay(1000);
}

void Test10()
{
  //Desplazar un sprite escribiendo directamente en pantalla
  
  //Creamos el sprite
  RoJoSprite16 mySprite(pinCS_SD);
  //Lo llenamos con una imagen
  mySprite.load(F("/round.s16"));
  //Definimos el recorrido
  for(int i=-20;i<80;i++)
  {
    //Limpiamos pantalla
    display.clear();
    //Dibujamos el sprite en la nueva posición
    display.drawSprite(i,i,&mySprite);
    //Esperamos un momento
    delay(10);
  }
  //Borramos el sprite utilizado
  mySprite.clean();
}

void Test11()
{
  //Desplazar un sprite escribiendo sobre otro sprite

  //Puesto que Mega no tiene memoria suficiente como para mantener un sprite del
  //tamaño de la pantalla, tenemos que encontrar otra solución.
  //Como fondo de pantalla pondremos un mosaico de una imagen
  //Como sabemos el tamaño de la plantalla y de la imagen de mosaico, podemos
  //saber qué hay en cada punto de la pantalla.
  //Cargaremos en un sprite la imagen que queremos mover
  //Crearemos un sprite con un pixel más en cada dimensión que utilizaremos para
  //dibujar en pantalla la imagen en movimiento y la fila y columna de más nos
  //servirá para borrar la imagen anterior

  //Creamos el sprite de fondo
  RoJoSprite16 backSprite(pinCS_SD);
  //Leemos la imagen desde un archivo
  backSprite.load(F("/RPi.s16"));
  //Dibujamos la imagen como mosaico en el fondo de la pantalla
  for(byte x=0;x<3;x++)
    for(byte y=0;y<2;y++)
      display.drawSprite(x*32,y*32,&backSprite);

  //Creamos el sprite que se moverá
  RoJoSprite16 moveSprite(pinCS_SD);
  //Lo llenamos con una imagen
  moveSprite.load(F("/round.s16")); 

  //Creamos el sprite que usaremos como máscara
  RoJoSprite16 maskSprite(pinCS_SD);
  //Tendrá como tamaño un pixel más en cada dimensión
  maskSprite.setSize(17,17);
  
  //Definimos el recorrido
  for(int i=-20;i<80;i++)
  {
    //Simulamos el fondo sobre el sprite de máscara
    for(int x=0;x<2;x++)
      for(int y=0;y<2;y++)
        maskSprite.drawSprite(x*32-i%32,y*32-i%32,&backSprite);
    //Copiamos el sprite de máscara en movimiento sobre el de fondo
    maskSprite.drawSprite(1,1,&moveSprite);
    //Dibujamos el sprite de máscara en la posición que le corresponde
    //display.drawSprite(i,i,&maskSprite);
    display.drawSprite(i,i,&maskSprite);
    
    //Esperamos un momento
    delay(10);
    //Esta vez no esperamos, porque el desplazamiento no es muy rápido
    //Permitimos que se lancen los procesos de fondo para que no se bloquee
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
  RoJoSprite16 baseSprite(pinCS_SD);
  //Leemos la imagen
  baseSprite.load(F("/RPi.s16"));
  //Creamos el sprite que se moverá (pequeño)
  RoJoSprite16 smallSprite(pinCS_SD);
  //Le damos tamaño al sprite pequeño
  smallSprite.setSize(16,32);
  //Recortamos parte del sprite base para crear el sprite pequeño
  smallSprite.drawSprite(0,0,&baseSprite);
  //Guardamos el sprite pequeño
  smallSprite.save(F("/small.s16"));
  //Eliminamos ambos sprites
  baseSprite.clean();
  smallSprite.clean();

  //Creamos el sprite que usaremos para enviar al dislay
  RoJoSprite16 backSprite(pinCS_SD);
  //Lo llenamos con la imagen recortada antes
  backSprite.load("/small.s16");
  //Dibujamos el sprite varias veces llenando la pantalla
  for(byte x=0;x<6;x++)
    for(byte y=0;y<2;y++)
      display.drawSprite(x*16,y*32,&backSprite);
  //Borramos el sprite utilizado
  backSprite.clean();
  //Borramos el archivo del sprite pequeño
  SPIFFS.remove(F("/small.s16"));

  delay(1000);
}

void Test13()
{
  //Mostrar texto

  //Limpiamos pantalla
  display.clear();
  
  //Creamos el sprite que contendrá el texto
  RoJoSprite16 textSprite(pinCS_SD);
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

  //Creamos el sprite con la imagen del mosaico de fondo
  RoJoSprite16 backSprite(pinCS_SD);
  //Leemos la imagen
  backSprite.load("/RPi.s16");
  //Dibujamos la imagen como mosaico en el fondo de la pantalla
  for(byte x=0;x<3;x++)
    for(byte y=0;y<2;y++)
      display.drawSprite(x*32,y*32,&backSprite);

  //Creamos el sprite del texto
  RoJoSprite16 textSprite(pinCS_SD);
  //Creamos objeto de gestión de fuentes
  RoJoABC16 font;
  //Definimos color de texto
  uint16_t textColor = display.getColor(0,0,255); //blue
  //Definimos color de fondo que utilizaremos como color invisible (red)
  uint16_t backColor = display.getColor(255,0,255); //red
  //Creamos un texto con color de fondo
  //Si no podemos crear sprite de texto...hemos terminado  
  if(!font.print(F("/10x15d.fon"),F("8765"),&textSprite,textColor,backColor)) return;
  //Dibujamos el sprite de texto
  display.drawSprite(30,5,&textSprite);

  //Escribiremos el mismo texto un poco más abajo, pero queremos que el fondo sea
  //transparente para que se vea el mosaico de pantalla
  //Crearemos un sprite del mismo tamaño que el de texto.
  //En él dibujaremos el mosaico de pantalla que corresponda a la posición y después
  //dibujaremos el sprite de texto haciendo el color de fondo invisible.
  //Finalmente dibujaremos el sprite generado en la posición decidida

  //Coordenadas del sprite
  byte xBase=30;
  byte yBase=22;
  //Creamos el sprite de máscara
  RoJoSprite16 maskSprite(pinCS_SD);
  //Con el mismo tamaño que el sprite de texto
  maskSprite.setSize(textSprite.width(),textSprite.height());
  //Dibujamos el mosaico que tenemos en pantalla
  for(int x=0;x<3;x++)
    for(int y=0;y<2;y++)
      maskSprite.drawSprite(x*32-xBase%32,y*32-yBase%32,&backSprite);
  //Dibujamos encima el sprite de texto pero hacemos invisible el color de fondo
  maskSprite.drawSprite(0,0,&textSprite,backColor);
  //Dibujamos el sprite de texto en pantalla
  display.drawSprite(xBase,yBase,&maskSprite);

  //Liberamos la memoria de los sprites utilizados
  maskSprite.clean();
  textSprite.clean();

  //Creamos el mismo texto pero con borde
  //Color de texto = textColor = blue
  //Color de fondo = backColor = red
  //Color de borde = 0 = black
  //Nuevas coordenadas del sprite
  xBase=29;
  yBase=40;
  if(!font.print(F("/10x15d.fon"),F("8765"),&textSprite,textColor,backColor,0)) return;
  //Redimensionaremos el sprite de máscara al tamaño del nuevo sprite de texto
  maskSprite.setSize(textSprite.width(),textSprite.height());
  //Dibujamos el mosaico que tenemos en pantalla
  for(int x=0;x<3;x++)
    for(int y=0;y<2;y++)
      maskSprite.drawSprite(x*32-xBase%32,y*32-yBase%32,&backSprite);
  //Dibujamos encima el sprite de texto pero hacemos invisible el color de fondo
  maskSprite.drawSprite(0,0,&textSprite,backColor);
  //Dibujamos el sprite de texto en pantalla
  display.drawSprite(xBase,yBase,&maskSprite);

  //Borramos los sprites utilizados
  backSprite.clean();
  textSprite.clean();
  maskSprite.clean();

  delay(2000);
}

void Test15()
{
  //Resize

  //Limpiamos pantalla
  display.clear();
  
  //Creamos el sprite de tamaño normal
  RoJoSprite16 normalSprite(pinCS_SD);
  //Cargamos su imagen
  normalSprite.load(F("/round.s16")); 
  //Creamos un nuevo sprite para el redimensionado
  RoJoSprite16 resizeSprite(pinCS_SD);
  //Ampliaremos su tamaño 30 pixels en ambas dimensiones
  for(byte i=0;i<30;i++)
  {
    //Dimensionamos el sprite a dibujar
    resizeSprite.setSize(16+i,16+i);
    //Redimensionamos el sprite original
    resizeSprite.resize(16+i,16+i,&normalSprite);
    //Mostramos el sprite redimensionado en pantalla
    display.drawSprite(0,0,&resizeSprite);
    delay(10);
  }
  //Borramos los sprites utilizados
  resizeSprite.clean();
  normalSprite.clean();

  delay(1000);
}

void Test16()
{
  //Texto con fuentes de caracteres

  //Limpiamos pantalla
  display.clear();

  //Creamos el sprite de texto
  RoJoSprite16 textSprite(pinCS_SD);
  //Creamos objeto de gestión de fuentes
  RoJoABC16 font;
  //Definimos color del borde
  uint16_t borderColor = display.getColor(0,0,255); //blue
  //El valor cambiará de 22.3 a 18.6
  for(byte t=223;t>185;t--)
  {
    //Si no podemos crear el sprite de texto...hemos terminado
    //El color del texto varía con el valor mostrado
    uint16_t textColor = display.getColor(t,(t-185)*6,(t-185)*6);
    //El fondo será negro
    if(!font.print(F("/7x11.fon"),"V=" + String(t/10) + "." + String(t%10) + " Km/h",&textSprite,textColor,0,borderColor)) return;
    //Mostramos el sprite en pantalla
    display.drawSprite(0,0,&textSprite);
    //Esperamos un momento
    yield();
    delay(50);
  }
  //Borramos los sprites utilizados
  textSprite.clean();

  delay(1000);
}

void setup()
{
  Serial.begin(115200);
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
  Test16(); //Texto con fuentes de caracteres
}

