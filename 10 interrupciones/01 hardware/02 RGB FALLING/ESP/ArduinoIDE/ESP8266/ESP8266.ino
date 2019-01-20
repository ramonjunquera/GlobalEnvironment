/*
  Autor: Ramón Junquera
  
  Tema: WeMosD1 R2 & Mini
  
  Objetivo: Gestión de interrupciones por hardware
  
  Material adicional: WeMos D1 o Mini, breadboard, 3 pulsadores, led RGB

  Descripción:
  Todos los pines de las placas WeMos tienen posibilidad de generar una interrupción por
  hardware, excepto el pin D0.
  El número de la interrupción coincide con el número de pin.

  En el siguiente circuito utilizamos 3 interruptores, conectados a pines digitales
  de entrada, con resistencias internas de pullpup, que generan una interrupción
  por cambio de estado al pulsar (de HIGH a LOW = FALLING), que llaman a distintas
  funciones que cambian el estado de cada uno de los leds.
  
  No se tiene en cuenta el efecto rebote!

  WeMos    Arduino integrated hard other
  pin      pin     led        int  functions
  -------- ------- ---------- ---- ---------
  D0       16      -           -
  D1        5      -           5   SCL
  D2        4      -           4   SDA
  D3        0      -           0
  D4        2      antenna     2
  D5       14      SCK        14    SCK
  D6       12      -          12   MISO
  D7       13      -          13   MOSI
  D8       15      -          15   SS
  TX        1      -           1   TX
  RX        3      -           3   RX
  A0       A0      -           -

  Resultado:
  Cada pulsador enciende un color
*/

const byte pinSwitchR = D1;
const byte pinSwitchG = D2;
const byte pinSwitchB = D3;
const byte pinLedR = D6;
const byte pinLedG = D0;
const byte pinLedB = D5;

void setup()
{ 
  //Configuramos los de los interruptores como entrada con resistencias de pullup
  pinMode(pinSwitchR,INPUT_PULLUP); //R en pin D1
  pinMode(pinSwitchG,INPUT_PULLUP); //G en pin D2
  pinMode(pinSwitchB,INPUT_PULLUP); //B en pin D3
  
  //Configuramos los pines del led RGB como salidas
  pinMode(pinLedR, OUTPUT); //R en pin D6
  pinMode(pinLedG, OUTPUT); //G en pin D0
  pinMode(pinLedB, OUTPUT); //B en pin D5

  //Apagamos todos los leds
  digitalWrite(pinLedR, HIGH);
  digitalWrite(pinLedG, HIGH);
  digitalWrite(pinLedB, HIGH);
 
  //Configuramos las funciones de interrupción
  attachInterrupt(pinSwitchR,IntSwitchR,FALLING);
  attachInterrupt(pinSwitchG,IntSwitchG,FALLING);
  attachInterrupt(pinSwitchB,IntSwitchB,FALLING);
}

void loop()
{
  //No tenemos nada especial que hacer aquí. Todo se gestiona por interrupciones
}

void IntSwitchR()
{
  //Se ha pulsado el interruptor R

  //Cambiamos el estado del led R
  digitalWrite(pinLedR,!digitalRead(pinLedR));
}

void IntSwitchG()
{
  //Se ha pulsado el interruptor G

  //Cambiamos el estado del led G
  digitalWrite(pinLedG,!digitalRead(pinLedG));
}

void IntSwitchB()
{
  //Se ha pulsado el interruptor B

  //Cambiamos el estado del led B
  digitalWrite(pinLedB,!digitalRead(pinLedB));
}

