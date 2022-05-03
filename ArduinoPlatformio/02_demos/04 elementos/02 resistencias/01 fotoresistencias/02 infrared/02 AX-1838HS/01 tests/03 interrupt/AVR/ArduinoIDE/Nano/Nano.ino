/*
  Autor: Ramón Junquera
  Tema: Receptor AX-1838HS
  Objetivo: Funcionamiento de un receptor de infrarrojos AX-1838HS
  Material: breadboard, receptor de infrarrojos modelo AX-1838HS, pulsadore, led IR,
    cables y Arduino Nano.

  Descripción:
  Hemos comprobado que si conectamos el sensor a un pin digital, no varía la respuesta cuando se enciende
  un led infrarrojo.

  En este ejercicio conservaremos el circuito anterior, pero comprobaremos si es cierto que no varía la
  señal digital.
  Para ello, aprovecharemos que estamos utilizando el pin 2 y crearemos una interrupción que vigile el
  estado el pin.
  Crearemos una variable global y volátil que contará el número de cambios detectados.
  
  La aplicación sólo se encargará de mostrar periódicamente el contador de cambios.

  Puesto que estamos utilizando un interruptor de pulsador para encender el led, es posible que tengamos 
  efecto rebote.
  Para evitarlo, no tomaremos en cuenta los cambios detectados en un tiempo determinado después del primero.
  
  En la práctica...
  Descubrimos que cuando se enciende led, el sensor AX-1838HS hace un cambio de estado digital.
  En el ejercicio anterior no lo detectábamos porque tomábamos muestras cada varias décimas de segundo.
  Ahora estamos comprobando el estado a la velocidad del reloj interno (16 millones de veces por segundo).

  Cada vez que detecta un aumento de intensidad IR, cambia a nivel bajo por un instante.
  
  Cuando se apaga el led, no hay ningún tipo de cambio de estado, tal y como sospechábamos en los ejercicios
  anteriores.
  
  Puesto que el paso a nivel bajo corresponde con un aumento del nivel infrarrojo, vemos que si mantenemos
  pulsado el botón, tenemos más probabilidades de aumentar el contador de detecciones.
  Esto es porque la intensidad del led emisor no es constante. Las pequeñas fluctuaciones son suficientes para
  ser detectadas y producir los cambios de estado.

  Resultado:
  El sensor pasa a estado bajo cada vez que detecta un aumento del nivel de luz infrarroja
*/

#include <Arduino.h>

//Variables globales
volatile int changeCount=0;
volatile unsigned long lastChange=0;
int lastCount=0;

void setup()
{
  //Activamos el puerto serie
  Serial.begin(115200);
  //Activamos la interrupción 0 correspondiente al pin 2, para que ejecute la función IntSwitch cuando
  //detecte el paso de estado HIGH a LOW
  attachInterrupt(0,IntSwitch,CHANGE);
}

void loop()
{
  //Si el contador ha variado desde la última vez...
  if(lastCount!=changeCount)
  {
    //Mostraremos el valor del contador
    Serial.println(changeCount);
    //Actualizamos el valor del último contador detectado
    lastCount=changeCount;
  }
}

void IntSwitch()
{
  //Función a la que se llama cuando cambia el estado del interruptor

  //Si ha pasado más de dos décimas de segundo desde el último cambio...
  if(millis()>lastChange+200)
  {
    //...podemos tener en cuenta el cambio actual

    //Aumentaremos el contador de cambios
    changeCount++;
    //La última vez sera la actual
    lastChange=millis();
  }
}
