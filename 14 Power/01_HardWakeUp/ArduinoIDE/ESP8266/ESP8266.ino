/*
  Autor: Ramón Junquera
  Tema: Modo de bajo consumo
  Fecha: 20180621
  Objetivo: Majo consume e interrupciones por hardware
  Material: placa ESP8266, interruptor

  Descripción:
  Demostración de cómo salir de una hibernación a través de un cambio de estado de un pin.
  Esto lo hacemos poniendo el pin RST en estado LOW cuando pulsamos el interruptor.

  Por otra parte, cuando entra en hibernación, indicamos que se mantendrá en ella un máximo
  15 segundos. Después, saldrá de ella automáticamente, si no lo forzamos antes.
  
  Resultado:
  El interruptor obliga a salir de la hibernación provocando un reset.
*/

const unsigned long sleepSeconds=15; //Número de segundos que durará la hibernación

void setup() 
{
  //Configuramos el pin D0 para que siempre esté en estado HIGH gracias a las
  //resistencias internas de pullup. Sólo cambiará a estado LOW al finalizar
  //el tiempo de hibernación
  pinMode(D0, WAKEUP_PULLUP);
  //Configuramos el pin del led integrado como salida
  pinMode(BUILTIN_LED, OUTPUT);
  //Hacemos parpadear el led integrado tres veces
  for(byte i=0;i<3;i++)
  {
    digitalWrite(BUILTIN_LED, LOW); //Encendemos el led
    delay(500);
    digitalWrite(BUILTIN_LED, HIGH); //Apagamos el led
    delay(500);
  }
  //Entramos en modo de bajo consumo durante el tiempo indicado
  ESP.deepSleep(1000000*sleepSeconds);
}
void loop() 
{
  //No tenemos nada especial que hacer en la función loop()
}
