/*
  Autor: Ramón Junquera
  Fecha: 20181019
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
    La tecla B cancale el modo edición
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
#include "RoJoMAX7219d.h" //Gestión de MAX7219 para displays de dígitos de leds
#include "RoJoKeypad4x4.h" //Gestión de teclado de membrana
#include "RoJoStepper.h" //Gestión de motores paso a paso

//Declaración de constantes globales
//Pines del MAX7219 que gestiona el display
const byte pinDIN_display=15; //A1
const byte pinCS_display=14; //A0
const byte pinCLK_display=16; //A2
const uint32_t editCancelMax=10000; //Tiempo en ms para salida automática del modo de edición
const uint32_t editBlinkMax=200; //Tiempo en ms para parpadeo del modo edición

//Declaración de variables globales
RoJoMAX7219d display;
RoJoKeypad4x4 pad(13,12,11,10,9,8,7,6);
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

void setup()
{
  Serial.begin(115200);

  //Inicialización del motor
  //Utilizaremos la interrupción 1 para el timer que refresca los motores
  RoJoStepper::startTimer(1);
  //Añadimos el motor 0:
  //  motorId=0
  //  pin1=2
  //  pin2=3
  //  pìn3=4
  //  pin4=5
  //  sequenceType=2
  RoJoStepper::addMotor(0,2,3,4,5,2);
  //Activamos el motor 0
  RoJoStepper::enableMotor(0,true);

  //Inicialización del display
  //begin(byte chainedChips,byte pinDIN, byte pinCS, byte pinCLK)
  display.begin(1,pinDIN_display,pinCS_display,pinCLK_display);

  //Mostramos el ángulo actual
  // int64_t number = currentAngle
  // byte startPos = 0
  // byte digits = 4
  // bool leftZeros = false
  display.printInt(currentAngle,0,4,false);
  display.show();
}

void loop()
{
  //Inicialmente no hay cambios en el display
  displayChanged=false;
  
  //Si tenemos nueva pulsación en el teclado...
  if(char key=pad.get())
  {
    //Si estamos en modo edición...
    if(editMode)
    {
      //Se prorroga el tiempo de salida automática del modo de edición
      editCancel=millis()+editCancelMax;
      //Si la tecla pulsada en un dígito...
      if(key>='0' && key<='9')
      {
        //Calculamos el nuevo valor
        int newEditValue=editValue*10+(key-'0')*(editValue<0?-1:1);
        //Si está dentro del rango de valores permitido...
        if(newEditValue>=-999 && newEditValue<=9999)
        {
          //...tomaremos el valor como bueno
          editValue=newEditValue;
          //Si en este momento se está mostrando el valor editado...
          if(editShowing)
          {
            //...lo actualizaremos
            display.printInt(editValue,0,4,false);
            //Se ha cambiado algo en el display
            displayChanged=true;
          }
        }
      }
      else //Si la tecla pulsada no es un dígito...
      {
        switch(key)
        {
          case '*': //Cambio de signo
            //Si se puede cambiar de signo si superar los límites...
            if(editValue<=999)
            {
              //...lo cambiamos
              editValue=-editValue;
              //Si en este momento se está mostrando el valor editado...
              if(editShowing)
              {
                //...lo actualizaremos
                display.printInt(editValue,0,4,false);
                //Se ha cambiado algo en el display
                displayChanged=true;
              }
            }
            break;
          case 'A': //Borrar dígito de unidades
            editValue/=10;
            //Si en este momento se está mostrando el valor editado...
            if(editShowing)
            {
              //...lo actualizaremos
              display.printInt(editValue,0,4,false);
              //Se ha cambiado algo en el display
              displayChanged=true;
            }
            break;
          case 'B': //Cancela el modo edición
            editMode=false;
            //Mostramos el ángulo actual
            display.printInt(currentAngle,0,4,false);
            //Se ha cambiado algo en el display
            displayChanged=true;
            break;
          case 'D': //Se acepta el valor editado
            //Salimos del modo edición
            editMode=false;
            //Guardamos el valor editado como ángulo actual
            currentAngle=editValue;
            //Calculamos el paso correspondiente al ángulo teniendo en cuenta que con
            //la secuencia 2 da un giro en 2048 pasos.
            //Obligatorio un cast de currentAngle porque es int
            //int32_t step=(int32_t)currentAngle*2048/360;
            int32_t step=((int32_t)currentAngle*256)/45; //Simplificado
            //Asignamos la posición al motor 0
            RoJoStepper::go(0,step);
            //Mostramos el ángulo actual
            display.printInt(currentAngle,0,4,false);
            //Se ha cambiado algo en el display
            displayChanged=true;
            break;
        }
      }
    }
    else //Si no estamos en modo edición...
    {
      //Si la tecla es la D (entrar/salir de modo edición)...
      if(key=='D')
      {
        //...entramos en modo edición
        editMode=true;
        //Inicialmente el valor editado es cero
        editValue=0;
        //No se muestra el valor editado en pantalla
        editShowing=false;
        //Definimos el tiempo del próximo blink
        editNextBlink=0; //Ya!. Para que se refresque en el próximo ciclo
        //Definimos el tiempo para a salida automática del modo edición
        editCancel=millis()+editCancelMax;
      }
    }
  }
  //Hemos terminado de procesar las pulsaciones de teclado

  //Si estamos en modo de edición...
  if(editMode)
  {
    //Si hemos superado el tiempo de salida automática del modo edición...
    if(millis()>editCancel)
    {
      //...cancelamos el modo edición
      editMode=false;
      //Mostramos de nuevo el valor del ángulo actual
      display.printInt(currentAngle,0,4,false);
      //Se ha cambiado algo en el display
      displayChanged=true;
    }
    else //Si no se ha superado el tiempo salida automatica de edición...
    {
      //Si hemos superado el tiempo del próximo blink...
      if(millis()>editNextBlink)
      {
        //...tenemos que cambiar su estado (apagado/encendido)
        //Si se está mostrando el valor editado...borramos el display
        if(editShowing) display.clear();
        //Si no se está mostrando el valor editado...mostramos el valor editado
        else display.printInt(editValue,0,4,false);
        //Se ha cambiado algo en el display
        displayChanged=true;
        //Cambiamos el estado
        editShowing=!editShowing;
        //Calculamos el próximo blink
        editNextBlink=millis()+editBlinkMax;
      }
    }
  }

  //Anotamos si el motor 0 está en marcha
  motorStatusCurrent=RoJoStepper::busy(0);
  //Si el motor está en movimiento...
  if(motorStatusCurrent)
  {
    //...activamos el bit del punto decimal para el cuarto dígito directamente en
    //la memoria de vídeo
    display.videoMem[3]|=0b00000001;
  }
  else //Si el motor no está en movimiento...
  {
    //...desactivamos el bit del punto decimal para el cuarto dígito directamente
    //en la memoria de vídeo
    display.videoMem[3]&=0b11111110;
  }

  //Si ha cambiado el estado del motor desde el ciclo anterior...
  if(motorStatusLast!=motorStatusCurrent)
  {
    //...el contenido del display ha cambiado
    displayChanged=true;
    //Guardamos el estado antual en la variable del anterior
    motorStatusLast=motorStatusCurrent;
  }

  //Si se ha modificado el contenido del display...lo mostramos;
  if(displayChanged) display.show();
}

