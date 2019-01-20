/*
  Autor: Ramón Junquera
  Tema: Fotorresistencia
  Objetivo: Funcionamiento de una fotorresistencia estándar
  Material: breadboard, fotorresistencia, resistencia de 10Kohmios, cables, Arduino UNO

  Descripción:
  La resistencia que ofrece la detectaremos porque conectaremos una de sus patas a una entrada analógica.
  Para asegurarnos que siempre tendremos una señal válida (GND) en la entrada analógica aunque no funcione
  el fotorresistor, utilizaremos una resistencia de 10Kohmios como PULL-DOWN.

  Enviaremos el valor leido por el puerto serie varias veces por segundo para que pueda ser mostrado en la
  consola. Si estamos utilizando una versión del IDE con "Serial plotter" podremos ver gráficamente los
  resultados.
  
  Utilizaremos la primera entrada enalógica (A0).
  
  Resultado:
  Comprobamos que los valores recibidos varían con la intensidad de la luz ambiental.
  Si aplicamos una fuente de luz (linterna) directamente a la fotoresistencia, vemos que prácticamente no
  ofrece resistencia.
  Para aumentar la resistencia es mejor cubrirla con las manos, que taparla con un sólo dedo, porque 
  recibe menos luz.
  El rango de valores de un puerto analógico es de [0,1023]. Podemos ver que con luz ambiental, los valores
  se encuentran por encima de la media (512). Y cubriendo la fotoresistencia, por debajo.
*/

#include <Arduino.h>

void setup()
{
  //Activamos el puerto serie
  Serial.begin(115200);
}

void loop()
{
  //Enviamos por el puerto serie el valor analógico leido
  Serial.println(analogRead(A0));
  //Esperamos un momento
  delay(100);
}
