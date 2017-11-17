/*
  Autor: Ramón Junquera
  Tema: Fotorresistencia infrarroja AX-1838HS
  Objetivo: Funcionamiento de una fotorresistencia infrarroja AX-1838HS con librería IRremote.h
  Material adicional: breadboard, receptor de infrarrojos modelo AX-1838HS, cables y mando a distancia de televisión.

  Descripción:
  Para emitir la señal infrarroja utilizaremos un mando a distancia cualquiera de una televisión.
  En mi caso he utilizado un mando Sony modelo RMT-D247P.

  Cada fabricante tiene su propio protocolo de transmisión infrarroja. Aun así, la librería IRremote parece reconocer
  todos los mandos que he probado.

  El ejemplo que mostramos es una variación del ejemplo que viene con la librería.
  Nos permite ver por la consola del puerto serie, los códigos de las pulsaciones que detecta el receptor.

  Así sabremos si la librería es capaz de decodificar correctamene el protocolo utilizado por el mando a distancia, y por otra
  parte podremos tomar nota de los códigos de las teclas que nos interese utilizar en nuestros programas.

  Resultado:
  Al pulsar los botones del mando a distancia podremos ver en la consola del puesto serie los códigos que les corresponden
*/

#include <Arduino.h>
#include <IRremote.h>

//Definición de pines
const byte pinIRrec=2;

//Creación de objetos globales
IRrecv ir(pinIRrec);

void setup()
{
  //Activamos el puerto serie
  Serial.begin(115200);
  //Activamos el receptor de infrarrojos
  ir.enableIRIn();
}

void loop()
{
  //Creamos objeto para recibir los resultados del receptor infrarrojo
  decode_results results;
  //Si se ha recibido algo por el receptor infrarrojo...
  if(ir.decode(&results))
  {
    //..enviamos el código por el puerto serie en hexadecimal y binario
    Serial.print(results.value,HEX);
    Serial.print(" - ");
    Serial.println(results.value,BIN);
    
    //Esperamos medio segundo para que no se llene la pantalla de códigos
    delay(500);
    //Indicamos que estamos preparados para recibir el siguiente código
    ir.resume();
  }
}
