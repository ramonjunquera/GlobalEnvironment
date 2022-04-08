/*
  Autor: Ramón Junquera
  Fecha: 20220408
  Tema: Librería de gestión de .bmp (RoJoFileBMP)
  Objetivo: Demo de librería RoJoFileBMP

  Descripción:
  Descripción y demostración de funcionalidades de la librería.
  
  Resultado:
    Realizamos varios tests cíclicos
*/

#include <Arduino.h>
#include <RoJoILI9341.h> //Librería de gestión del display ILI9341
#include <RoJoSprite.h> //Gestión de sprites
#include <RoJoFileBMP.h>

//Definición de pines
#ifdef ESP8266 //Si es un ESP8266...
  const byte pinDC_display=D2;
  const byte pinRES_display=D1;
  const byte pinCS_display=D3;
  const byte pinBackLight_display=255; //No utilizado
#elif defined(ARDUINO_M5STACK_FIRE) //Si es un M5Stack Fire...los pines son fijos
  const byte pinDC_display=27;
  const byte pinRES_display=33;
  const byte pinCS_display=14;
  const byte pinBackLight_display=32; //Ya viene conectado
#elif defined(ESP32) //Si es un cualquier otro ESP32...
  const byte pinDC_display=22;
  const byte pinRES_display=2;
  const byte pinCS_display=5;
  const byte pinBackLight_display=255; //No utilizado
#elif defined(__arm__) //Si es una Raspberry Pi
  const byte pinDC_display=24;
  const byte pinRES_display=25;
  const byte pinCS_display=8;
  const byte pinBackLight_display=255; //No utilizado
#elif defined(ARDUINO_ARCH_AVR) //Si es una placa Arduino
  const byte pinDC_display=5;
  const byte pinRES_display=4;
  const byte pinCS_display=3;
  const byte pinBackLight_display=255; //No utilizado
#endif

//Creamos objeto de gestión
RoJoILI9341 display;

//Test de color
void test1() {
  const String filename="/test1.bmp";
  Serial.println(filename);
  byte color[7][3]={ //Creamos un array con los componentes de color (RGB) de cada una de las columnas
     {255,255,255} 
    ,{255,0,0}     //col 1 = red
    ,{0,255,0}     //col 2 = green
    ,{0,0,255}     //col 3 = blue
    ,{255,255,0}   //col 4 = yellow
    ,{255,0,255}   //col 5 = magenta
    ,{0,255,255}   //col 6 = cyan
  };
  RoJoFileBMP bmp;
  //Si no podemos crear el archivo...informamos
  if(!bmp.create(filename,display.xMax(),display.yMax())) Serial.println("Error creando "+filename);
  else {
    for(uint16_t y=0;y<10;y++) { //Recorremos las 10 filas
      for(uint16_t x=0;x<7;x++) { //Recorremos las 7 columnas
        Serial.print(String(7*y+x)+"/70 ");
        //Pintamos el rectángulo relleno
        bmp.block(x*32+8,y*32+8,16,16,bmp.getColor(color[x][0],color[x][1],color[x][2]));
        //Cambiamos el color para el próximo ciclo
        for(byte i=0;i<3;i++) color[x][i]=color[x][i]*5/6;
      }
    }
    Serial.println();
    bmp.close(); //Hemos terminado de modificar el bmp
    display.drawBMP(filename); //Mostramos el bmp en el display
    RoJoFS.remove(filename); //Eliminamos el archivo bmp
    delay(1000);
  }
}

//Test de pixels
void test2() {
  const String filename="/test2.bmp";
  Serial.println(filename);
  RoJoFileBMP bmp;
  bmp.create(filename,display.xMax(),display.yMax()); //Creamos bmp color del tamaño del display
  //Dibujamos una matriz de puntos
  for(int16_t y=bmp.yMax();y>=0;y-=5)
    for(int16_t x=bmp.xMax();x>=0;x-=5) {
      bmp.drawPixel(x,y,x*y/2);
    }
  bmp.close(); //Hemos terminado de modificar el bmp
  display.drawBMP(filename); //Mostramos el bmp en el display
  RoJoFS.remove(filename); //Eliminamos el archivo bmp
  delay(1000);
}

