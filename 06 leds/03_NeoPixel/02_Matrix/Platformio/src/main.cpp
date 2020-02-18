/*
  Autor: Ramón Junquera
  Fecha: 20200216
  Tema: matriz CJMCU-64 (NeoPixel)
  Objetivo: Demo de gestión de leds NeoPixel

  Descripción:
    Sucesión de varios ejemplos

  Notas:
    Arduino Nano
    - Aunque este ejemplo contempla su ejecución en esta placa, no
      funcionará debido a que no tiene memoria suficiente. No por no
      reproducir cada uno de las demos por separado, sino porque son
      demasiadas. Si se deja una sola demo, funcionará correctamente.
    - Esta placa no proporciona suficiente energía a la matriz como para
      que funcione correctamente. Es necesaria una fuente de alimentación
      externa (con su correspondiente puente de masa).
    Arduino UNO
    - Funcionarán todos las demos menos la última (Fire) debido a una
      falta de memoria.
*/

#include <Arduino.h>
#include <RoJoNeoPixel.h> //Gestión de leds NeoPixel

RoJoNeoPixel leds; //Objeto de gestión de leds NeoPixel

void setup() {
  //Definición de pinout
  #ifdef ARDUINO_ARCH_AVR //Placas Arduino: Mega, Nano, UNO
    byte pinComm=3;
  #elif defined(ESP32)
    byte pinComm=13;
  #elif defined(ESP8266)
    byte pinComm=D8;
  #endif
  leds.begin(8,8,pinComm); //Inicialización de leds NeoPixel. Matriz de 8x8
}

//Single led
void test1() {
  leds.v->clear(); //Apagamos todos los leds
  for(byte y=0;y<8;y++) {
    for(byte x=0;x<8;x++) {
      leds.v->drawPixel(x,y,{99,99,99}); //Encendemos el led procesado en gris
      leds.draw(); //Mostramos la configuración actual
      leds.v->drawPixel(x,y,{0,0,0}); //Apagamos el led procesado para que no aparezca en el siguiente ciclo
      delay(100); //Esperamos un momento
    }
  }
}

//Colores planos
void test2() {
  leds.v->clear({99,0,0}); //Rojo
  leds.draw();
  delay(1000);
  leds.v->clear({0,99,0}); //Verde
  leds.draw();
  delay(1000);
  leds.v->clear({0,0,99}); //Azul
  leds.draw();
  delay(1000);
  leds.v->clear({99,99,0}); //Amarillo
  leds.draw();
  delay(1000);
  leds.v->clear({99,0,99}); //Magenta
  leds.draw();
  delay(1000);
  leds.v->clear({0,99,99}); //Cyan
  leds.draw();
  delay(1000);
  leds.v->clear({99,99,99}); //Blanco
  leds.draw();
  delay(1000);
  leds.v->clear(); //Negro
  leds.draw();
  delay(1000);
}

//Texto scroll
void test3() {
  RoJoSprite spr(24);
  spr.print("/5x7.fon","Hola mundo!",{0,99,0}); //En verde
  int16_t xMax=-(int16_t)spr.xMax();
  for(int16_t x=8;x>xMax;x--) {
    leds.v->clear();
    leds.v->drawSprite(&spr,x,0);
    leds.draw();
    delay(100);
  }
  spr.end();
}

