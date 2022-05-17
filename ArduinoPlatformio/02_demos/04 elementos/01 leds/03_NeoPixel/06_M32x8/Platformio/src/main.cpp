/*
  Autor: Ramón Junquera
  Fecha: 20220517
  Tema: matriz M32x8 (NeoPixel)
  Objetivo: Demo de gestión de leds NeoPixel

  Descripción:
    Sucesión de varios ejemplos
*/

#include <Arduino.h>
#include <RoJoNeoPixelM32x8.h> //Gestión de leds NeoPixel

RoJoNeoPixelM32x8 leds; //Objeto de gestión de leds NeoPixel

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
  leds.begin(pinComm);
  Serial.println("Setup complete!");
}

//Single led
void test1() {
  leds.clear(); //Apagamos todos los leds
  for(byte y=0;y<8;y++) {
    for(byte x=0;x<32;x++) {
      leds.drawPixel(x,y,0x640000); //Encendemos el led procesado en rojo medio
      leds.show(); //Mostramos la configuración actual
      leds.drawPixel(x,y,0); //Apagamos el led procesado para que no aparezca en el siguiente ciclo
      delay(10); //Esperamos un momento
    }
  }
}

//Colores planos
void test2() {
  leds.clear(0x220000); //Rojo
  leds.show();
  delay(1000);
  leds.clear(0x002200); //Verde
  leds.show();
  delay(1000);
  leds.clear(0x000022); //Azul
  leds.show();
  delay(1000);
  leds.clear(0x222200); //Amarillo
  leds.show();
  delay(1000);
  leds.clear(0x220022); //Magenta
  leds.show();
  delay(1000);
  leds.clear(0x002222); //Cyan
  leds.show();
  delay(1000);
  leds.clear(0x222222); //Blanco
  leds.show();
  delay(1000);
  leds.clear(); //Negro
  leds.show();
  delay(1000);
}

