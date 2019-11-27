/*
  Autor: Ramón Junquera
  Fecha: 20190903
  Tema: Librería para display OLED I2C 0.96" 128x64 SSD1306
  Objetivo: Demo de librería RoJoSSD1306

  Descripción y demostración de funcionalidades de la librería.

  I2C pinout:

  device      SCL SDA RST
  ----------- --- --- ---
  Mega        21  20  ---
  ESP8266     D1  D2  ---
  ESP32       22  21  ---
  ESP32+OLED  15   4  16
  RPi 0       28  27  --- -> ID_SC & ID_SD    
  RPi 1        5   3  --- -> GPIO3 & GPIO2

  Notas:
  - El sistema de archivos seleccionado (SPIFFS o SD) debe contener los archivos de la carpeta
    data para su correcto funcionamiento.
  - Este modelo de display no permite dibujar directamente en pantalla, pero si se puede dibujar
    un sprite sobre ella. Para la mayoría de los ejemplos utilizarmos un sprite con las mismas
    dimensiones del display (128x64) en el que dibujaremos para después volcarlo en el display.
    Este sprite lo llamaremos 'v' (de memoria de vídeo) y lo definiremos globalmente.
  - Las placas Arduino UNO y Nano no tienen suficiente memoria como para almacenar el programa
  
  Resultado:
    Realizamos varios tests cíclicos
*/

#include <Arduino.h>
#include <RoJoSSD1306.h> //Librería de gestión del display SSD1306

//Creamos objeto de gestión
RoJoSSD1306 display;
//Creamos un sprite monocromo que utilizaremos como memoria de vídeo
RoJoSprite v(1);

//Definición de pines
#ifdef ESP32OLED16MB //Si se trata del ESP32+OLED 16Mb
  byte pinSDA=4;
  byte pinSCL=15;
  byte pinRES=16;
#elif defined(ESP32) //Si es ESP32
  byte pinSDA=255;
  byte pinSCL=255;
  byte pinRES=16;  
#elif defined(ESP8266) //Si es ESP8266
  byte pinSDA=255;
  byte pinSCL=255;
  byte pinRES=D0;
#elif defined(ARDUINO_AVR_MEGA2560) //Si es Arduino Mega
  byte pinSDA=255;
  byte pinSCL=255;
  byte pinRES=3;
#else //Para cualquier otro dispositivo utilizaremos los pines estándar
  byte pinSDA=255;
  byte pinSCL=255;
  byte pinRES=255; //Sin pin de reset
#endif

//Función drawPixel con color 1 = dibujar
void Test1() {
  //Limpiamos la memoria de vídeo
  v.clear();
  //Dibujamos una matriz de puntos
  for(uint16_t x=0;x<v.xMax();x+=5)
    for(uint16_t y=0;y<v.yMax();y+=5)
      v.drawPixel(x,y,1);
  //Mostramos el resultado
  display.drawSprite(&v);

  delay(1000);
}

//Función drawPixel con color 2 = invertir
void Test2() {
  //Dibujamos pixels en la mitad izquerda del display con el color 2 (invertir color)
  for(uint16_t y=0;y<v.yMax();y++)
    for(uint16_t x=0;x<v.xMax()/2;x++)
      v.drawPixel(x,y,2);
  //Mostramos el resultado
  display.drawSprite(&v);

  delay(1000);
}

//Función drawPixel con color 0 = borrar
void Test3() {
  //Dibujamos un rectángulo relleno en el interior que borra
  for(uint16_t y=10;y<40;y++)
    for(uint16_t x=10;x<40;x++)
      v.drawPixel(x,y,0);
  //Mostramos el resultado
  display.drawSprite(&v);

  delay(1000);
}

//Función getPixel
void Test4() {
  //Comprobamos los pixels de un cuadrado y dibujamos el color opuesto
  for(uint16_t y=30;y<50;y++)
    for(uint16_t x=30;x<50;x++)
      v.drawPixel(x,y,!v.getPixel(x,y));
  //Mostramos el resultado
  display.drawSprite(&v);

  delay(1000);
}

