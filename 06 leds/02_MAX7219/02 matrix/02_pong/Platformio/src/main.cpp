/*
  Autor: Ramón Junquera
  Fecha: 20191005
  Tema: Pong
  Objetivo: Desarrollar el pong para una matrix de leds de 8x8
  Material adicional: breadboard, cables, buzzer pasivo, led matrix 8x8 con chip MAX7219,
    2 potenciómetros de 10 Kohmios, pulsador
  Resultado:
    Podemos jugar al pong!
*/

#include <RoJoSwitch.h>
#include <RoJoMAX7219.h>
#include <RoJoMelody.h>

//Pinout
const byte pinBuzzer=8; //Mismo que en buzzer integrado de Maker UNO
const byte pinPad1=A0; //Potenciómetro izquierdo
const byte pinPad2=A1; //Potenciómetro derecho
const byte pinButton=2; //Mismo que en botón integrado de Maker UNO
//Pines de control de la matriz de leds
const byte pinMatrixDIN=10;
const byte pinMatrixCLK=9;
const byte pinMatrixLOAD=11;

//Definición de variables del juego
const byte brightness=8;
int64_t startSpeed=500; //Velocidad inicial en milisegundos
int64_t changeSpeed=10; //Decremento de velocidad en cada rebote con un pad
RoJoSwitch button = RoJoSwitch(pinButton); //Gestión del pulsador
RoJoMAX7219 display; //Gestión de la matriz de leds
RoJoMelody music(pinBuzzer); //Gestión de melodías
byte xBall,yBall; //Coordenadas de la pelota
int dxBall,dyBall; //Dirección de la pelota
//Paso en la que se encuentra el juego
// 0 = Reset
// 1 = Start
// 2 = Play
// 3 = End
byte step=0;
int64_t lastTime; //Valor del último tiempo de juego tomado en milisegundos
byte yPad1,yPad2; //Coordenada vertical de cada pad
int64_t currentSpeed; //Velocidad actual
int64_t currentTime; //Tiempo actual

void setup() {
  Serial.begin(115200);
  display.begin(1,pinMatrixDIN,pinMatrixLOAD,pinMatrixCLK); //Inicialización
  display.setBrightness(brightness); //Fijamos el brillo
  display.autoDraw=false; //Desactivamos el refresco automático
  //Inicializamos el generador de números aleatorios tomando como semilla el valor de una entrada 
  //analógica no utilizada, que dará un valor aleatorio.
  randomSeed(analogRead(A5));
}

//Refresca la posición de los pads y los dibuja
void RefreshPads() {
  //Leemos el valor de los potenciómetros
  //El valor del la entrada analógica oscila entre 0 y 1023
  //Pero debemos obtener un valor final entre 0 y 5
  //Asi que dividimos el valor de la entrada analógica por 171
  //En el caso de que obtengamos 1023, 1023/171=5.98, cuya parte entera es 5
  yPad1=analogRead(pinPad1)/171;
  yPad2=analogRead(pinPad2)/171;
  //Dibujamos los pads
  display.block(0,yPad1,0,yPad1+2,1);
  display.block(7,yPad2,7,yPad2+2,1);
  //Refrescamos la pantalla para que se visualicen los cambios
  display.draw();
}