//Kitt circular
void test3() {
  byte x,y; //Coordenadas
  byte levels[76]; //Niveles de intensidad de cada punto. Sólo canal rojo
  for(byte i=0;i<76;i++) levels[i]=0;;
  leds.clear(); //Borramos imágen
  for(byte cycles=0;cycles<4;cycles++) { //Número de vueltas...
    for(byte p=0;p<76;p++) { //Recorremos todas las posiciones del círculo...
      levels[p]=255; //Posición actual a brillo máximo
      for(byte steps=0;steps<4;steps++) { //Número de pasos por posición...
        //Dibujamos la situación actual
        for(byte i=0;i<76;i++) { //Recorremos todas la posiciones del círculo...
          //Calculamos las coordenadas de la posición actual
          if(i<32) { //Si estamos en la primera fila...
            x=i; y=0;
          } else if (i<39) { //Si estamos en la última columna...
            x=31; y=i-31;
          } else if(i<70) { //Si estamos en la última fila...
            x=69-i; y=7;
          } else { //Si estamos en la primera columna...
            x=0; y=76-i;
          }
          leds.drawPixel(x,y,((uint32_t)levels[i])<<16);
        }
        leds.show();
        //Dimming
        for(byte i=0;i<76;i++) {
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
    if(random(3)==1) leds.drawPixel(random(32),random(8),0x999999); //Una de cada 3 veces...pintamos un pixel de blanco
    //Dimming
    for(byte i=0;i<10;i++) {
      for(byte y=0;y<8;y++) {
        for(byte x=0;x<32;x++) {
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
  spr.loadBMP("/llave.bmp");
  leds.drawSprite(&spr);
  leds.show();
  delay(2000);
  spr.end();
}

//Texto scroll
void test6() {
  RoJoSprite spr(3); //Sprite color real
  spr.print("/5x7.fon","Hola mundo!",0x005500); //En verde
  int16_t xMax=-(int16_t)spr.xMax();
  for(int16_t x=32;x>xMax;x--) {
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
  String texts[7]={ //Tabla de textos
    "uno","dos","tres","cuatro","cinco","seis","siete"
  };
  RoJoSprite blankSpr(3); //Sprite negro
  blankSpr.setSize(32,8); //Mismo tamaño que pantalla
  RoJoSprite text(3); //Sprite con el texto
  text.setSize(32,8); //Mismo tamaño que pantalla
  RoJoSprite res(3); //Sprite con el resultado
  for(byte i=0;i<7;i++) {
    text.clear();
    text.printOver("/5x7.fon",texts[i],colors[i]);
    for(byte level=0;level<254;level+=2) { //Recorremos todos los niveles...
      res.fade(&blankSpr,&text,level); //Creamos el sprite resultado para este nivel
      leds.drawSprite(&res); //Lo dibujamos en la memoria de vídeo
      leds.show(); //Lo mostramos
      delay(4);
    }
    //Como antes pero ahora pasamos de texto a negro
    for(byte level=254;level>0;level-=2) {
      res.fade(&blankSpr,&text,level);
      leds.drawSprite(&res);
      leds.show();
      delay(4);
    }
    delay(400); //Esperamos un momento con los leds apagados
  }
  res.end();
  text.end();
  blankSpr.end();
}

//Fading
void test8() {
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
  String texts[7]={ //Tabla de textos
    "uno","dos","tres","cuatro","cinco","seis","siete"
  };
  RoJoSprite spr1(3),spr2(3),res(3); //Definimos los sprites para hacer el fade
  spr1.setSize(32,8); //Mismo tamaño que el display
  spr2.setSize(32,8); //Mismo tamaño que el display
  spr2.printOver("/5x7.fon",texts[0],colors[0]); //Creamos el primer texto en el sprite 2
  leds.drawSprite(&spr2); //Lo pasamos a la memoria de vídeo
  leds.show(); //Lo mostramos
  delay(500); //Esperamos 
  for(byte i=1;i<7;i++) { //Recorremos los textos comenzando desde el segundo
    spr1.copy(&spr2); //Copiamos el sprite 2 en el 1
    spr2.clear();
    spr2.printOver("/5x7.fon",texts[i],colors[i]); //Creamos el texto en el sprite 2
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
  spr.setSize(32,8);
  //Creamos la paleta de colores de fuego
  uint32_t fireColor[256];
  for(byte i=0;i<85;i++) {
    fireColor[i]=spr.getColor((byte)(i*3),0,0); //R
    fireColor[i+85]=spr.getColor(255,(byte)(i*3),0); //G
    fireColor[i+170]=spr.getColor(255,255,(byte)(i*3)); //B
  }
  fireColor[255]=spr.getColor(255,255,255);
  byte heat[32][8]; //Creamos array de calor
  byte descT; //Descenso de temperatura
  spr.clear();
  uint32_t maxTime=millis()+10000; //Definimos duración (10s)
  while(millis()<maxTime) {
    //Enfriamos todo un poco
    for(byte y=0;y<8;y++) {
      for(byte x=0;x<32;x++) {
        descT=random(14); //Decidimos cuánto se reducirá la temperatura para este pixel
        if(heat[x][y]>descT) heat[x][y]-=descT;
        else heat[x][y]=0;
      }
    }
    //Propagación del calor
    //Calor=8*(calor propio)+4*(calor inferior)+(calor izquierdo)+(calor derecho)
    for(byte y=0;y<8;y++) { //Recorremos las filas de arriba a abajo
      byte heatRow[32]; //Calor de la fila procesada
      for(byte x=0;x<32;x++) {
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
        if(x<31) { //Si no es la última columna...
          h+=heat[x+1][y];
          elements++;
        }
        heatRow[x]=h/elements; //Calculamos el promedio de calor y lo guardamos
      }
      for(byte x=0;x<32;x++) heat[x][y]=heatRow[x]; //Guardamos el calor calculado
    }
    //A veces saltan chispas que avivan el fuego
    for(byte x=0;x<32;x++) {
      if(random(25)==13) heat[x][7]=random(160,256); //Una de cada 25 veces salta una chispa de calor pseudoaleatorio
    }
    //Converimos el calor en color
    for(byte y=0;y<8;y++) {
      for(byte x=0;x<32;x++) {
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
