/*
  Autor: Ramón Junquera
  Fecha: 20201219
  Tema: Buzzer
  Objetivo: Demo de reproducción
  Material: M5Stack Fire, StickC, StickC+, Speaker Hat (opcional)

  Descripción:
  Al pulsar el primero de los botones, vamos a reproducir por el buzzer el contenido
  de un buffer de memoria.
  Previamente llenaremos el buffer con el resultado de una función senoidal.
  Utilizaremos el circuito DAC (Digital Analogic Converter) y lo aplicaremos al pin
  del buzzer.
  El circuito DAC tiene una resolución de 8 bits. Así que almacenaremos cada muestra
  en un byte.
  Utilizaremos la librería RoJoI2Sa para simplificar la reproducción y no tener
  que controlar los timers nosotros mismos.

  Nota:
  En el modelo StickC+, el buzzer interno está conectado al pin 2, pero los dos circutos
  DAC están asociados a los pines 25 y 26. Por lo tanto, NO podemos utilizar el buzzer
  interno para reproducir ondas. Es la razón por la que utilizamos un buzzer pasivo
  externo conectado al pin 26 o el hat de speaker. Esta solución es válida para los
  modelos StickC y StickC+.

  Nota:
  En el modelo StickC y StickC+ se puede acoplar el hat de speaker.
  Este detalle se indicará en el archivo Platformio.ini
*/

#include <Arduino.h>
#include <RoJoSwitch.h> //Gestión de interruptores
#include <RoJoI2Sa.h> //Gestión de dispositivos de audio analógicos
#ifdef SPEAKERHAT
  #include <Wire.h> //Para gestión de chip de alimentación AXP192
#endif

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
  const byte pinBuzzer=26; //Pin del buzzer o del speaker hat
  RoJoSwitch buttonA(37); //Botón M5
#endif

//Variables globales
byte sampleBuffer[sampleSize];
float period_s=1.0/(float)freq; //Periodo en segundos

void setup() {
  #ifdef SPEAKERHAT
    //Activamos la alimentación del speaker hat
    Wire.begin(21,22);
    Wire.beginTransmission(0x34);
    Wire.write(0x12);  
    Wire.write(0x4D); //Enable DC-DC1, OLED_VDD, 5B V_EXT
    Wire.endTransmission();
  #endif

  //Gestionaremos el speaker a través del driver RoJoI2Sa para simplificar la
  //programación y no tener que trabajar con timers.
  //Indicamos timer, pin analógico de salida y frecuencia.
  RoJoI2Sa_begin(0,pinBuzzer,freq);

  Serial.begin(115200); delay(2000);
  Serial.printf("Periodo en segundos: %f\n",period_s);
  //Llenamos el buffer con una onda senoidal
  for(uint32_t sample=0;sample<sampleSize;sample++) {
    sampleBuffer[sample]=127+127.0*sin((double)sample/8);
  }
  Serial.println("Ready!");
}

void loop() {
  if (buttonA.pressed()) {
    Serial.print("Playing...");
    RoJoI2Sa_write(0,(byte*)sampleBuffer,sampleSize); //Reproducimos buffer en speaker
    Serial.println("Stop");
  }
  delay(10);
}