//Dimming
void test4() {
  leds.v->clear({});
  RoJoSprite blankSpr(24); //Sprite negro
  blankSpr.setSize(5,8); //Mismo tamaño que una letra
  RoJoColor colors[7]={ //Tabla de colores
    {99,0,0}, //Rojo
    {0,99,0}, //Verde
    {0,0,99}, //Azul
    {99,99,0}, //Amarillo
    {99,0,99}, //Magenta
    {0,99,99}, //Cyan
    {99,99,99} //Blanco
  };
  RoJoSprite number(24); //Sprite con el número
  RoJoSprite res(24); //Sprite con el resultado
  for(byte i=1;i<=7;i++) { //Contamos del 1 al 7...
    number.print("/5x7.fon",String(i),colors[i-1]); //Dibujamos el número en el sprite
    for(byte level=0;level<255;level++) { //Recorremos todos los niveles...
      res.fade(&blankSpr,&number,level); //Creamos el sprite resultado para este nivel
      leds.v->drawSprite(&res,1); //Lo dibujamos en la memoria de vídeo
      leds.draw(); //Lo mostramos
      delay(4);
    }
    //Como antes pero ahora pasamos de número a negro
    for(byte level=255;level>0;level--) {
      res.fade(&blankSpr,&number,level);
      leds.v->drawSprite(&res,1);
      leds.draw();
      delay(4);
    }
    delay(400); //Esperamos un momento con los leds apagados
  }
  res.end();
  number.end();
  blankSpr.end();
}

//Fading
void test5() {
  leds.v->clear();
  RoJoSprite spr1(24),spr2(24),res(24); //Definimos los sprites para hacer el fade
  RoJoColor colors[7]={ //Tabla de colores
    {99,0,0}, //Rojo
    {0,99,0}, //Verde
    {0,0,99}, //Azul
    {99,99,0}, //Amarillo
    {99,0,99}, //Magenta
    {0,99,99}, //Cyan
    {99,99,99} //Blanco
  };
  spr2.print("/5x7.fon","1",colors[0]); //Creamos el primer número en el sprite 2
  leds.v->drawSprite(&spr2,1); //Lo pasamos a la memoria de vídeo
  leds.draw(); //Lo mostramos
  delay(500); //Esperamos 
  for(byte i=2;i<=7;i++) { //Recorremos los números del 2 al 7
    spr1.copy(&spr2); //Copiamos el sprite 2 en el 1
    spr2.print("/5x7.fon",String((int)i),colors[i-1]); //Creamos el número en el sprite 2
    for(uint16_t level=0;level<256;level++) { //hacemos el fade del sprite 1 al 2
      res.fade(&spr1,&spr2,level); //Creamos el sprite con el level correcto
      leds.v->drawSprite(&res,1); //Dibujamos el resultado
      leds.draw(); //Lo mostramos
      delay(4);
    }
    delay(500); //Tras el fade esperamos un momento
  }
  res.end();
  spr1.end();
  spr2.end();
}

//Kitt circular
void test6() {
  byte x,y; //Coordenadas
  byte levels[28]; //Niveles de intensidad de cada punto. Sólo canal rojo
  for(byte i=0;i<28;i++) levels[i]=0;;
  leds.v->clear(); //Borramos imágen
  for(byte cycles=0;cycles<4;cycles++) { //Número de vueltas...
    for(byte p=0;p<28;p++) { //Recorremos todas las posiciones del círculo...
      levels[p]=255; //Posición actual a brillo máximo
      for(byte steps=0;steps<13;steps++) { //Número de pasos por posición...
        //Dibujamos la situación actual
        for(byte i=0;i<28;i++) { //Recorremos todas la posiciones del círculo...
          //Calculamos las coordenadas de la posición actual
          if(i<8) { //Si estamos en la primera fila...
            x=i; y=0;
          } else if (i<15) { //Si estamos en la última columna...
            x=7; y=i-7;
          } else if(i<22) { //Si estamos en la última fila...
            x=21-i; y=7;
          } else { //Si estamos en la primera columna...
            x=0; y=28-i;
          }
          leds.v->drawPixel(x,y,{levels[i],0,0});
        }
        leds.draw();
        //Dimming
        for(byte i=0;i<28;i++) {
          //if(levels[i]<10) levels[i]=0;
          //else levels[i]-=10;
          if(levels[i]>0) levels[i]--;
        }
        delay(5);
      }
    }
  }
}

