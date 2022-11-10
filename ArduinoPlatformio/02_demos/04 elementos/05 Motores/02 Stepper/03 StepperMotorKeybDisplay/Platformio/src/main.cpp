/*
  Autor: Ramón Junquera
  Fecha: 20221110
  Tema: Motores paso a paso
  Objetivo: Proyecto con varios elementos
  Material: placa Arduino, cables, motor paso a paso (28BYJ-48), display de
    4 dígitos led, MAX7219, keyboard pad 4x4.
  Descripción:
    Se trata de introducir por teclado el ángulo de giro de un motor paso a paso, con
    la ayuda de un display de 4 dígitos de leds.
    En todo momento se muestra en el display el ángulo al que se dirige el motor.
    Tenemos un teclado numérico de membrana.
    Pulsando la tecla D entramos en modo edición en el que parpadea el display.
    Se puede escribir un nuevo valor con las teclas numéricas.
    La tecla A sirve para borrar las unidades.
    La tecla * sirve para cambiar el signo.
    La tecla B cancela el modo edición
    La tecla D confirma el valor, sale del modo edición y el motor comienza a moverse
    hasta ese ángulo.
    Si no se valida durante 10 segundos, deja de parpadear y vuelve al estado de
    mostrar el ángulo fijado.
    Puesto que tenemos 4 dígitos y el valor podría ser negativo, fijaremos los límites
    en -999 y 9999 grados.
    El punto decimal del cuarto dígito está encendido mientras el motor se encuentra en
    movimiento.
    Se puede introducir y validar un nuevo ángulo mientras el motor está en movimiento.
    Puesto que es necesaria una fuente de alimentación externa para el motor, se
    aprovecha también para la placa, convirtiendo el sistema en autónomo.
  YouTube: https://youtu.be/RwJnd247Y18
*/

#include <Arduino.h>
#include <RoJoMAX7219d.h> //Gestión de MAX7219 para displays de dígitos de leds
#include <RoJoPad4x4.h> //Gestión de teclado de membrana
#include <RoJoStepper.h> //Gestión de motores paso a paso

//Declaración de constantes globales
//Pines del MAX7219 que gestiona el display
const byte pinDIN_display=15; //A1
const byte pinCS_display=14; //A0
const byte pinCLK_display=16; //A2
const uint32_t editCancelMax=10000; //Tiempo en ms para salida automática del modo de edición
const uint32_t editBlinkMax=200; //Tiempo en ms para parpadeo del modo edición
const byte pinoutStepper[]={5,4,3,2}; //Pinout del motor (IN4,IN3,IN2,IN1)

//Declaración de variables globales
RoJoMAX7219d display;
RoJoPad4x4 pad(13,12,11,10,9,8,7,6);
RoJoStepper stepper;
//No necesitamos crear un objeto de gestión de RoJoStepper porque es una clase estática
bool editMode=false; //Estamos en modo de edición?
int editValue; //Valor editado
uint32_t editNextBlink; //Próximo blink en ms
bool editShowing; //Se está mostrando el valor editado en pantalla?
uint32_t editCancel; //Tiempo de salida automática de modo edición
int currentAngle=0; //Ángulo actual. Inicialmente cero
bool displayChanged; //Se ha modificado el contenido del display?
bool motorStatusCurrent; //El motor está en marcha actualmente?
bool motorStatusLast=false; //El motor estaba en marcha en el ciclo anterior?

void setup() {
  Serial.begin(115200);
  stepper.begin(1,970); //Init motor 1 con usDelay=970
  stepper.setCfg(0,pinoutStepper,1); //Config de motor 0,pinout,seqType=1=MaxTorque
  stepper.enable(0); //Activamos el motor 0

  //Inicialización del display
  //begin(byte chainedChips,byte pinDIN, byte pinCS, byte pinCLK)
  display.begin(1,pinDIN_display,pinCS_display,pinCLK_display);

  //Mostramos el ángulo actual
  // int64_t number = currentAngle
  // byte startPos = 0
  // byte digits = 4
  // bool leftZeros = false
  display.printInt(currentAngle,0,4,false);
  display.draw();
}

