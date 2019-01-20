/*
  Autor: Ramón Junquera
  Tema: Pines digitales
  Fecha: 20180803
  Objetivo: Uso de pines digitales de salida
  Material adicional: placa MakerUNO
  Descripción:
  Aprovechando que esta placa ya tiene incluidos en placa leds para los pines del 2 al 13,
  utilizaremos todos ellos (12) para este desarrollo.
  No se necesitan elementos externos.

  Nota:
  Puesto que el pin 8 está asociado con el buzzer, cada vez que cambiemos su estado se
  escuchará un chasquido. Para evitarlo es suficiente con desconectar el buzzer con el
  interruptor integrado en placa.

  Resultado:
  Podemos encender cada uno de los leds conectados a los pines digitales por separado.
*/

//Definición de variables globales
const byte pin[12]={2,3,4,5,6,7,8,9,10,11,12,13};
byte currentLed=0;
int delta=1;
const unsigned long delayValue=100; 

void setup()
{
  //Configuramos todos los pines de salida
  for(byte p=0;p<12;p++) pinMode(pin[p],OUTPUT);
}
void loop()
{
  //Apagamos el led actual
  digitalWrite(pin[currentLed],LOW);
  //Pasamos al siguiente pin
  currentLed+=delta;
  //Si hemos llegado a un extremo...cambiamos de dirección
  if(currentLed==0 || currentLed==11) delta=-delta;
  //Encendemos el led
  digitalWrite(pin[currentLed],HIGH);
  //Esperamos
  delay(delayValue);
}

