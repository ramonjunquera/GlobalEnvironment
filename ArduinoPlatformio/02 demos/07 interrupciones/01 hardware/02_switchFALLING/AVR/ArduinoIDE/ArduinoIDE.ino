/*
  Autor: Ramón Junquera
  Fecha: 20191027
  Tema: Gestión de interrupciones de hardware
  Objetivo: Descripción de configuración de interrupciones de ATtiny85 por IDE
  Material adicional: ATtiny85, breadboard, interruptor de pulsador

  Descripción:
  El ATtiny85 sólo tiene un pin asociado a interrupciones de hardware cuando se
  utilizan los comandos habituales que proporciona el IDE de Arduino.
  El funcionamiento es idéntico al de una placa UNO de Arduino.

  La descripción de los pines es la siguiente:
    Pin 0 → I2C SDA, PWM
    Pin 1 → PWM, LED integrado
    Pin 2 → I2C SCK, Analog(A1), INT0
    Pin 3 → Analog(A3)
    Pin 4 → PWM, Analog(A2)
    Pin 5 → RESET

  Por lo tanto sólo tenemos el pin 2 asociado a la interrupción número 0 (INT0).

  En este ejemplo gestionamos el cambio de estado del led integrado en placa
  a través de interrupciones.
  En el pin asociado a la interrupción (2) conectamos un pulsador.

  Creamos la función a la que se llama cuando cae el estado (pasa de HIGH a LOW),
  que es cuando se pulsa.
  En ese momento controlamos el tiempo para evitar el efecto rebote y cambiamos
  el estado del led.

  No tenemos nada que escribir en el bucle principal porque todo está gestionado
  por interrupciones.
  
  Resultado:
  Cuando pulsamos el interruptor, se cambia el estado del led integrado.
*/


//Declaración de variables globales
const byte pinLed = 1; //pin del led
const byte pinSwitch = 2; //pin al que está conectado el interruptor
const byte intSwitch = 0; //número de interrupción asociada al pin del interruptor
bool ledStatus=false; //estado del led, inicialmente apagado
const long int delayTime=400; //tiempo de demora en milisegundos antes de comprobar una nueva interrupción
volatile long int nextIntTime=0; //tiempo a partir del cual se puede tener en cuenta la próxima interrupción

void setup() {
  //Configuramos el pin del led para que sea de salida
  pinMode(pinLed,OUTPUT);
  //Comenzamos con el led apagado
  digitalWrite(pinLed,LOW);
  //Activamos las resistencias de pull-up para el pin de interruptor
  //Por defecto el pin del interruptor estará en HIGH y cuando se pulse en LOW
  pinMode(pinSwitch,INPUT_PULLUP);
  
  //Definimos la interrupción
  //Se llamará a la función IntSwitch
  //Ocurrirá cuando se pulse el interruptor (pase de HIGH a LOW)
  attachInterrupt(intSwitch,IntSwitch,FALLING);
}

void loop() {
  //No tenemos nada que hacer en el bucle principal
  //Todo se gestiona por interrupciones
}

void IntSwitch() {
  //Función a la que se llama cuando se pulsa el interruptor

  //Tomamos nota de la hora actual
  volatile long int now=millis();
  //Si la hora actual es superior a mínimo tiempo que debe transcurrir hasta el próximo evento...
  if(now>nextIntTime) {
    //Podemos tomar esta interrupción como buena!
    //Calculamos la hora mínima en la que debe ocurrir el próximo evento
    nextIntTime=now+delayTime;
    //El estado actual del led será el contrario del que tenía hasta ahora
    ledStatus=!ledStatus;
    //Aplicamos al led el estado actual
    digitalWrite(pinLed,ledStatus);
  }
}