//Rectángulos
void Test5() {
  //Limpiamos pantalla
  v.clear();
  //Dibujamos un rectángulo relleno sin borde (un bloque)
  v.block(0,0,20,20,1); //1=sólido
  //Dibujamos un rectángulo con borde inverso (sin relleno)
  v.rect(10,10,30,30,2);
  //Mostramos el resultado
  display.drawSprite(&v);

  delay(1000);
}

//Líneas
void Test6() {
  //Limpiamos pantalla
  v.clear();
  for(byte y=0;y<v.yMax();y+=10) v.line(0,0,v.xMax()-1,y,1);
  //Mostramos el resultado
  display.drawSprite(&v);

  delay(1000);
}

//Sprites definidos en programa
//  RoJoSprite::drawPage
void Test7() {
  //Limpiamos pantalla
  v.clear();
  //Creamos el sprite monocromo
  RoJoSprite mySprite(1);
  //Lo dimensionamos. Anchura=7. Altura=8 (una página)
  mySprite.setSize(7,8);
  //Lo dibujamos
  //bool RoJoSprite::drawPage(uint16_t x,uint16_t page,byte mask,byte color) {
  mySprite.drawPage(0,0,0b00111110,4); //4=escribir el valor tal cual
  mySprite.drawPage(1,0,0b01000001,4);
  mySprite.drawPage(2,0,0b01010101,4);
  mySprite.drawPage(3,0,0b01010001,4);
  mySprite.drawPage(4,0,0b01010101,4);
  mySprite.drawPage(5,0,0b01000001,4);
  mySprite.drawPage(6,0,0b00111110,4);

  //Lo dibujamos varias veces
  for(byte y=0;y<v.yMax();y+=8)
    for(byte x=0;x<v.xMax();x+=8)
      v.drawSprite(&mySprite,x,y);
  //Borramos el sprite utilizado
  mySprite.end();
  //Mostramos el resultado
  display.drawSprite(&v);

  delay(1000);
}

//Leer y dibujar sprite desde un archivo
//  RoJoSprite::loadSprite,drawSprite
//  RoJoSSD1306::reverse
void Test8() {
  Serial.println("Test8. start");
  display.clear(); //Limpiamos display mientras creamos el sprite
  //En el sprite que vamos a leer el dibujo lo hacen los pixels activos
  //En dibujo original estaba hecho en negro sobre fondo blanco
  //Es justo al revés de lo que muestra el display por defecto
  //Lo normal es que una pantalla vacía sera de color negro
  //Por eso activaremos el modo inverso
  display.reverse(true);

  //Limpiamos sprite de memoria de vídeo
  v.clear();

  //Método 1. Cargamos leemos de un archivo a un nuevo sprite
  //Creamos el sprite monocromo
  RoJoSprite mySprite(1);
  //Lo leemos desde el archivo
  byte errorCode=mySprite.loadSprite("/ball.spr");
  Serial.println("Test8. loadSprite ball.spr = "+String(errorCode));
  //Dibujamos el sprite en memoria de vídeo (en 0,0)
  v.drawSprite(&mySprite);
  //Borramos el sprite utilizado
  mySprite.end();

  //Método 2. Dibujamos sprite directamente sobre otro
  //Leemos archivo y lo dibujamos en 64,0
  errorCode=v.drawSprite("/ball.spr",64,0);
  Serial.println("Test8. drawSprite ball.spr = "+String(errorCode));

  //Mostramos el resultado
  display.drawSprite(&v);

  delay(1000);
  Serial.println("Test8. end");
}