void loop() {
  switch(step) {
    case 0: //Reset
      //Pulsado el botón de reset
      //Borramos la pantalla
      display.clear();
      //Decidimos la dirección horizontal de la pelota. Valores: -1,1
      dxBall=random(2)*2-1;
      //Decidimos la dirección vertical de la pelota. Valores: -1,0,1
      dyBall=random(3)-1;
      //La pelota siempre comienza en la misma fila
      yBall=3;
      //La pelota comenzará en la columna más lejana al jugador al que se dirige. Valores: 3,4
      xBall=4-(dxBall+1)/2;
      //Dibujamos la pelota
      display.drawPixel(xBall,yBall,1);
      //Fijamos la velocidad actual como la inicial
      currentSpeed=startSpeed;
      //Indicamos cómo será la música de inicio
      music.setSong("CSCSC5-1000",false);
      //Iniciamos la reproducción de la música
      music.start();
      //Mientras la música esté sonando...
      while(music.playing()) {
        //Refrescamos la posición de los pads
        RefreshPads();
        //Reproducimos la música
        music.play();
      };
      //Pasamos el siguiente paso
      step=1;
      break;
    case 1: //Play
      //Repetimos siempre
      while(1) {
        //Si he ha pulsado el botón de reset...
        if(button.pressed()) {
          //El siguiente paso será el inicial
          step=0;
          //Salimos de la rutina
          return;
        }
        //Refrescamos la posición de los pads
        RefreshPads();
        //Reproducimos sonidos
        music.play();

        //Tomamos nota del tiempo actual
        currentTime=millis();
        //Si ya ha pasado el suficiente tiempo como para que la pelota se mueva...
        if(currentTime-lastTime>currentSpeed) {
          //Si la pelota rebota contra las paredes...
          if((yBall==0 && dyBall<0) || (yBall==7 && dyBall>0)) {
            //Cambiamos la dirección vertical de la pelota
            dyBall=-dyBall;
            //Reproducimos sonido de rebote con pared
            music.setSong("C6-100",false);
            music.start();
          }
          //Si choca con alguna pala...
          if(display.v->getPixel(xBall+dxBall,yBall+dyBall)) {
            //Debe rebotar. Cambiamos su dirección horizontal
            dxBall=-dxBall;
            //Si ha chocado con el pad1
            if(xBall==1) {
              //Calculamos el efecto dependiendo del punto de colisión con el pad
              dyBall=yBall+dyBall-yPad1-1;
            }
            //Si ha chocado con el pad2
            if(xBall==6) {
              //Calculamos el efecto dependiendo del punto de colisión con el pad
              dyBall=yBall+dyBall-yPad2-1;
            }
            //La velocidad de la pelota aumenta
            currentSpeed-=changeSpeed;
            //Nunca se puede superar la máxima velocidad
            if(currentSpeed<0) currentSpeed=0;
            //Reproducimos sonido de rebote con pad
            music.setSong("C5-100",false);
            music.start();
          }
          //Si la pelota rebota contra las paredes...
          //Porque podría ser que rebote contra una pared, contra un pad y vuelva a rebotar con una pared
          if((yBall==0 && dyBall<0) || (yBall==7 && dyBall>0)) {
            //Cambiamos la dirección vertical de la pelota
            dyBall=-dyBall;
            //Reproducimos sonido de rebote con pared
            music.setSong("C6-100",false);
            music.start();
          }
          //Borramos la posición actual de la pelota
          display.drawPixel(xBall,yBall,0);
          //Calculamos las nuevas coordenadas de la pelota
          xBall+=dxBall;
          yBall+=dyBall;
          //Dibujamos la pelota
          display.drawPixel(xBall,yBall,1);
          //Si la pelota está dentro de alguna portería...
          if(xBall==0 || xBall==7) {
            //Dibujamos la pantalla
            display.draw();
            //Pasamos al siguiente paso
            step=2;
            //Salimos de la rutina
            return;
          }
          //El último tiempo será el actual
          lastTime=currentTime;
        }
      }
      break;
    case 2: //End
      //Música de fin de juego
      music.setSong("C3",false);
      music.start();
      //Definimos variable para que parpadee la pelota
      boolean visibleBall=true;
      //Al salir de este paso siempre volveremos a comenzar
      step=0;
      //Repetir siempre...
      while(1) {
        //Repetimos el bucle de espera
        for(int c=0;c<9000;c++) {
          //Reproducimos la música
          music.play();
          //Si he ha pulsado el botón de reset...salimos de la rutina
          if(button.pressed()) return;
        }
        //Cambiamos el estado a la visualización de la pelota
        visibleBall=1-visibleBall;
        //Dibujamos (o borramos la pelota)
        display.drawPixel(xBall,yBall,visibleBall);
        //Refrescamos la pantalla
        display.draw();
      }
      break;
  }
  //Si se ha pulsado el botón de reset...pasamos directamente al paso 0=Reset
  if(button.pressed()) step=0;
}


