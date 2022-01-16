/*
  Tema: Fotorresistencia
  Fecha: 20211020
  Autor: Ramón Junquera

Descripción:
  La resistencia que ofrece la detectaremos porque conectaremos una de sus patas a una entrada analógica.
  Para asegurarnos que siempre tendremos una señal válida (GND) en la entrada analógica aunque no funcione
  el fotorresistor, utilizaremos una resistencia de 10Kohmios como PULL-DOWN.

  Enviaremos el valor leido por el puerto serie varias veces por segundo para que pueda ser mostrado en la
  consola.
  Desde el IDE de Arduino podríamos seleccionar "Serial plotter" y ver los resultados gráficamente.
  
  Resultado:
  Comprobamos que los valores recibidos varían con la intensidad de la luz ambiental.
  Si aplicamos una fuente de luz directamente a la fotoresistencia, vemos que prácticamente no
  ofrece resistencia.
  Para aumentar la resistencia es mejor cubrirla con las manos, que taparla con un sólo dedo, porque 
  también recibe luz por los laterales.
*/

#include <Arduino.h>

#ifdef ESP32 //Wemos Lolin32 Lite
  const byte pinAnalog = 12;
#elif defined(ARDUINO_AVR_UNO) //UNO
  const byte pinAnalog = A0;
#endif

void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.println(analogRead(pinAnalog));
  delay(100);
}
