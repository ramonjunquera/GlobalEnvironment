/*
  Autor: Ramón Junquera
  Tema: WeMosD1 R2 & Mini
  Objetivo: Gestión de interrupciones por hardware
  Material: WeMos D1 o Mini, breadboard, 3 pulsadores, led RGB

  Descripción:
  Basándonos en el ejemplo anterior, gestionaremos el efecto rebote.
  Las interrupciones se producirán ante cualquier cambio de estado, pero sólo la pulsación
  hará cambiar el led.

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
const unsigned long delayBounce = 200; //Tiempo de inactividad para efecto rebote
unsigned long nextIntR,nextIntG,nextIntB; //Tiempo a partir del cuál se admitirán nuevas interrupciones
 
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
  attachInterrupt(pinSwitchR,IntSwitchR,CHANGE);
  attachInterrupt(pinSwitchG,IntSwitchG,CHANGE);
  attachInterrupt(pinSwitchB,IntSwitchB,CHANGE);
}

void loop()
{
  //No tenemos nada especial que hacer aquí. Todo se gestiona por interrupciones
}

void IntSwitchR()
{
  //El interruptor R ha cambiado de estado

  //Tomamos nota del estado del interruptor
  bool switchStatus=digitalRead(pinSwitchR);
  //Tomamos nota del tiempo actual
  unsigned long now=millis();
  //Si hemos superado el tiempo de espera...
  if(now>nextIntR)
  {
    //...podemos considerar la nueva interrupción

    //Si el interruptor está actualmente pulsado...
    if(!switchStatus)
    {
      //...cambiamos el estado del led
      digitalWrite(pinLedR,!digitalRead(pinLedR));
    }
    //Calculamos el tiempo para la próxima interrupción
    nextIntR=now+delayBounce;
  }
}

void IntSwitchG()
{
  //El interruptor G ha cambiado de estado

  //Tomamos nota del estado del interruptor
  bool switchStatus=digitalRead(pinSwitchG);
  //Tomamos nota del tiempo actual
  unsigned long now=millis();
  //Si hemos superado el tiempo de espera...
  if(now>nextIntG)
  {
    //...podemos considerar la nueva interrupción

    //Si el interruptor está actualmente pulsado...
    if(!switchStatus)
    {
      //...cambiamos el estado del led
      digitalWrite(pinLedG,!digitalRead(pinLedG));
    }
    //Calculamos el tiempo para la próxima interrupción
    nextIntG=now+delayBounce;
  }
}

void IntSwitchB()
{
  //El interruptor B ha cambiado de estado

  //Tomamos nota del estado del interruptor
  bool switchStatus=digitalRead(pinSwitchB);
  //Tomamos nota del tiempo actual
  unsigned long now=millis();
  //Si hemos superado el tiempo de espera...
  if(now>nextIntB)
  {
    //...podemos considerar la nueva interrupción

    //Si el interruptor está actualmente pulsado...
    if(!switchStatus)
    {
      //...cambiamos el estado del led
      digitalWrite(pinLedB,!digitalRead(pinLedB));
    }
    //Calculamos el tiempo para la próxima interrupción
    nextIntB=now+delayBounce;
  }
}