//Test de líneas
void test3() {
  const String filename="/test3.bmp";
  Serial.println(filename);
  RoJoFileBMP bmp;
  bmp.create(filename,display.xMax(),display.yMax()); //Creamos bmp color del tamaño del display
  //Dibujamos líneas verdes
  Serial.print("1/2 ");
  for(uint16_t x=0;x<bmp.xMax();x+=50) {
    bmp.line(0,0,x,bmp.yMax()-1,bmp.getColor(0,255,0));
    #ifdef ESP8266
      yield();
    #endif
  }
  bmp.close(); //Hemos terminado de modificar el bmp
  display.drawBMP(filename); //Mostramos el bmp en el display
  
  bmp.open(filename); //Abrimos de nuevo el bmp creado y seguimos editándolo
  //Dibujamos líneas azules
  Serial.println("2/2");
  for(uint16_t x=0;x<bmp.xMax();x+=50) {
	  bmp.line(0,bmp.yMax()-1,x,0,bmp.getColor(0,0,255));
	  #ifdef ESP8266
      yield();
    #endif
  }
  bmp.close(); //Hemos terminado de modificar el bmp
  display.drawBMP(filename); //Mostramos el bmp en el display
  
  RoJoFS.remove(filename); //Eliminamos el archivo bmp
  delay(1000);
}

//Test de rectángulos
void test4() {
  const String filename="/test4.bmp";
  Serial.println(filename);
  RoJoFileBMP bmp;
  bmp.create(filename,display.xMax(),display.yMax()); //Creamos bmp color del tamaño del display

  bmp.block(40,40,80,120,bmp.getColor(255,0,0)); //Dibujamos un rectángulo relleno rojo
  bmp.rect(80,120,120,120,bmp.getColor(0,255,0)); //Dibujamos un rectángulo sin relleno verde
  bmp.block(40,200,120,80,bmp.getColor(0,0,255)); //Dibujamos un rectángulo relleno azul

  bmp.close(); //Hemos terminado de modificar el bmp
  display.drawBMP(filename); //Mostramos el bmp en el display
  RoJoFS.remove(filename); //Eliminamos el archivo bmp
  
  delay(1000);
}

//Test de triángulos
void test5() {
  const String filename="/test5.bmp";
  Serial.println(filename);
  RoJoFileBMP bmp;
  bmp.create(filename,display.xMax(),display.yMax()); //Creamos bmp color del tamaño del display

  //Dibujamos un triángulo relleno rojo
  Serial.print("1/3 ");
  bmp.triangleFill(80,0,200,160,0,120,bmp.getColor(255,0,0));
  //Dibujamos un triángulo sin relleno verde
  Serial.print("2/3 ");
  bmp.triangle(80,80,200,280,40,240,bmp.getColor(0,255,0));
  //Dibujamos un triángulo relleno azul
  Serial.println("3/3");
  bmp.triangleFill(80,160,239,200,120,280,bmp.getColor(0,0,255));

  bmp.close(); //Hemos terminado de modificar el bmp
  display.drawBMP(filename); //Mostramos el bmp en el display
  RoJoFS.remove(filename); //Eliminamos el archivo bmp
    
  delay(1000);
}

//Test de circunferencias y elipses
void test6() {
  const String filename="/test6.bmp";
  Serial.println(filename);
  RoJoFileBMP bmp;
  bmp.create(filename,display.xMax(),display.yMax()); //Creamos bmp color del tamaño del display

  bmp.circle(60,60,10,bmp.getColor(255,0,0)); //Circunferencia
  bmp.disk(180,260,10,bmp.getColor(0,255,0)); //Círculo
  bmp.ellipse(180,100,10,15,bmp.getColor(0,0,255)); //Elipse
  bmp.ellipseFill(60,220,10,15,bmp.getColor(255,0,255)); //Elipse rellena

  bmp.close(); //Hemos terminado de modificar el bmp
  display.drawBMP(filename); //Mostramos el bmp en el display
  RoJoFS.remove(filename); //Eliminamos el archivo bmp
  
  delay(1000);
}

