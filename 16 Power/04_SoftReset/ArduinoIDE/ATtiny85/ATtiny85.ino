/*
  Autor: Ramón Junquera
  Fecha: 20191028
  Tema: Funcionalidades de placa ATtiny85
  Objetivo: Aprender a programar un reseteo por software
  Material adicional: ATtiny85, breadboard, pulsador

  Descripción:
  Definimos una función que se encarga de pasar la ejecución al inicio de la memoria
  donde se encuentra la rutina de inicialización (bootloader).

  Si el programa detecta la pulsación del interruptor, llama a la función de reinicio.
  Para detectar que la placa está reiniciando, hacemos parpadear el led integrado en placa
  contínuamente.
  Así veremos que el parpadeo se detiene cuando se llama a la función.
  
  Resultado:
  Al pulsar el interruptor, la placa se resetea, dejando de hacer parpadear el led mientras reinicia.
*/

//Declaración de constantes globales
const byte pinLed=1; //Pin del led (1=el integrado en placa)
const byte pinSwitch=0; //Pin del pulsador
const int blinkDelay=500; //Tiempo de espera en ms para el parpadeo
unsigned long nextBlink=0; //Tiempo en milisegundos en el que se producirá el siguiente parpadeo

void setup() { 
  //Configuramos el pin del led como salida para poder escribir en él
  pinMode(pinLed, OUTPUT);
  //Configuramos el pin del pulsador como entrada para poder leerlo
  pinMode(pinSwitch,INPUT_PULLUP);
}

//Reinicia el ATtiny85
void reboot(void) {
  //Desactivamos las interrupciones porque pueden interferir en los cambios del prescaler
  noInterrupts();
  //Activamos el cambio de velocidad del prescaler
  CLKPR=B10000000;
  //Fijamos la velocidad del prescaler a la que tiene por defecto
  //Esta velocidad es necesaria por la rutina de reinicio (bootloader)
  CLKPR=0;
  //Definimos una función que se inicia donde indica el puntero
  void (*ptrToFunction)();
  //El puntero indica el inicio de la memoria = vector de inicio del bootloader 
  ptrToFunction=0;
  //Llamamos a la función, pasando la ejecución directamente al inicio del bootloader
  (*ptrToFunction)();
}

void loop() {
  //Tomamos nota del tiempo transcurrido desde el arranque, en ms
  unsigned long now=millis();
  //Si hemos superado el momento de hacer el siguiente parpadeo...
  if(now>nextBlink) {
    //Cambiamos el estado del led
    digitalWrite(pinLed, !digitalRead(pinLed));
    //Calculamos que el siguiente parpadeo debe ocurrir dentro de blickDelay ms a partir de ahora
    nextBlink=now+blinkDelay;
  }
  //Si el interruptor está pulsado...llamamos a la función de reinicio
  if(!digitalRead(pinSwitch)) reboot();  
}