//Mover sprite por pantalla
void Test9() {
  Serial.println("Test9. start");
  //Seguiremos con el modo inverso activado (pixels en negro)
  display.reverse(true);
  //Creamos un nuevo sprite monocromo
  RoJoSprite ball(1);
  //Leemos su contenido desde un archivo
  byte errorCode=ball.loadSprite("/ball.spr"); //Su tamaño es x=30,y=30,pages=4
  Serial.println("Test9. loadSprite ball.spr = "+String(errorCode));
  //Coordenadas
  byte x=0,y=0;
  //Deltas
  int8_t dx=1,dy=1;
  //Calculamos el tiempo máximo de duración de esta demo
  uint32_t tMax=millis()+4000; //4 segundos
  //Bucle principal
  while(millis()<tMax) {
    //Borramos la memoria de vídeo
    v.clear();
    //Dibujamos el sprite en la memoria de vídeo
    v.drawSprite(&ball,x,y);
    //Refrescamos pantalla
    display.drawSprite(&v);
    //Calculamos las nuevas coordenadas
    x+=dx, y+=dy;
    //Si hemos tocado con un borde, cambiamos la dirección
    if(x==0 || x==97) dx=-dx; //97=128-30-1
    if(y==0 || y==33) dy=-dy; //33=64-30-1
    #ifdef ESP8266
      yield(); //Refrescamos WatchDog
    #endif
  }
  //Borramos el sprite utilizado
  ball.end();
  Serial.println("Test9. end");
}

//Sincronización de sprites
void Test10() {
  Serial.println("Test10. start");
  //La función de sincronización de sprites permite usar la técnica del dóble buffer
  //de memoria de vídeo. Ambas del mismo tipo y tamaño, el mismo que el display.
  //Una de las memoria será la de trabajo que podremos leer o escribir.
  //La otra será la memoria de vídeo del display.
  //Cuando sincronizacmos ambos sprites, se comparan y se aplican los cambios encontrados
  //a la memoria de vídeo del display al mismo tiempo que se envían al display.
  //Con la ventaja de que sólo se enviarán las zonas que han cambiado, acelerando
  //la velocidad de escritura
  //En este ejempl tomaremos v como la memoria de vídeo de trabajo, como hemos estado
  //haciendo hasta ahora, y crearemos uno nuevo llamado videoMem para la memoria de
  //vídeo del display.

  //Seguiremos con el modo inverso activado (pixels en negro)
  display.reverse(true);
  //Limpiamos la memoria de vídeo de trabajo
  v.clear();
  //Creamos el sprite de memoria de vídeos del display
  RoJoSprite videoMem(1);
  //Le damos el mismo tamaño que el display
  //Ya se crea limpio
  videoMem.setSize(display.xMax(),display.yMax());
  //Borramos el display
  display.clear();
  //Tenemos las 3 memorias vacías (sincronizadas)

  //Repetimos el ejemplo anterior de mover un sprite por pantalla
  //Creamos un nuevo sprite monocromo
  RoJoSprite ball(1);
  //Leemos su contenido desde un archivo
  byte errorCode=ball.loadSprite("/ball.spr"); //Su tamaño es x=30,y=30,pages=4
  Serial.println("Test10. loadSprite ball.spr = "+String(errorCode));
  //Coordenadas
  byte x=0,y=0;
  //Deltas
  int8_t dx=1,dy=1;
  //Calculamos el tiempo máximo de duración de esta demo
  uint32_t tMax=millis()+4000; //4 segundos
  //Bucle principal
  while(millis()<tMax) {
    //Borramos la memoria de vídeo
    v.clear();
    //Dibujamos el sprite en la memoria de vídeo
    v.drawSprite(&ball,x,y);
    //Refrescamos pantalla
    display.drawSpriteSync(&v,&videoMem);
    //Calculamos las nuevas coordenadas
    x+=dx, y+=dy;
    //Si hemos tocado con un borde, cambiamos la dirección
    if(x==0 || x==97) dx=-dx; //97=128-30-1
    if(y==0 || y==33) dy=-dy; //33=64-30-1
  }
  //Borramos el sprite utilizado
  ball.end();
  videoMem.end();
  Serial.println("Test10. end");
}

