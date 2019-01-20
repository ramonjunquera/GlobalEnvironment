/*
  Autor: Ramón Junquera
  Tema: Processing. Comunicación serie
  Fecha: 20180715
  Objetivo: Interactuar desde Arduino con Processing
  Material adicional: cualquier placa

  Descripción:
  Tomamos como base un circuito simple que sólo contiene un interruptor.
  Si la placa tiene un interruptor incluido, como 'DOIT ESP32 DevKit V1' o 'ESP32 OLED TTGO 16Mb'
  lo utilizaremos. Sino, incluiremos un interruptor externo asociado al pin 0 en placas ESP32.

  El objetivo del desarrollo es que cada vez que se detecte una pulsación en el interruptor, se
  envíe un carácter/byte determinado a Processing.
  En Processing se recibirá el carácter clave y se cambiará el estado booleano que guarda que es
  representado por una imagen de una bombilla (encendida o apagada) que se muestra en tiempo real.

  En Processing, cargaremos inicialmente todas las imágenes utilizadas para no tener que cargarlas
  cada vez que se requiere.
  Las imágenes se encuentran en la misma carpeta del desarrollo.

  Como carácter clave tomaremos 'X'.

  Utilizaremos la librería de gestión de interruptores para simplificar el desarrollo.

  Hay otra parte muy importante en la inicialización: la detección del puerto serie usado para la
  comunicación.

  En el PC podemos tener varios puertos serie que pueden ser creados de manera dinámica.
  Podríamos definir una constante global al principio del desarrollo, pero no tendría porqué se
  correcta en todos los casos. Además se agrava teniendo en cuenta que los puertos serie no tienen
  la misma nomenclatura en Windows que en Linux.

  Para evitar hacer modificaciones, hemos creado un detector automático de puerto serie.

  El funcionamiento en simple.
  Desde Arduino nos dedicamos a enviar periódicamente un carácter de control que servirá para informar
  a quien escuche que pretendemos utilizar ese puerto.
  Al mismo tiempo estamos escuchando por si se recibe respuesta. Si recibimos el carácter adecuado
  entenderemos que alguien nos ha escuchado y que además entiende y acepta el puerto de comunicación.
  En ese momento dejamos de enviar caracteres de control y pasamos a funcionamiento real del
  desarrollo.
  Desde Processing recorreremos todos los puertos existentes secuencialmente.
  Utilizamos un control de excepciones por si el puerto no está realmente disponible o no es accesible.
  Si conseguimos abrir el puerto, damo un tiempo prudencial para poder recibir algún carácter.
  Si recibimos el carácter de control, automáticamente respondemos por el mismo puerto y lo dejamos,
  Si no, lo intentamos con el siguiente puerto.
  
  Tanto el carácter que envía Arduino como el que responde Processing para confirmar se han definido como '0'.
  
  Resultado:
  Una vez detectado el puerto de comunicación, cuando se pulsa el interruptor en la placa, cambia el estado
  de la imagen en Processing
*/

#include <Arduino.h>
#include "RoJoSwitch.h" //Gestión de interruptores

//Utilizaremos el interruptor integrado en placa correspondiente al pin 0
#ifdef ESP32
  //Seleccionamos el pin 0 porque algunas placas ESP32 ya tienen un interruptor integrado asignado a este pin
  const byte pinSwitch=0;
#elif defined(ESP8266)
  const byte pinSwitch=D1;
#elif defined(ARDUINO_ARCH_AVR)
  const byte pinSwitch=2;
#endif

//Declaramos el objeto de gestión del interruptor
RoJoSwitch mySwitch(pinSwitch);

void setup()
{
  Serial.begin(115200);
  //Carácter recibido
  char recChar='a'; //Cualquiera que no sea el esperado para que funcione la primera vez
  //Contador. Hasta que no recorremos todos los valores del contador, no enviamos un nuevo carácter
  uint16_t counter=0;
  //Enviaremos el carácter '0' hasta que nos respondan con el mismo carácter
  //Mientras el carácter recibido sea distinto a '0' seguiremos probando
  while(recChar!='0')
  {
    //Si hay información pendiente por recibir...leemos el siguiente carácter
    if(Serial.available()) recChar=Serial.read();
    //No hay información pendiente...Si el contador es 0...enviamos el carácter '0'
    else if(!++counter) Serial.write('0');
  }
  //Ya han reconocido el puerto serie por el que enviaremos. Fin de la inicialización
}

void loop()
{
  //Si se ha pulsado el botón...enviamos el código 'X'
  if(mySwitch.pressed()) Serial.write('X');
  //Incluimos un refresco de los procesos de fondo para evitar un error WatchDog por falta de
  //refresco en placas ESP
  yield();
}

