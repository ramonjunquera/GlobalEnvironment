/*
  Autor: Ramón Junquera
  Fecha: 20201121
  Tema: Buzzer
  Objetivo: Demo de reproducción
  Material: M5Stack Fire, StickC, StickC+

  Descripción:
  Al pulsar el primero de los botones, vamos a reproducir por el buzzer el contenido
  de un buffer de memoria.
  Previamente llenaremos el buffer con el resultado de una función senoidal.
  Utilizaremos el circuito DAC (Digital Analogic Converter) y lo aplicaremos al pin
  del buzzer.
  El circuito DAC tiene una resolución de 8 bits. Así que almacenaremos cada muestra
  en un byte.
  Para reproducir las muestras a una frecuencia determinada utilizaremos un timer
  por hardware.

  Nota:
  En el modelo StickC+, el buzzer interno está conectado al pin 2, pero los dos circutos
  DAC están asociados a los pines 25 y 26. Por lo tanto, NO podemos utilizar el buzzer
  interno para reproducir ondas. Es la razón por la que utilizamos un buzzer pasivo
  externo conectado al pin 26. Esta solución es válida para los podelos StickC y StickC+.
*/

#include <Arduino.h>
#include <RoJoSwitch.h> //Gestión de interruptores
#include <RoJoTimerESP32.h> //Gestión de timers

//Contantes globales
const uint32_t sampleSize=20000; //Número máximo de muestras
const uint32_t freq=10000; //Frecuencia de muestreo

#ifdef ARDUINO_M5STACK_FIRE
  const byte pinBuzzer=25;
  RoJoSwitch buttonA(39); //Primer botón
#elif ARDUINO_M5Stick_Watch
  const byte pinBuzzer=25;
  RoJoSwitch buttonA(35);
#elif ARDUINO_M5Stick_C
  const byte pinBuzzer=26;
  RoJoSwitch buttonA(37); //Botón M5
#endif

//Variables globales
byte *sampleBuffer=nullptr; //Puntero a buffer
RoJoTimerESP32 myTimer(0); //Usaremos el timer 0
float period_s=1.0/(float)freq; //Periodo en segundos
uint32_t samplePos; //Posición de la muestra reproducida

//Función de interrupción para reproducción
void playingF() {
  dacWrite(pinBuzzer,sampleBuffer[samplePos++]);
  if(samplePos==sampleSize) { //Si hemos llegado al final de las muestras..
    myTimer.detach(); //Detenemos el timer
    dacWrite(pinBuzzer,0); //Silencio
  }
}

void setup() {
  Serial.begin(115200); delay(2000);
  
  sampleBuffer=new byte[sampleSize];
  if(sampleBuffer==nullptr) Serial.println("Error reservando memoria");
  else Serial.println("Reserva de memoria OK");
  Serial.printf("Periodo en segundos: %f\n",period_s);

  //Llenamos el buffer con una onda senoidal
  for(uint32_t sample=0;sample<sampleSize;sample++) {
    sampleBuffer[sample]=127+127.0*sin((double)sample/8);
  }
  Serial.println("Ready!");
}

void loop() {
  if (buttonA.pressed()) {
    Serial.print("Playing..");
    samplePos=0; //Reproducimos desde el principio
    myTimer.attach(period_s,playingF); //Activamos timer
    while(myTimer.active()) delay(100); //Esperamos hasta que finalice la reproducción
    Serial.println("Stop");
  }
  delay(10);
}