//Mover sprite por pantalla manteniendo fija una imagen de fondo
void Test11() {
  Serial.println("Test11. start");
  //Nota: los pixel apagados del sprite en movimiento aparecen transparentes

  //Seguiremos con el modo inverso activado (pixels en negro)
  display.reverse(true);
  //Creamos un nuevo sprite monocromo para la imagen fija
  RoJoSprite backSprite(1);
  //Leemos su contenido desde un archivo
  byte errorCode=backSprite.loadSprite("/mickey.spr");
  Serial.println("Test11. loadSprite mickey.spr = "+String(errorCode));
  //Creamos un nuevo sprite monocromo para la imagen en movimiento
  RoJoSprite ball(1);
  //Leemos su contenido desde un archivo
  errorCode=ball.loadSprite("/ball.spr"); //Su tamaño es x=30,y=30,pages=4
  Serial.println("Test11. loadSprite ball.spr = "+String(errorCode));
  //Coordenadas
  byte x=0,y=0;
  //Deltas
  int8_t dx=1,dy=1;
  //Calculamos el tiempo máximo de duración de esta demo
  uint32_t tMax=millis()+4000; //4 segundos
  //Bucle principal
  while(millis()<tMax) {
    //Dibujamos la imagen fija en la memoria de vídeo
    v.drawSprite(&backSprite);
    //Dibujamos el sprite en movimiento tomando los pixels apagados como transparentes
    v.drawSprite(&ball,x,y,1);
    //Refrescamos pantalla
    display.drawSprite(&v);
    //Calculamos las nuevas coordenadas
    x+=dx, y+=dy;
    //Si hemos tocado con un borde, cambiamos la dirección
    if(x==0 || x==97) dx=-dx; //97=128-30-1
    if(y==0 || y==33) dy=-dy; //33=64-30-1
  }
  //Borramos los sprites utilizados
  ball.end();
  backSprite.end();
  Serial.println("Test11. end");
}

//Mover sprite por pantalla manteniendo una imagen de fondo y que no sea transparente
void Test12() {
  Serial.println("Test12. start");
  //Para conseguir esto, necesitamos una imagen que nos haga de máscara
  //Esta máscara permitirá borrar antes de dibujar el sprite definitivo

  //Seguiremos con el modo inverso activado (pixels en negro)
  display.reverse(true);
  //Creamos un nuevo sprite monocromo para la imagen fija
  RoJoSprite backSprite(1);
  //Leemos su contenido desde un archivo
  byte errorCode=backSprite.loadBMP("/mickey.bmp");
  Serial.println("Test12. loadBMP mickey.bmp = "+String(errorCode));
  //Creamos un nuevo sprite monocromo para la imagen en movimiento
  RoJoSprite ball(1);
  //Leemos su contenido desde un archivo
  errorCode=ball.loadSprite("/ball.spr"); //Su tamaño es x=30,y=30,pages=4
  Serial.println("Test12. loadSprite ball.spr = "+String(errorCode));
  //Creamos un nuevo sprite monocromo para la máscara de la imagen en movimiento
  RoJoSprite ballMask(1);
  //Leemos su contenido desde un archivo
  errorCode=ballMask.loadSprite("/ballmask.spr"); //Su tamaño es x=30,y=30,pages=4
  Serial.println("Test12. loadSprite ballmask.spr = "+String(errorCode));
  //Coordenadas
  byte x=0,y=0;
  //Deltas
  int8_t dx=1,dy=1;
  //Calculamos el tiempo máximo de duración de esta demo
  uint32_t tMax=millis()+4000; //4 segundos
  //Bucle principal
  while(millis()<tMax)
  {
    //Dibujamos el sprite estático
    v.drawSprite(&backSprite);
    //Dibujamos la máscara del sprite en movimiento borrando
    v.drawSprite(&ballMask,x,y,0);
    //Dibujamos el sprite en movimiento
    v.drawSprite(&ball,x,y,1);
    //Refrescamos pantalla
    display.drawSprite(&v);
    //Calculamos las nuevas coordenadas
    x+=dx, y+=dy;
    //Si hemos tocado con un borde, cambiamos la dirección
    if(x==0 || x==97) dx=-dx; //97=128-30-1
    if(y==0 || y==33) dy=-dy; //33=64-30-1
  }
  //Borramos los sprites utilizados
  ballMask.end();
  ball.end();
  backSprite.end();
  Serial.println("Test12. end");
}

