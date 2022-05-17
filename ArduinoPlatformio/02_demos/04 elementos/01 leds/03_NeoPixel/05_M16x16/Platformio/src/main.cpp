/*
  Autor: Ramón Junquera
  Fecha: 20220517
  Tema: matriz M16x16 (NeoPixel)
  Objetivo: Demo de gestión de leds NeoPixel

  Descripción:
    Sucesión de varios ejemplos
*/

#include <Arduino.h>
#include <RoJoNeoPixelM16x16.h> //Gestión de leds NeoPixel

RoJoNeoPixelM16x16 leds; //Objeto de gestión de leds NeoPixel

void setup() {
  Serial.begin(115200); //DEBUG
  //Definición de pinout
  #ifdef ARDUINO_ARCH_AVR //Placas Arduino: Mega, Nano, UNO
    byte pinComm=3;
  #elif defined(ARDUINO_M5STACK_FIRE)
    byte pinComm=22;
  #elif defined(ESP32)
    byte pinComm=13;
  #elif defined(ESP8266)
    byte pinComm=D8;
  #endif
  leds.begin(pinComm); //Inicialización de leds CJMCU64
  Serial.println("Setup complete!");
}

//Single led
void test1() {
  leds.clear(); //Apagamos todos los leds
  for(byte y=0;y<16;y++) {
    for(byte x=0;x<16;x++) {
      leds.drawPixel(x,y,0x640000); //Encendemos el led procesado en rojo medio
      leds.show(); //Mostramos la configuración actual
      leds.drawPixel(x,y,0); //Apagamos el led procesado para que no aparezca en el siguiente ciclo
      delay(10); //Esperamos un momento
      delay(10); //Esperamos un momento
    }
  }
}

//Colores planos
void test2() {
  leds.clear(0x320000); //Rojo
  leds.show();
  delay(1000);
  leds.clear(0x003200); //Verde
  leds.show();
  delay(1000);
  leds.clear(0x000032); //Azul
  leds.show();
  delay(1000);
  leds.clear(0x323200); //Amarillo
  leds.show();
  delay(1000);
  leds.clear(0x320032); //Magenta
  leds.show();
  delay(1000);
  leds.clear(0x003232); //Cyan
  leds.show();
  delay(1000);
  leds.clear(0x323232); //Blanco
  leds.show();
  delay(1000);
  leds.clear(); //Negro
  leds.show();
  delay(1000);
}

//Kitt circular
void test3() {
  byte x,y; //Coordenadas
  byte levels[60]; //Niveles de intensidad de cada punto. Sólo canal rojo
  for(byte i=0;i<60;i++) levels[i]=0;;
  leds.clear(); //Borramos imágen
  for(byte cycles=0;cycles<4;cycles++) { //Número de vueltas...
    for(byte p=0;p<60;p++) { //Recorremos todas las posiciones del círculo...
      levels[p]=255; //Posición actual a brillo máximo
      for(byte steps=0;steps<6;steps++) { //Número de pasos por posición...
        //Dibujamos la situación actual
        for(byte i=0;i<60;i++) { //Recorremos todas la posiciones del círculo...
          //Calculamos las coordenadas de la posición actual
          if(i<16) { //Si estamos en la primera fila...
            x=i; y=0;
          } else if (i<31) { //Si estamos en la última columna...
            x=15; y=i-15;
          } else if(i<46) { //Si estamos en la última fila...
            x=45-i; y=15;
          } else { //Si estamos en la primera columna...
            x=0; y=60-i;
          }
          leds.drawPixel(x,y,((uint32_t)levels[i])<<16);
        }
        leds.show();
        //Dimming
        for(byte i=0;i<60;i++) {
          if(levels[i]>0) levels[i]--;
        }
        delay(1);
      }
    }
  }
}

//Glitter
void test4() {
  leds.clear();
  uint32_t maxTime=millis()+10000; //Definimos duración (10s)
  while(millis()<maxTime) {
    if(random(3)==1) leds.drawPixel(random(16),random(16),0x999999); //Una de cada 3 veces...pintamos un pixel de blanco
    //Dimming
    for(byte i=0;i<10;i++) {
      for(byte y=0;y<16;y++) {
        for(byte x=0;x<16;x++) {
          uint32_t color=leds.getPixel(x,y)&0xFF;
          if(color) {
            color--;
            leds.drawPixel(x,y,(color<<16)+(color<<8)+color); //Todos los canales son iguales
          }
        }
      }
      leds.show();
      delay(1);
    }
  }
}

//Carga y dibujo de archivos bmp y spr
void test5() {
  leds.clear();
  RoJoSprite spr(3); //Color real
  spr.loadSprite("/house.spr");
  leds.drawSprite(&spr);
  leds.show();
  delay(2000);
  spr.loadBMP("/smiley.bmp");
  leds.drawSprite(&spr);
  leds.show();
  delay(2000);
  spr.end();
  spr.loadBMP("/heart.bmp");
  leds.drawSprite(&spr);
  leds.show();
  delay(2000);
}

//Texto scroll
void test6() {
  RoJoSprite spr(3); //Sprite color real
  spr.print("/10x15.fon","Hola mundo!",0x004400); //En verde
  int16_t xMax=-(int16_t)spr.xMax();
  for(int16_t x=16;x>xMax;x--) {
    leds.clear();
    leds.drawSprite(&spr,x);
    leds.show();
    delay(50);
  }
  spr.end();
}

