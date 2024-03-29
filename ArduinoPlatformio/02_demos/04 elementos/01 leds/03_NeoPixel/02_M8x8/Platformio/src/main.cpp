/*
  Autor: Ramón Junquera
  Fecha: 20220517
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
*/

#include <Arduino.h>
#include <RoJoNeoPixelCJMCU64.h> //Gestión de leds NeoPixel

RoJoNeoPixelCJMCU64 leds; //Objeto de gestión de leds NeoPixel

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
  for(byte y=0;y<8;y++) {
    for(byte x=0;x<8;x++) {
      leds.drawPixel(x,y,0x640000); //Encendemos el led procesado en rojo medio
      leds.show(); //Mostramos la configuración actual
      leds.drawPixel(x,y,0); //Apagamos el led procesado para que no aparezca en el siguiente ciclo
      delay(100); //Esperamos un momento
    }
  }
}

//Colores planos
void test2() {
  leds.clear(0x640000); //Rojo
  leds.show();
  delay(1000);
  leds.clear(0x006400); //Verde
  leds.show();
  delay(1000);
  leds.clear(0x000064); //Azul
  leds.show();
  delay(1000);
  leds.clear(0x646400); //Amarillo
  leds.show();
  delay(1000);
  leds.clear(0x640064); //Magenta
  leds.show();
  delay(1000);
  leds.clear(0x006464); //Cyan
  leds.show();
  delay(1000);
  leds.clear(0x646464); //Blanco
  leds.show();
  delay(1000);
  leds.clear(); //Negro
  leds.show();
  delay(1000);
}

//Kitt circular
void test3() {
  byte x,y; //Coordenadas
  byte levels[28]; //Niveles de intensidad de cada punto. Sólo canal rojo
  for(byte i=0;i<28;i++) levels[i]=0;;
  leds.clear(); //Borramos imágen
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
          leds.drawPixel(x,y,((uint32_t)levels[i])<<16);
        }
        leds.show();
        //Dimming
        for(byte i=0;i<28;i++) {
          if(levels[i]>0) levels[i]--;
        }
        delay(5);
      }
    }
  }
}

//Carga y dibujo de archivos bmp y spr
void test4() {
  RoJoSprite spr(3); //Color real
  spr.loadBMP("/heart.bmp");
  leds.drawSprite(&spr);
  leds.show();
  delay(2000);
  spr.loadSprite("/house.spr");
  leds.drawSprite(&spr);
  leds.show();
  delay(2000);
  spr.loadBMP("/smiley.bmp");
  leds.drawSprite(&spr);
  leds.show();
  delay(2000);
  spr.end();
}

//Glitter
void test5() {
  RoJoSprite spr(3); //Color real
  spr.setSize(8,8);
  uint32_t maxTime=millis()+10000; //Definimos duración (6s)
  while(millis()<maxTime) {
    if(random(5)==1) spr.drawPixel(random(8),random(8),0x808080); //Una de cada 5 veces...pintamos un pixel de blanco
    //Dimming
    for(byte i=0;i<20;i++) {
      for(byte y=0;y<8;y++) {
        for(byte x=0;x<8;x++) {
          byte r,g,b;
          spr.getColor(spr.getPixel(x,y),&r,&g,&b); //Descomponemos color en canales
          if(r>0) r--;
          spr.drawPixel(x,y,spr.getColor(r,r,r)); //Todos los canales son iguales
        }
      }
      leds.drawSprite(&spr);
      leds.show();
    }
  }
  spr.end();
}

//Texto scroll
void test6() {
  RoJoSprite spr(3); //Sprite color real
  spr.print("/5x7.fon","Hola mundo!",0x006400); //En verde
  int16_t xMax=-(int16_t)spr.xMax();
  for(int16_t x=8;x>xMax;x--) {
    leds.clear();
    leds.drawSprite(&spr,x);
    leds.show();
    delay(100);
  }
  spr.end();
}

//Dimming
void test7() {
  leds.clear();
  RoJoSprite blankSpr(3); //Sprite negro
  blankSpr.setSize(5,8); //Mismo tamaño que una letra
  uint32_t colors[7]={ //Tabla de colores
    blankSpr.getColor(99,0,0), //Rojo
    blankSpr.getColor(0,99,0), //Verde
    blankSpr.getColor(0,0,99), //Azul
    blankSpr.getColor(99,99,0), //Amarillo
    blankSpr.getColor(99,0,99), //Magenta
    blankSpr.getColor(0,99,99), //Cyan
    blankSpr.getColor(99,99,99) //Blanco
  };
  RoJoSprite number(3); //Sprite con el número
  RoJoSprite res(3); //Sprite con el resultado
  for(byte i=1;i<=7;i++) { //Contamos del 1 al 7...
    number.print("/5x7.fon",String(i),colors[i-1]); //Dibujamos el número en el sprite
    for(byte level=0;level<255;level++) { //Recorremos todos los niveles...
      res.fade(&blankSpr,&number,level); //Creamos el sprite resultado para este nivel
      leds.drawSprite(&res,1); //Lo dibujamos en la memoria de vídeo
      leds.show(); //Lo mostramos
      delay(4);
    }
    //Como antes pero ahora pasamos de número a negro
    for(byte level=255;level>0;level--) {
      res.fade(&blankSpr,&number,level);
      leds.drawSprite(&res,1);
      leds.show();
      delay(4);
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
  uint32_t colors[7]={ //Tabla de colores
    spr1.getColor(99,0,0), //Rojo
    spr1.getColor(0,99,0), //Verde
    spr1.getColor(0,0,99), //Azul
    spr1.getColor(99,99,0), //Amarillo
    spr1.getColor(99,0,99), //Magenta
    spr1.getColor(0,99,99), //Cyan
    spr1.getColor(99,99,99) //Blanco
  };
  spr2.print("/5x7.fon","1",colors[0]); //Creamos el primer número en el sprite 2
  leds.drawSprite(&spr2,1); //Lo pasamos a la memoria de vídeo
  leds.show(); //Lo mostramos
  delay(500); //Esperamos 
  for(byte i=2;i<=7;i++) { //Recorremos los números del 2 al 7
    spr1.copy(&spr2); //Copiamos el sprite 2 en el 1
    spr2.print("/5x7.fon",String((int)i),colors[i-1]); //Creamos el número en el sprite 2
    for(uint16_t level=0;level<256;level++) { //hacemos el fade del sprite 1 al 2
      res.fade(&spr1,&spr2,level); //Creamos el sprite con el level correcto
      leds.drawSprite(&res,1); //Dibujamos el resultado
      leds.show(); //Lo mostramos
      delay(4);
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
  spr.setSize(8,8);
  //Creamos la paleta de colores de fuego
  uint32_t fireColor[256];
  for(byte i=0;i<85;i++) {
    fireColor[i]=spr.getColor((byte)(i*3),0,0); //R
    fireColor[i+85]=spr.getColor(255,(byte)(i*3),0); //G
    fireColor[i+170]=spr.getColor(255,255,(byte)(i*3)); //B
  }
  fireColor[255]=spr.getColor(255,255,255);
  byte heat[8][8]; //Creamos array de calor
  byte descT; //Descenso de temperatura
  spr.clear();
  uint32_t maxTime=millis()+10000; //Definimos duración (10s)
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
  test4(); //Load bmp & spr
  test5(); //Glitter
  #if !defined(ARDUINO_AVR_UNO) && !defined(ARDUINO_AVR_NANO)
    test6(); //Texto scroll
    test7(); //Dimming
    test8(); //Fading
    #ifdef ESP32
      test9(); //Fire
    #endif
  #endif
}