void loop() {
  displayChanged=false; //Inicialmente no hay cambios en el display
  if(char key=pad.get()) { //Si tenemos nueva pulsación en el teclado...
    if(editMode) { //Si estamos en modo edición...
      //Se prorroga el tiempo de salida automática del modo de edición
      editCancel=millis()+editCancelMax;
      if(key>='0' && key<='9') { //Si la tecla pulsada en un dígito...
        int newEditValue=editValue*10+(key-'0')*(editValue<0?-1:1); //Calculamos el nuevo valor
        if(newEditValue>=-999 && newEditValue<=9999) { //Si está dentro del rango de valores permitido...
          editValue=newEditValue; //...tomaremos el valor como bueno
          if(editShowing) { //Si en este momento se está mostrando el valor editado...
            display.printInt(editValue,0,4,false); //...lo actualizaremos
            displayChanged=true; //Se ha cambiado algo en el display
          }
        }
      } else { //Si la tecla pulsada no es un dígito...
        switch(key) {
          case '*': //Cambio de signo
            if(editValue<=999) { //Si se puede cambiar de signo si superar los límites...
              editValue=-editValue; //...lo cambiamos
              if(editShowing) { //Si en este momento se está mostrando el valor editado...
                display.printInt(editValue,0,4,false); //...lo actualizaremos
                displayChanged=true; //Se ha cambiado algo en el display
              }
            }
            break;
          case 'A': //Borrar dígito de unidades
            editValue/=10;
            if(editShowing) { //Si en este momento se está mostrando el valor editado...
              display.printInt(editValue,0,4,false); //...lo actualizaremos
              displayChanged=true; //Se ha cambiado algo en el display
            }
            break;
          case 'B': //Cancela el modo edición
            editMode=false;
            display.printInt(currentAngle,0,4,false); //Mostramos el ángulo actual
            displayChanged=true; //Se ha cambiado algo en el display
            break;
          case 'D': //Se acepta el valor editado
            editMode=false; //Salimos del modo edición
            currentAngle=editValue; //Guardamos el valor editado como ángulo actual
            //Calculamos el paso correspondiente al ángulo teniendo en cuenta que con
            //la secuencia 1 da un giro en 2048 pasos.
            //Obligatorio un cast de currentAngle porque es int
            //int32_t step=(int32_t)currentAngle*2048/360;
            int32_t step=((int32_t)currentAngle*256)/45; //Simplificado
            stepper.move(0,step,false); //Asignamos la posición al motor 0 y no esperamos a que termine
            display.printInt(currentAngle,0,4,false); //Mostramos el ángulo actual
            displayChanged=true; //Se ha cambiado algo en el display
            break;
        }
      }
    } else { //Si no estamos en modo edición...
      if(key=='D') { //Si la tecla es la D (entrar/salir de modo edición)...
        editMode=true; //...entramos en modo edición
        editValue=0; //Inicialmente el valor editado es cero
        editShowing=false; //No se muestra el valor editado en pantalla
        editNextBlink=0; //Definimos el tiempo del próximo blink. Ya!. Para que se refresque en el próximo ciclo
        editCancel=millis()+editCancelMax; //Definimos el tiempo para a salida automática del modo edición
      }
    }
  }
  //Hemos terminado de procesar las pulsaciones de teclado

  if(editMode) { //Si estamos en modo de edición...
    if(millis()>editCancel) { //Si hemos superado el tiempo de salida automática del modo edición...
      editMode=false; //...cancelamos el modo edición
      display.printInt(currentAngle,0,4,false); //Mostramos de nuevo el valor del ángulo actual
      displayChanged=true; //Se ha cambiado algo en el display
    } else { //Si no se ha superado el tiempo salida automatica de edición...
      if(millis()>editNextBlink) { //Si hemos superado el tiempo del próximo blink...
        //...tenemos que cambiar su estado (apagado/encendido)
        //Si se está mostrando el valor editado...borramos el display
        if(editShowing) display.clear();
        //Si no se está mostrando el valor editado...mostramos el valor editado
        else display.printInt(editValue,0,4,false);
        displayChanged=true; //Se ha cambiado algo en el display
        editShowing=!editShowing; //Cambiamos el estado
        editNextBlink=millis()+editBlinkMax; //Calculamos el próximo blink
      }
    }
  }

  //Anotamos si el motor 0 está en marcha
  motorStatusCurrent=stepper.isMoving(0);
  if(motorStatusCurrent) { //Si el motor está en movimiento...
    //...activamos el bit del punto decimal para el cuarto dígito directamente en
    //la memoria de vídeo
    display.videoMem[3]|=0b00000001;
  } else { //Si el motor no está en movimiento...
    //...desactivamos el bit del punto decimal para el cuarto dígito directamente
    //en la memoria de vídeo
    display.videoMem[3]&=0b11111110;
  }

  
  if(motorStatusLast!=motorStatusCurrent) { //Si ha cambiado el estado del motor desde el ciclo anterior...
    displayChanged=true; //...el contenido del display ha cambiado
    motorStatusLast=motorStatusCurrent; //Guardamos el estado antual en la variable del anterior
  }

  if(displayChanged) display.draw(); //Si se ha modificado el contenido del display...lo mostramos;
}

