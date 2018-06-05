/*
  Autor: Ramón Junquera
  Tema: WeMosD1 R2
  Objetivo: Uso de pines digitales de salida
  Material adicional: WeMos D1 o Mini, 11x led, breadboard
  Descripción:
  Utilizaremos los 11 pines digitales de la placa.
  Puesto que también usamos los pines del puerto serie, éste quedará inutilizado.
  Se deben desconectar los pines RX/TX cuando se transfiera el programa.

  Aunque conocemos la tabla de equivalencias de pines...
  
  WeMosD1  ArduinoIDE
  -------- ----------
  SCL/D1   5
  SDA/D2   4
  SCK/D5   14 - led integrado SCK
  MISO/D6  12
  MOSI/D7  13
  SS/D8    15
  D7       13
  D6       12
  D5       14 - led integrado SCK
  D4       2 - led integrado antena con pullup
  D3       0
  SDA/D2   4
  SCL/D1   5
  D0       16
  TX       1
  RX       3
  A0       A0
  SDA      4
  SCL      5

  ...por defecto tenemos creadas las siguientes constantes: D0,D1,D2,D3,D4,D5,D6,D7,D8,RX y TX.
  Cada una de ellas contiene el valor del pin que le corresponde en el entorno de desarrollo
  de Arduino. Esto nos permite olvidarnos de la tabla de conversión y asignar directamente los
  valores en base a estas constantes.

  Resultado:
  Podemos encender cada uno de los leds conectados a los pines digitales por separado.
*/

//Definición de variables globales
const byte pin[11]={TX,RX,D1,D2,D3,D4,D8,D7,D6,D5,D0};
byte currentLed=0;
int delta=1;
const unsigned long delayValue=100; 

void setup()
{
  //Configuramos todos los pines de salida
  for(byte p=0;p<11;p++) pinMode(pin[p],OUTPUT);
}
void loop()
{
  //Apagamos el led actual
  digitalWrite(pin[currentLed],LOW);
  //Pasamos al siguiente pin
  currentLed+=delta;
  //Si hemos llegado a un extremo...cambiamos de dirección
  if(currentLed==0 || currentLed==10) delta=-delta;
  //Encendemos el led
  digitalWrite(pin[currentLed],HIGH);
  //Esperamos
  delay(delayValue);
}