//Obtener sprite de parte de otro
void Test13() {
  Serial.println("Test13. start");
  //Seguiremos con el modo inverso activado (pixels en negro)
  display.reverse(true);
  //Dibujamos un sprite desde un archivo
  byte errorCode=v.drawSprite("/mickey.spr");
  Serial.println("Test13. loadSprite mickey.spr = "+String(errorCode));
  //Creamos un nuevo sprite monocromo
  RoJoSprite mySprite(1);
  //Copiamos parte de la memoria de vídeo en el nuevo sprite
  mySprite.copy(&v,60,0,92,40);
  //Dibujamos el sprite varias veces
  for(byte x=0;x<v.xMax();x+=32)
    for(byte y=0;y<v.yMax();y+=40)
      v.drawSprite(&mySprite,x,y);
  //Refrescamos pantalla
  display.drawSprite(&v);
  //Borramos el sprite utilizado
  mySprite.end();

  delay(1000);
  Serial.println("Test13. end");
}

//Leer un bmp
//Guardar un sprite
void Test14() {
  Serial.println("Test14. start");
  //Seguiremos con el modo inverso activado (pixels en negro)
  display.reverse(true);
  //Dibujamos un bmp desde un archivo en la memoria de vídeo en 0,0
  byte errorCode=v.drawBMP("/mickey.bmp");
  Serial.println("Test14. drawBMP mickey.bmp = "+String(errorCode));
  //Creamos un nuevo sprite monocromo
  RoJoSprite mySprite(1);
  //Copiamos parte de la memoria de vídeo en el nuevo sprite
  mySprite.copy(&v,50,16,65,40);
  //Guardamos el sprite en un archivo
  errorCode=mySprite.save("/mick2.spr");
  Serial.println("Test14. save mick2.spr = "+String(errorCode));
  //Borramos el sprite utilizado
  mySprite.end();
  //Creamos un nuevo sprite monocromo
  RoJoSprite mySprite2(1);
  //Leemos su contenido desde el archivo
  errorCode=mySprite2.loadSprite("/mick2.spr");
  Serial.println("Test14. loadSprite mick2.spr = "+String(errorCode));
  //Dibujamos el sprite varias veces en la memoria de vídeo
  for(byte y=0;y<v.yMax();y+=24)
    for(byte x=0;x<v.xMax();x+=15)
      v.drawSprite(&mySprite2,x,y);
  //Refrescamos pantalla
  display.drawSprite(&v);
  //Borramos el sprite utilizado
  mySprite2.end();
  //Borramos el archivo
  #ifdef ROJO_PIN_CS_SD //Si se utiliza SD...
    SD.remove("/mick2.spr");
  #else //Si utilizamos SPIFFS...
    SPIFFS.remove("/mick2.spr");
  #endif

  delay(1000);
  Serial.println("Test14. end");
}

//Resize
void Test15() {
  Serial.println("Test15. start");
  //Seguiremos con el modo inverso activado (pixels en negro)
  display.reverse(true);
  //Creamos un nuevo sprite monocromo
  RoJoSprite mySprite(1);
  //Leemos su contenido desde un archivo
  byte errorCode=mySprite.loadSprite("/mickey.spr");
  Serial.println("Test15. loadSprite mickey.spr = "+String(errorCode));
  //Creamos el sprite monocromo utilizado para el redimensionado
  RoJoSprite resizeSprite(1);
  //Borramos el display
  display.clear();
  //Ampliaremos la imagen desde un 5% hasta un 100%
  for(uint16_t r=5;r<=100;r++) {
    //Calculamos el sprite redimensionado al factor actual
    resizeSprite.resize(&mySprite,(mySprite.xMax()*r)/100,(mySprite.yMax()*r)/100);
    //Dibujamos el sprite redimensionado en el display
    display.drawSprite(&resizeSprite);
    //Esperamos un momento
    delay(10);
  }
  //Borramos los sprites utilizados
  resizeSprite.end();
  mySprite.end();

  delay(1000);
  Serial.println("Test15. end");
}

