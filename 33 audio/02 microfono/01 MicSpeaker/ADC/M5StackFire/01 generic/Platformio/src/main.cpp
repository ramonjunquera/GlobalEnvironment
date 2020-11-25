/*
  Autor: Ramón Junquera
  Fecha: 20201117
  Tema: Micrófono
  Objetivo: Demo de grabación/reproducción
  Material: M5Stack Fire

  Descripción:
  El circuito ADC (Analogic Digital Conversion) del ESP32 es capaz de convertir
  valores analógicos en digitales con una resolución de 12bits.
  Utilizamos el comando analogRead, que devuelve el valor en un uint16_t.
  Los 4 bits superiores siempre son ceros.
  En cambio el circuito DAC (Digital Anatogic Converter) sólo tiene una resolución
  de 8bits.
  Puesto que en este ejemplo queremos tango grabar como reproducir, utilzaremos
  datos de 8bits (bytes) para almacenar las muestras. Es cierto que estamos perdiendo
  calidad de audio, pero tampoco podríamos reproducirla.
  También ahorramos memoria por cada muestra.

  La adquisición o reproducción de muestras no es un problema. La clave está en mantener
  la frecuencia en ambos procesos.
  Para ello nos vemos obligados a utilizar interrupciones (timers) que periódicamente
  se encarguen de ello.
  Si tomamos como ejemplo una frecuencia de 22KHz = 22000Hz, el periodo es de
  p = 1/22000 s = 0.00004545 s = 0.04545 ms = 45.45 microsegundos.
  La resolución del timer utilizado debe contemplar estos periodos.
  Los timers por software como Ticker (basado en los timers de FreeRTOS), no alcanzan
  pariodos tan bajos, porque dependen de la carga de la CPU.
  Estamos obligados a utilizar timers controlados por hardware.
  En este ejemplo utilizarmos la librería RoJoTimerESP32 para facilitarlo.
*/

#include <Arduino.h>
#include <RoJoILI9341.h> //Librería de gestión del display ILI9341
#include <RoJoSprite.h> //Gestión de sprites color
#include <RoJoSwitch.h> //Gestión de interruptores
#include <RoJoTimerESP32.h> //Gestión de timers

//Contantes globales
const byte pinMic=34;
const byte pinBuzzer=25;
const uint32_t bufferSize=100000; //Número máximo de muestras
const uint32_t freq=10000; //Frecuencia de muestreo

//Creamos objeto de gestión
RoJoILI9341 display;
RoJoSwitch buttonA(39),buttonB(38); //Botones en M5Stack Fire

//Variables globales
byte *sampleBuffer=nullptr; //Puntero a buffer
uint32_t sampleSize=0; //Número de muestras de la grabación
uint32_t samplePos; //Posición actual del buffer
RoJoTimerESP32 myTimer(0); //Usaremos el timer 0
float period_s=1.0/(float)freq; //Periodo en segundos
uint16_t lastX,currentX; //Coordenada horizontal de la barra de progreso

//Detiene la reproducción
void stopPlaying() {
  myTimer.detach();
  dacWrite(pinBuzzer,0); //Silencio
}

//Detiene la grabación
void stopRecording() {
  stopPlaying();
  sampleSize=samplePos;
  display.block(2,102,317,148,{0,0,0}); //Borra la barra de progreso
}

//Función de interrupción para grabación
void recordingF() {
  //La muestra tiene una resolución de 12bits. La desplazamos 4 posiciones, perdiendo
  //los 4 bits más bajos. La guardamos en la posición del buffer que corresponde.
  sampleBuffer[samplePos++]=analogRead(pinMic)>>4;
  //Si hemos llegado al final de la memoria...paramos de grabar
  if(samplePos==bufferSize) stopRecording(); 
}

//Función de interrupción para reproducción
void playingF() {
  dacWrite(pinBuzzer,sampleBuffer[samplePos++]);
  //Si hemos llegado al final de las muestras...paramos de reproducir
  if(samplePos==sampleSize) stopPlaying(); 
}

void setup() {
  Serial.begin(115200); //Para DEBUG
  //Inicializamos el display para M5Stack Fire
  //RES=33,DC=27,CS=14,BackLight=32
  display.begin(33,27,14,32);
  display.rotation(1); //Botonera abajo
  display.disk(65,225,10,{255,0,0}); //Label REC
  display.triangleFill(150,215,170,225,150,235,{0,255,0}); //Label PLAY
  display.printOver("/10x22.fon","Demo grabacion/reproduccion",{255,255,255},25);
  display.rect(0,100,319,150,{255,255,255}); //Marco de la barra de progreso

  sampleBuffer=new byte[bufferSize];
  if(sampleBuffer==nullptr) Serial.println("Error reservando memoria");
  else Serial.println("Reserva de memoria OK");
  Serial.printf("Periodo en segundos: %f\n",period_s);
}

void loop() {
  if(myTimer.active()) { //Si está grabando...
    if(buttonA.released()) stopRecording(); //Si se suelta el botón A...paramos de grabar
    else { //Seguimos grabando...
      //Dibujamos la barra de progreso
      currentX=2+(316*samplePos)/bufferSize; //Calculamos la X de la posición actual de la barra
      if(currentX>lastX) { //Si ha cambiado respecto a la última vez...
        display.block(lastX+1,102,currentX,148,{255,0,0}); //Dibujamos la diferencia
        lastX=currentX; //La proxima vez comenzaremos desde aquí
      }
    }
  }
  else { //No está grabando...
    if (buttonA.pressed()) { //Si se ha pulsado el botón de grabar...
      samplePos=sampleSize=0; //No tenemos nada grabado
      myTimer.attach(period_s,recordingF);
      lastX=1;
    }
    //Si se ha pulsado el botón de reproducir y hay algo para reproducir...
    if (buttonB.pressed() && sampleSize>0) {
      samplePos=0; //Reproducimos desde el principio
      myTimer.attach(period_s,playingF);
      lastX=1;
      while(myTimer.active()) { //Mientras no haya terminado de reproducir...
        currentX=2+(316*samplePos)/sampleSize; //Calculamos la X de la posición actual de la barra
        if(currentX>lastX) { //Si ha cambiado respecto a la última vez...
          display.block(lastX+1,102,currentX,148,{0,255,0}); //Dibujamos la diferencia
          lastX=currentX; //La proxima vez comenzaremos desde aquí
        }
        delay(10);
      }
      display.block(2,102,317,148,{0,0,0}); //Borra la barra de progreso
    }
  }
  delay(10);
}
