/*
  Autor: Ramón Junquera
  Tema: WeMosD1 R2 & Mini
  Fecha: 20180621
  Objetivo: Ejemplo de lectura analógica
  Material: breadboard, potenciómetro de 10 Kohmios, 5 leds, placa ESP8266

  Descripción:
  En esta ocasión el potenciómetro conectado al pin analógico de entrada servirá para
  seleccionar el led que está encendido.
  Aprovechamos las capacidades PWM de todos los pines para que el paso de un led a 
  otro se haga con suavidad.
    
  Resultado:
  Al girar el potenciómetro va cambiando suavemente el led encendido
*/

//Definición de constantes globales
const int pinCount=5; //Número de pines
const byte pin[pinCount]={D1,D2,D3,D4,D5}; //Pines que utilizaremos

void setup()
{
  //Todos los pines de los leds son de salida
  for(byte i=0;i<pinCount;i++) pinMode(pin[i],OUTPUT);
}

void loop()
{
  //Tomamos nota del valor del pin analógico
  int a=analogRead(A0);
  //Recorremos todos los leds
  for(int i=0;i<pinCount;i++)
  {
    //Calculamos el brillo del led procesado
    //Se divide el total de valores (máx 1023) en tantos bloques como leds-1 tengamos
    //El inicio de un bloque será nuestro valor de referecia
    //Cuanto más cerca del valor de referencia esté el valor analógico leido, más 
    //brillo tendrá ese led
    int level=1023-4*abs(a-i*1023/(pinCount-1));
    //No podemos asignar valores negativos
    if(level<0) level=0;
    //Aplicamos el valor de brillo calculado al led
    analogWrite(pin[i],level);
  }
}
