/*
  Autor: Ramón Junquera
  Fecha: 20200903
  Tema: Multitasking / FreeRTOS
  Objetivo: Demostración de uso de semáforo de contador con interrupciones
  Material: M5Stack Atom Lite
  Descripción:
    Cada vez que se pulse el botón, el led decrementará su intensidad hasta
    apagarse y la volverá a incrementar en otro color distinto.
    Para la detección de la pulsación utilizaremos interruciones de hardware
    para evitar hacer polling.
    Se crea un semáforo contador con un valor máximo configurable.
    Cuando se produce un cambio en el estado del pin del pulsador, se genera
    una interrupción.
    La función de interrupción intenta aumentar el contador del semáforo.
    Por otra parte definimos una tarea que atenderá el semáforo.
    Cuando puede decrementar su valor lanzará la rutina de fading.
    Utilizaremos la librería RoJoNeoPixelAtomLite para gestionar el led por ser
    más compacta que la genérica de RoJoNeoPixel.
    Utilizaremos la librería RoJoSwitch para evitar el efecto rebote.
  Comentarios:
  - Puesto que la secuencia de fading dura mucho, no podemos meterla en la
    función de interrupción.
  - El sistema NO consume ningún recurso mientras no se detecte una pulsación.
  - Podemos pulsar varias veces seguidas el botón y comprobaremos que es capaz
    de recordar las pulsaciones hasta el máximo indicado.
*/

#include <Arduino.h>
#include <RoJoNeoPixelAtomLite.h> //Gestión de led NeoPixel en M5Stack Atom Lite
#include <RoJoSwitch.h> //Gestión de interruptor

RoJoNeoPixelAtomLite led; //Objeto de gestión de leds NeoPixel
RoJoSwitch button(39); //Objeto de gestión de pulsador. Pin 39 en Atom
byte maxClicksMem=3; //Número máximo de pulsaciones memorizadas
SemaphoreHandle_t sema=xSemaphoreCreateCounting(maxClicksMem,0); //Semáforo contador
byte colorIndex=0; //Inicialmente comenzaremos por el primer color
RoJoColor ledColors[7]={ //Array de colores
   {255,0,0} //RoJo
  ,{0,255,0} //Verde
  ,{0,0,255} //Azul
  ,{255,255,0} //Amarillo
  ,{255,0,255} //Magenta
  ,{0,255,255} //Cyan
  ,{255,255,255} //Blanco
};

//Función a la que se llama cuando cambia el estado del interruptor
void intSwitch() {
  //Si el interruptor ha sido pulsado...incrementamos el valor del semáforo
  //Nota: Si el semáforo ya tiene el valor máximo, no se tiene en cuenta el give
  if(button.pressed()) xSemaphoreGive(sema);
}

//Función para la tarea
void taskFunc(void *param) {
  for(;;) { //Bucle infinito
    xSemaphoreTake(sema,portMAX_DELAY); //Esperamos a decrementar el valor del semáforo
    //Decrementando intensidad
    RoJoColor colorOri=ledColors[colorIndex]; //Color actual
    RoJoColor colorDim={0,0,0}; //Color con menos intensidad
    for(byte i=255;i>0;i-=5) {
      for(byte channel=0;channel<3;channel++) { //Calculamos los 3 canales
        if(colorOri.channels[channel]) colorDim.channels[channel]=i;
      }
      led.draw(colorDim); //Aplicamos color resultante
      delay(20);
    }
    if(++colorIndex>6) colorIndex=0; //Pasamos al siguiente color
    //Incrementando intensidad
    colorOri=ledColors[colorIndex];
    colorDim={0,0,0};
    for(byte i=0;i<255;i+=5) {
      for(byte channel=0;channel<3;channel++) {
        if(colorOri.channels[channel]) colorDim.channels[channel]=i;
      } 
      led.draw(colorDim);
      delay(20);
    }
  }
}

void setup() {
  led.begin(); //Inicialización de led NeoPixel en M5Stack Atom Lite
  led.draw(ledColors[0]); //Comenzamos con el primer color
  attachInterrupt(39,intSwitch,CHANGE); //Definimos la interrupción para el pin 39 (pulsador)
  xTaskCreatePinnedToCore(taskFunc,"",2000,NULL,1,NULL,0); //Creamos tarea en CPU0
  vTaskDelete(NULL); //Eliminamos la tarea del programa principal
}

void loop() {
  //Nunca llegará hasta este punto, porque se ha eliminado la tarea de main prog
}