//Test de carga de sprite desde archivo
void test7() {
  const String filename="/test7.bmp";
  Serial.println(filename);
  RoJoFileBMP bmp;
  bmp.create(filename,64,64); //Creamos bmp color

  //Dibujamos el sprite como una matriz de 2x2
  for(uint16_t y=0;y<2;y++) //Se arriba a abajo
    for(uint16_t x=0;x<2;x++) { //De izquierda a derecha
      bmp.drawSprite("/heart.spr",x*32,y*32);
      #ifdef ESP8266
        yield();
      #endif
    }

  bmp.close(); //Hemos terminado de modificar el bmp
  display.clear();
  display.drawBMP(filename,88,128); //Mostramos el bmp en el centro del display
  RoJoFS.remove(filename); //Eliminamos el archivo bmp
  
  delay(2000); //Esperamos
}

//Test de texto
void test8() {
  const String filename="/test8.bmp";
  Serial.println(filename);
  RoJoFileBMP bmp;
  bmp.create(filename,display.xMax(),display.yMax()); //Creamos bmp color del tamaño del display

  //Definimos color de texto
  uint32_t color=bmp.getColor(0,255,0); //green
  //Escribimos un texto y lo mostramos
  Serial.print("1/6 ");
  bmp.printOver("/5x7d.fon","20190528",color);
  //Utilizaremos otra fuente más grande
  Serial.print("2/6 ");
  bmp.printOver("/10x15d.fon","20190530",color,0,8);
  //Cambiamos el color del texto
  color=bmp.getColor(255,255,0); //yellow
  //Imprimimos fuera de pantalla
  Serial.print("3/6 ");
  bmp.printOver("/10x15d.fon","1234567890",color,-35,25); //No se ve el principio
  Serial.print("4/6 ");
  bmp.printOver("/10x15d.fon","1234567890",color,192,25); //No se ve el final
  //Fuente más grande
  Serial.print("5/6 ");
  bmp.printOver("/21x33d.fon","1234567890",color,0,45);
  //Cambiamos el color
  color=bmp.getColor(255,0,0); //red
  Serial.println("6/6");
  bmp.printOver("/7x11.fon","Hola, esto es una prueba!",color,0,80);

  bmp.close(); //Hemos terminado de modificar el bmp
  display.drawBMP(filename); //Mostramos el bmp
  RoJoFS.remove(filename); //Eliminamos el archivo bmp
  
  delay(2000); //Esperamos
}

//Test monocromo
void test9() {
  const String filename="/test9.bmp";
  Serial.println(filename);
  RoJoFileBMP bmp;
  //Si no podemos crear el bmp monocromo del tamaño del display...error
  if(!bmp.create(filename,display.xMax(),display.yMax(),0)) Serial.println("Error creando archivo");

  //bmp.drawPixel(50,50,1);
  bmp.line(0,0,50,50,1);
  bmp.line(50,0,50,50,1);
  bmp.line(0,50,50,50,1);
  bmp.block(52,1,15,50,1);
  bmp.line(68,0,68,50,1);
  bmp.rect(70,1,50,50,1);

  bmp.printOver("/10x15d.fon","20220405",1,0,70);

  bmp.circle(25,125,25,1);
  bmp.disk(50,100,5,1);
  bmp.ellipse(100,125,15,25,1);

  bmp.close(); //Hemos terminado de modificar el bmp

  display.clear();
  Serial.println("drawBMP error="+String((int)display.drawBMP(filename))); //Mostramos el bmp
  RoJoFS.remove(filename); //Eliminamos el archivo bmp
  
  delay(1000); //Esperamos

}

void setup() {
  Serial.begin(115200);
  while(!Serial) delay(1);
  display.begin(pinRES_display,pinDC_display,pinCS_display,pinBackLight_display);
}

void loop() {
  //Tests de funciones de dibujo directo desde el driver
  test1(); //Test de block
  test2(); //Test de pixels
  test3(); //Test de líneas
  test4(); //Test de rectángulos
  test5(); //Test de triángulos
  test6(); //Test de circunferencias y elipses
  test7(); //Test de carga de sprite desde archivo
  test8(); //Test de texto
  test9(); //Test de monocromo
}