//Mostrar texto
void Test16() {
  Serial.println("Test16. start");
  //Limpiamos la memoria de vídeo
  v.clear();
  //Desactivamos el modo inverso (pixels en blanco)
  display.reverse(false);
  
  //Escribimos el texto directamente sobre el sprite
  byte errorCode=v.printOver("/5x7d.fon","20190829",1);
  Serial.println("Test16. printOver 20190829 = "+String(errorCode));
  //Creamos el sprite monocromo que contendrá el texto
  RoJoSprite textSprite(1);
  //Utilizamos una fuente más grande que crea un texto más grande que la anchura del display
  errorCode=textSprite.print("/10x15d.fon","123456789012",1);
  Serial.println("Test16. print 123456789012 = "+String(errorCode));
  //Dibujamos el sprite de texto en la memoria de vídeo. No se verá el final
  v.drawSprite(&textSprite,5,16);
  //Lo mostramos de nuevo desplazado a la izquierda, comenzando desde una
  //posición fuera de pantalla. No se verá el principio
  v.drawSprite(&textSprite,-33,32);
  //Mostramos la memoria de vídeo
  display.drawSprite(&v);
  //Borramos el sprite utilizado
  textSprite.end();

  delay(1000);
  Serial.println("Test16. end");
}

//Rotar sprites
void Test17() {
  Serial.println("Test17. start");
  //Limpiamos la memoria de vídeo
  v.clear();
  
  //Creamos el sprite monocromo que contendrá el texto
  RoJoSprite textSprite(1);
  //Escribimos el texto en el sprite
  byte errorCode=textSprite.print("/10x15d.fon","123",1);
  Serial.println("Test17. print 123 = "+String(errorCode));
  //Dibujamos el sprite de texto en la memoria de vídeo
  v.drawSprite(&textSprite,30,0);
  //Creamos el sprite monocromo que contendrá el texto rotado
  RoJoSprite textSpriteRotate(1);
  //Rotamos el sprite de texto 90 grados
  textSpriteRotate.rotate(&textSprite,90);
  //Dibujamos el sprite de texto rotado en la memoria de vídeo
  v.drawSprite(&textSpriteRotate,60,16);
  //Rotamos el sprite de texto 180 grados
  textSpriteRotate.rotate(&textSprite,180);
  //Dibujamos el sprite de texto rotado en la memoria de vídeo
  v.drawSprite(&textSpriteRotate,30,46);
  //Rotamos el sprite de texto 270 grados
  textSpriteRotate.rotate(&textSprite,270);
  //Dibujamos el sprite de texto rotado en la memoria de vídeo
  v.drawSprite(&textSpriteRotate,14,16);
  
  //Mostramos la memoria de vídeo
  display.drawSprite(&v);
  //Borramos los sprites utilizados
  textSpriteRotate.end();
  textSprite.end();

  delay(2000);
  Serial.println("Test17. end");
}

//Flip
void Test18() {
  Serial.println("Test18. start");
  //Limpiamos la memoria de vídeo
  v.clear();
  
  //Creamos el sprite monocromo que contendrá el texto
  RoJoSprite textSprite(1);
  //Escribimos el texto en el sprite
  byte errorCode=textSprite.print("/10x15d.fon","123",1);
  Serial.println("Test18. print 123 = "+String(errorCode));
  //Dibujamos el sprite de texto en la memoria de vídeo
  v.drawSprite(&textSprite);
  //Creamos el sprite monocromo que contendrá el texto volteado
  RoJoSprite textSpriteFlip(1);
  //Volteamos el sprite en horizontal
  textSpriteFlip.flipH(&textSprite);
  //Dibujamos el sprite de texto volteado en horizontal en la memoria de vídeo
  v.drawSprite(&textSpriteFlip,33,0);
  //Volteamos el sprite de texto en vertical
  textSpriteFlip.flipV(&textSprite);
  //Dibujamos el sprite de texto volteado en vertical en la memoria de vídeo
  v.drawSprite(&textSpriteFlip,0,16);
  //Mostramos la memoria de vídeo
  display.drawSprite(&v);
  //Borramos los sprites utilizados
  textSpriteFlip.end();
  textSprite.end();

  delay(2000);
  Serial.println("Test18. end");
}

