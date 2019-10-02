/*
  Pendiente:
    Modificaciones para ESP32. M5Stick Watch
    Necesario incluir librerías nuevas


  Autor: Ramón Junquera
  Fecha: 20191001
  Tema: Elementos analógicos
  Objetivo: Aprender cómo funcionan las escrituras analógicas

  Descripción:
    Haremos variar la intensidad de un led conectado a un pin con capacidades PWM
    progresivamente desde apagado hasta el máximo y después a la inversa.
  Precisión de PWM:
    Las placas Arduino pueden escribir valores analógicos (PWM) con una precisión
    de 8 bits (2^8=256), pero las ESP tiene precisión de 10 bits (2^10=1024).
    Por esta razón definimos distinto máximo para el nivel de PWM, dependiendo
    de la placa.
  Placas UNO/Nano:
    No todos los pines de las placas Arduino tienen la posibilidad de escritura
    analógica (PWM). En Arduino UNO o Nano, el pin 13 asociado al led integrado
    no la tiene. Por eso utilizamos otro pin con un led externo.
  Placa MakerUno:
    Aunque pertenece a la familia de placas Arduino, ya tiene integrado un led para
    cada pin, por lo tanto no es necesario ningún montaje adicional.
  Placa Mega:
    En Arduino Mega, el pin 13 asociado al led integrado SI tiene capacidades PWM,
    pero para guardar compatibilidad con el resto de placas Arduino, utilizaremos
    un led externo.
  Placas ESP8266:
    El juego de instrucciones ya incluyen la función analogWrite
  Placas ESP32:
    No incluyen la instrucción analogWrite. Tenemos dos librerías que implementan
    esta función pero con distintos métodos:
      RoJoAnalogWriteESP32L.h -> método ledc
      RoJoAnalogWriteESP32S.h -> método SigmaDelta
    En este ejemplo utilizaremos el método ledc por las siguientes razones:
      - Permite asignar PWM a 16 pines, contra 8 con SigmaDelta
      - Permite compartir los canales con otras funciones

  Resultado:
  El led se enciende y apaga progresivamente
*/

#include <Arduino.h>

//Pinout
#ifdef ARDUINO_ARCH_AVR //Si es una placa Arduino
  const byte pinLed=3;
  const uint16_t maxLevel=255; //Máximo nivel de PWM
#elif defined(ESP8266) //Si es un ESP8266
  const byte pinLed=LED_BUILTIN;
  const uint16_t maxLevel=1023;
#elif defined(ESP32) //Si es un ESP32
  #include <RoJoAnalogWriteESP32L.h>
  const byte pinLed=LED_BUILTIN;
  const uint16_t maxLevel=255;
#endif

void setup() {
  //Configuramos el pin del led como salida para poder escribir en él
  pinMode(pinLed,OUTPUT);
}

//Declaración de variables globales
int16_t level=0; //Nivel PWM
int16_t delta=1; //Dirección

void loop() {
  //Escribimos el nivel PWM en el pin del led
  analogWrite(pinLed,level);
  //Calculamos el valor del nivel del próximo ciclo
  level+=delta;
  //Si hemos llegado a alguno de los extremos...cambiamos de dirección
  if(level==0 || level==maxLevel) delta=-delta;
  //Esperamos un momento
  delay(2);
}
