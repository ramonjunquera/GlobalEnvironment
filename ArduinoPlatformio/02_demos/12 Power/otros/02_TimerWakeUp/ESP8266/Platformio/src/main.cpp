/*
  Tema: WeMosD1 R2 & Mini
  Fecha: 20211116
  Autor: Ramón Junquera
  Objetivo: Modo de bajo consumo

  Descripción:
  La placa WeMos puede "hibernar" durante un periodo de tiempo definido.
  La función es: ESP.deepSleep(unsigned int32 time_to_sleep)
  El parámetro es el tiempo en microsegundos que debe permanecer hibernado
  Puesto que es un unsigned int32 el máximo valor que puede contener es
  de 4294967296.
  Las equivalencias son las siguientes:
    4294967296 µs
    4294967 ms
    429 s
    7,15 m
    7m 9s

  Conclusión: se puede pedir que hiberne durante poco más de 7 minutos.

  Inicialmente configuramos el pin D0 de una manera especial:
  pinMode(D0, WAKEUP_PULLUP);
  Esto obliga al pin D0 a que se quede en estado HIGH gracias a las 
  resistencias de pullup internas.
  Sólo pasará a estado LOW cuando finalice el tiempo de hibernación.
  Esta es la razón por la que el pin D0 es el único al que no se le 
  puede asignar una interrupción de hardware.

  Si utilizamos el modo de hibernación no podremos utilizar este pin para
  nada más.

  Si conectamos este pin al pin de reset (RST), no ocurrirá nada mientras
  se encuentre en estado HIGH.
  Al finalizar el tiempo de hibernación D0 pasará a estado LOW y cuando ponemos
  el pin RST en LOW producimos un reset de la placa.

  Si no unimos los pines D0 y RST, la placa nunca saldrá de una hibernación.

  Por lo tanto. Una vez que entramos en modo de hibernación, no continuaremos en
  el mismo punto del programa, sino que al terminar se reseteará la placa.

  Es conveniente reconfigurar de nuevo todo el hardware utilizado.

  Es una buena idea utilizar la memoria EEPROM o el sistema de archivos, para guardar
  la configuración, si es que se necesita una configuración específica.

  El programa hace parpadear el led integrado en placa 3 veces. Después entra
  en hibernación durante el tiempo indicado en la constante global.
  Una vez terminada la hibernación, se resetea la placa y comienza de nuevo.

  Nota:
  Cuando está conectado este circuito, habitualmente no se puede enviar un 
  programa a la placa, porque el pin RST se mantiene en estado HIGH debido a 
  su conexión con el pin D0.
  La solución es desconectarlo temporalmente para hacer la transferencia.
  
  Resultado:
  La placa se pasa la mayor parte del tiempo hinernando. Cuando está normal hace
  parpadear el led integrado 3 veces.
*/

#include <Arduino.h>

//Definición de constantes globales
const uint32_t sleepSeconds=5; //Número de segundos que durará la hibernación

void setup() {
  //Configuramos el pin D0 para que siempre esté en estado HIGH gracias a las
  //resistencias internas de pullup. Sólo cambiará a estado LOW al finalizar
  //el tiempo de hibernación
  pinMode(D0,WAKEUP_PULLUP);
  //Configuramos el pin del led integrado como salida
  pinMode(LED_BUILTIN, OUTPUT);
  //Hacemos parpadear el led integrado tres veces
  for(byte i=3;i>0;i--) {
    digitalWrite(LED_BUILTIN,LOW); //Encendemos el led
    delay(500);
    digitalWrite(LED_BUILTIN,HIGH); //Apagamos el led
    delay(500);
  }
  //Entramos en modo de bajo consumo durante el tiempo indicado
  ESP.deepSleep(1000000*sleepSeconds);
  //Nunca superaremos este punto. Sólo saldrá de la hibernación con un reset
}
void loop() {
  //No tenemos nada especial que hacer en la función loop()
  //Las siguientes líneas hacen parpadear el led integrado rapidamente, pero nunca se ejecutarán.
  for(byte i=50;i>0;i--) {
    digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN));
    delay(50);
  }
}