//Carga y digujo de archivos bmp
void test7() {
  //leds.v->loadBMP("/heart.bmp");
  leds.v->loadBMP("/heart.bmp");
  leds.draw();
  delay(2000);
  leds.v->loadBMP("/house.bmp");
  leds.draw();
  delay(2000);
  leds.v->loadBMP("/smiley.bmp");
  leds.draw();
  delay(2000);
}

//Glitter
void test8() {
  byte colorChannel;
  leds.v->clear(); //Borramos imágen
  uint32_t maxTime=millis()+10000; //Definimos duración (6s)
  while(millis()<maxTime) {
    if(random(10)==3) leds.v->drawPixel(random(8),random(8),{255,255,255}); //Una de cada 10 veces...pintamos un pixel de blanco
    //Dimming
    for(byte i=0;i<20;i++) {
      for(byte y=0;y<8;y++) {
        for(byte x=0;x<8;x++) {
          colorChannel=leds.v->getPixel(x,y).channels[0]; //Obtenemos el canal rojo del pixel procesado
          if(colorChannel>0) colorChannel--;
          leds.v->drawPixel(x,y,{colorChannel,colorChannel,colorChannel}); //Todos los canales son iguales
        }
      }
      leds.draw();
    }
  }
}

//Fire
void test9() {
  //Creamos la paleta de colores de fuego
  RoJoColor fireColor[256];
  for(byte i=0;i<85;i++) {
    fireColor[i]={(byte)(i*3),0,0}; //R
    fireColor[i+85]={255,(byte)(i*3),0}; //G
    fireColor[i+170]={255,255,(byte)(i*3)}; //B
  }
  fireColor[255]={255,255,255};
  byte heat[8][8]; //Creamos array de calor
  byte descT; //Descenso de temperatura
  leds.v->clear();
  uint32_t maxTime=millis()+10000; //Definimos duración (6s)
  while(millis()<maxTime) {
    //Enfriamos todo un poco
    for(byte y=0;y<8;y++) {
      for(byte x=0;x<8;x++) {
        descT=random(14); //Decidimos cuánto se reducirá la temperatura para este pixel
        if(heat[x][y]>descT) heat[x][y]-=descT;
        else heat[x][y]=0;
      }
    }
    //Propagación del calor
    //Calor=8*(calor propio)+4*(calor inferior)+(calor izquierdo)+(calor derecho)
    for(byte y=0;y<8;y++) { //Recorremos las filas de arriba a abajo
      byte heatRow[8]; //Calor de la fila procesada
      for(byte x=0;x<8;x++) {
        uint16_t h=heat[x][y]*8; //Sumatorio de calor = calor propio
        uint16_t elements=8; //Número de elementos = calor propio
        if(y<7) { //Si no estamos en la última fila...
          h+=heat[x][y+1]*4;
          elements+=4;
        }
        if(x>0) { //Si no es la primera columna...
          h+=heat[x-1][y];
          elements++;
        }
        if(x<7) { //Si no es la última columna...
          h+=heat[x+1][y];
          elements++;
        }
        heatRow[x]=h/elements; //Calculamos el promedio de calor y lo guardamos
      }
      for(byte x=0;x<8;x++) heat[x][y]=heatRow[x]; //Guardamos el calor calculado
    }
    //A veces saltan chispas que avivan el fuego
    for(byte x=0;x<8;x++) {
      if(random(25)==13) heat[x][7]=random(160,256); //Una de cada 25 veces salta una chispa de calor pseudoaleatorio
    }
    //Converimos el calor en color
    for(byte y=0;y<8;y++) {
      for(byte x=0;x<8;x++) {
        leds.v->drawPixel(x,y,fireColor[heat[x][y]]);
      }
    }
    leds.draw();
    delay(30);
  } //end while
}

void loop() {
  test1(); //Single led
  test2(); //Colores planos
  test3(); //Texto scroll
  test4(); //Dimming
  test5(); //Fading
  test6(); //Kitt circular
  test7(); //Load bmp
  test8(); //Glitter
  test9(); //Fire
}