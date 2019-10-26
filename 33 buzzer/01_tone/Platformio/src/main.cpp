/*
  Autor: Ramón Junquera
  Fecha: 20191002
  Tema: Tone
  Objetivo: Uso de la función tone en varios dispositivos

  Descripción:
    Ejemplo simple de uso de la función tone en todas los dispositivos.
    Los ESP32 no tienen esta función por defecto. Se implementa gracias a la librería RoJoToneESP32.h

    Sintaxis de la función tone:
      tone(pin,frecuencia,duración)
  
    El pin es el pin al que está conectado el buzzer
    La frecuencia es el número de veces que debe cambiar de estado por segundo. Está relacionado con el tono que
    genera. Y como ya sabemos, una nota tiene el doble de frecuencia en la octava siguiente.
    El parámetro duración en milisegundos es opcional.
  
    Lo interesante de la función tone es que no necesitamos preocuparnos por ella durante el flujo de nuestro
    programa. Podemos pedir que se genere un tono (indefinidamente o durante un tiempo limitado) y despreocuparnos.

    En el programa, guardaremos en un array las frecuencias correspondientes a las notas de la escala musical
    de DO a DO.
    Lo que haremos será repetir contínuamente esta escala, parando un segundo al finalizar.

  Nota:
    Si en vez de utilizar un buzzer pasivo conectamos uno activo descubriremos que también funciona, aunque con un 
    volumen más bajo y con un sonido distorsionado.
    Para evitar la distorsión es suficiente con que incluyamos una resistencia de al menos 100 ohmios.
    El volumen nunca alcanzará al de un zumbador pasivo, y dependerá del valor de la resistencia que pongamos.
    Pero la pregunta es: porqué funciona un zumbador pasivo si tiene su propio oscilador?
    La respuesta es que aunque están preparado sólo para vibrar en una sóla frecuencia (la que genera su oscilador)
    nosotros nos encargamos de activar y desactivar la energía del componente con el comando tone.
    Cada vez que lo activamos genera un pico que es similar al pico de la onda que queremos crear.
    Conclusión. Podríamos utilizar zumbadores activos, pero no es recomendable. La calidad no es la misma.
*/

#include <Arduino.h>

#ifdef ESP32 //Si es un ESP32 necesita una librería adicional
  #include <RoJoToneESP32.h>
#endif

//Pinout
#ifdef ARDUINO_M5STACK_FIRE //Si es un M5Stack Fire...
  const byte pinBuzzer=25;
#elif defined(ARDUINO_M5Stick_Watch) //Si es un M5Stick Watch
  const byte pinBuzzer=26;
#elif defined(ARDUINO_ARCH_AVR) //Si es una placa Arduino
  const byte pinBuzzer=8; //Coincide con el buzzer integrado de Maker UNO
#endif

//Frecuencias de las notas musicales
uint16_t tones[] = {261, 294, 330, 349, 392, 440, 494, 522};

void setup() {
}

void loop() {
  //Recorremos todos los tonos del array
  for(byte i=0;i<8;i++) {
    //Generamos el tono que corresponde y hacemos que dure 400ms
    tone(pinBuzzer,tones[i],400);
    //Esperamos medio segundo. El todo finalizará una décima de segundo antes de comenzar el siguiente
    delay(500);
  }
  //Silenciamos. Pedimos que no genere tono.
  noTone(pinBuzzer);
  //Esperamos un segundo
  delay(1000);
}