//Redimensionar texto
void Test19() {
  Serial.println("Test19. start");
  //Crearemos un sprite de texto
  //Lo redimensionaremos al tamaño del display
  //Lo mostraremos

  //Creamos el sprite monocromo para el texto
  RoJoSprite textSprite(1);
  //Escribimos el texto con una fuente pequeña
  textSprite.print("/5x7d.fon","1234",1);
  //Redimensionamos el texto al tamaño del display
  v.resize(&textSprite,display.xMax(),display.yMax());
  //Mostramos el sprite en el display
  display.drawSprite(&v);
  //Borramos los sprites utilizados
  textSprite.end();

  delay(1000);
  Serial.println("Test19. end");
}

//Mostrar texto con más fuentes
void Test20() {
  //Limpiamos la memoria de vídeo
  v.clear();
  //Escribimos con distintos tamaños de fuente
  v.printOver("/3x5.fon","Hello world!",1);
  v.printOver("/5x7.fon","Hello world!",1,0,8);
  v.printOver("/7x11.fon","Hello world!",1,0,16);
  v.printOver("/10x15.fon","Hello world!",1,0,32);
  //Para escribir con borde necesitamos crear un sprite
  RoJoSprite textSprite(1);
  textSprite.print("/10x15.fon","Hello",0,0,1);
  v.drawSprite(&textSprite,0,48);
  textSprite.end();
  //Lo mostramos
  display.drawSprite(&v);

  delay(2000);
}

//Círculos y elipses
void Test21() {
  //Limpiamos la memoria de vídeo
  v.clear();

  v.circle(15,15,14,1);
  v.disk(15,45,14,1);
  v.ellipse(60,15,29,14,1);
  v.ellipseFill(60,45,29,14,1);
  //Lo mostramos
  display.drawSprite(&v);

  delay(1000);
}

//Test de triángulos
//  RoJoSprite::triangle,triangleFill
void Test22() {
  //Limpiamos la memoria de vídeo
  v.clear();
  //Dibujamos un triángulo relleno
  v.triangleFill(20,0,50,40,0,20,1);
  //Dibujamos un triángulo sin relleno
  v.triangle(20,40,80,10,40,60,1);
  //Dibujamos un triángulo relleno
  v.triangleFill(50,10,90,30,70,60,1);
  //Lo mostramos
  display.drawSprite(&v);
    
  delay(1000);
}

void setup() {
  Serial.begin(115200); //DEBUG

  //Inicializamos el display con los pines en los que está conectado la placa
  //void begin(byte pinSDA,byte pinSCL,byte pinRST,uint32_t freqCOMM=0);
  display.begin(pinSDA,pinSCL,pinRES);

  //Dimensionamos el sprite que usaremos como memoria de vídeo con el mismo
  //tamaño que el display
  v.setSize(display.xMax(),display.yMax());
}

void loop() {
  #if !defined(ARDUINO_AVR_UNO) && !defined(ARDUINO_AVR_NANO)
    Test1(); //Función drawPixel con color 1 = dibujar
    Test2(); //Función drawPixel con color 2 = invertir
    Test3(); //Función drawPixel con color 0 = borrar
    Test4(); //Función getPixel
    Test5(); //Rectángulos
    Test6(); //Líneas
    Test7(); //Sprites definidos en programa.
    Test8(); //Leer sprite desde un archivo
    Test9(); //Mover sprite por pantalla
    Test10(); //Sincronización de sprites
    Test11(); //Mover sprite por pantalla manteniendo una imagen de fondo
    Test12(); //Mover sprite por pantalla manteniendo una imagen de fondo no transparente
    Test13(); //Obtener sprite de parte de otro
    Test14(); //Leer un bmp y guardar un sprite
    Test15(); //Resize
    Test16(); //Mostrar texto
    Test17(); //Rotar sprites
    Test18(); //Flip
    Test19(); //Redimensionar texto
    Test20(); //Mostrar texto con más fuentes
    Test21(); //Círculos y elipses
    Test22(); //Test de triángulos
  #endif
}