//Dimming
void test7() {
  leds.clear();
  uint32_t colors[7]={ //Tabla de colores
    0x550000, //Rojo
    0x005500, //Verde
    0x000055, //Azul
    0x555500, //Amarillo
    0x550055, //Magenta
    0x005555, //Cyan
    0x555555 //Blanco
  };
  RoJoSprite blankSpr(3); //Sprite negro
  blankSpr.setSize(16,16); //Mismo tamaño que el display
  RoJoSprite number(3); //Sprite con el número
  number.setSize(16,16); //Mismo tamaño que el display
  RoJoSprite res(3); //Sprite con el resultado
  for(byte i=1;i<=7;i++) { //Contamos del 1 al 7...
    number.clear();
    number.printOver("/10x15.fon",String(i),colors[i-1],3); //Dibujamos el número en el sprite
    for(byte level=0;level<255;level++) { //Recorremos todos los niveles...
      res.fade(&blankSpr,&number,level); //Creamos el sprite resultado para este nivel
      leds.drawSprite(&res); //Lo dibujamos en la memoria de vídeo
      leds.show(); //Lo mostramos
      delay(2);
    }
    //Como antes pero ahora pasamos de número a negro
    for(byte level=255;level>0;level--) {
      res.fade(&blankSpr,&number,level);
      leds.drawSprite(&res);
      leds.show();
      delay(2);
    }
    delay(400); //Esperamos un momento con los leds apagados
  }
  res.end();
  number.end();
  blankSpr.end();
}

//Fading
void test8() {
  leds.clear();
  RoJoSprite spr1(3),spr2(3),res(3); //Definimos los sprites para hacer el fade
  spr1.setSize(16,16); //Mismo tamaño que el display
  spr2.setSize(16,16); //Mismo tamaño que el display
  uint32_t colors[7]={ //Tabla de colores
    0x550000, //Rojo
    0x005500, //Verde
    0x000055, //Azul
    0x555500, //Amarillo
    0x550055, //Magenta
    0x005555, //Cyan
    0x555555 //Blanco
  };
  spr2.printOver("/10x15.fon","1",colors[0],3); //Creamos el primer número en el sprite 2
  leds.drawSprite(&spr2); //Lo pasamos a la memoria de vídeo
  leds.show(); //Lo mostramos
  delay(500); //Esperamos 
  for(byte i=2;i<=7;i++) { //Recorremos los números del 2 al 7
    spr1.copy(&spr2); //Copiamos el sprite 2 en el 1
    spr2.clear();
    spr2.printOver("/10x15.fon",String((int)i),colors[i-1],3); //Creamos el número en el sprite 2
    for(uint16_t level=0;level<256;level++) { //hacemos el fade del sprite 1 al 2
      res.fade(&spr1,&spr2,level); //Creamos el sprite con el level correcto
      leds.drawSprite(&res); //Dibujamos el resultado
      leds.show(); //Lo mostramos
      delay(2);
    }
    delay(500); //Tras el fade esperamos un momento
  }
  res.end();
  spr1.end();
  spr2.end();
}

//Fire
void test9() {
  RoJoSprite spr(3); //Color real
  spr.setSize(16,16);
  //Creamos la paleta de colores de fuego
  uint32_t fireColor[256];
  for(byte i=0;i<85;i++) {
    fireColor[i]=spr.getColor((byte)(i*3),0,0); //R
    fireColor[i+85]=spr.getColor(255,(byte)(i*3),0); //G
    fireColor[i+170]=spr.getColor(255,255,(byte)(i*3)); //B
  }
  fireColor[255]=spr.getColor(255,255,255);
  byte heat[16][16]; //Creamos array de calor
  byte descT; //Descenso de temperatura
  spr.clear();
  uint32_t maxTime=millis()+10000; //Definimos duración (10s)
  while(millis()<maxTime) {
    //Enfriamos todo un poco
    for(byte y=0;y<16;y++) {
      for(byte x=0;x<16;x++) {
        descT=random(14); //Decidimos cuánto se reducirá la temperatura para este pixel
        if(heat[x][y]>descT) heat[x][y]-=descT;
        else heat[x][y]=0;
      }
    }
    //Propagación del calor
    //Calor=10*(calor propio)+12*(calor inferior)+(calor izquierdo)+(calor derecho)
    for(byte y=0;y<16;y++) { //Recorremos las filas de arriba a abajo
      byte heatRow[16]; //Calor de la fila procesada
      for(byte x=0;x<16;x++) {
        uint16_t h=heat[x][y]*10; //Sumatorio de calor = calor propio
        uint16_t elements=10; //Número de elementos = calor propio
        if(y<15) { //Si no estamos en la última fila...
          h+=heat[x][y+1]*12;
          elements+=12;
        }
        if(x>0) { //Si no es la primera columna...
          h+=heat[x-1][y];
          elements++;
        }
        if(x<15) { //Si no es la última columna...
          h+=heat[x+1][y];
          elements++;
        }
        heatRow[x]=h/elements; //Calculamos el promedio de calor y lo guardamos
      }
      for(byte x=0;x<16;x++) heat[x][y]=heatRow[x]; //Guardamos el calor calculado
    }
    //A veces saltan chispas que avivan el fuego
    for(byte x=0;x<16;x++) {
      if(random(25)==13) heat[x][15]=random(160,256); //Una de cada 25 veces salta una chispa de calor pseudoaleatorio
    }
    //Converimos el calor en color
    for(byte y=0;y<16;y++) {
      for(byte x=0;x<16;x++) {
        spr.drawPixel(x,y,fireColor[heat[x][y]]);
      }
    }
    leds.drawSprite(&spr);
    leds.show();
    delay(30);
  } //end while
  spr.end();
}

void loop() {
  test1(); //Single led
  test2(); //Colores planos
  test3(); //Kitt circular
  test4(); //Glitter
  #if !defined(ARDUINO_ARCH_AVR)
    test5(); //Load bmp & spr
    test6(); //Texto scroll
    test7(); //Dimming
    test8(); //Fading
    test9(); //Fire
  #endif
}
