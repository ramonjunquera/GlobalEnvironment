/*
  Autor: Ramón Junquera
  Tema: Processing. Comunicación serie
  Fecha: 20180721
  Objetivo: Enviar información desde Processing a Arduino
  Material adicional: cualquier placa

  Descripción:
  Toda el espacio de visualización estará ocupado por una imagen de una bombilla.
  Su estado (encendido/apagado) cambiará al detectarse una pulsación del ratón en
  cualquier punto.
  Además de actualizar la imagen, se enviará un código a Arduino por el puerto serie
  que para que se le aplique el mismo estado al led integrado en placa.
  Los códigos serán:
    0 : apagar
    1 : encender
  Estos códigos corresponden con el estado en Processing.
  
  Al igual que todos los ejercicios en los que se utilice el puerto serie, se implementa
  la rutina de autoidentificación de puerto.
  
  Resultado:
  Cuando cambiamos el estado de la imagen en processing con un click de ratón, también se
  cambia en el led integrado en placa.
*/

#include <Arduino.h>

//Los estados del led integrado dependen de la placa
#ifdef ARDUINO_ARCH_AVR //Placas Arduino
  const bool ledON=HIGH;
  const bool ledOFF=LOW;
#else //Resto de placas (ESP)
  const bool ledON=LOW;
  const bool ledOFF=HIGH;
#endif

void setup()
{
  Serial.begin(115200);
  //Carácter recibido
  char recChar='a'; //Cualquiera que no sea el esperado para que funcione la primera vez
  //Contador. Hasta que no recorremos todos los valores del contador, no enviamos un nuevo carácter
  uint16_t counter=0;
  //Enviaremos el carácter '0' hasta que nos respondan con el mismo carácter
  //Mientras el carácter recibido sea distinto a 'J' seguiremos probando
  while(recChar!='J')
  {
    //Si hay información pendiente por recibir...leemos el siguiente carácter
    if(Serial.available()) recChar=Serial.read();
    //No hay información pendiente...Si el contador es 0...enviamos el carácter 'R'
    else if(!++counter) Serial.write('R');
  }
  //Ya han reconocido el puerto serie por el que enviaremos. Fin de la inicialización

  //El pin del led integrado se configura de salida
  pinMode(LED_BUILTIN,OUTPUT);
}

void loop()
{
  //Si hay caracteres pendientes por recibir en el puerto serie...
  if(Serial.available())
  {
    //...leemos el carácter y decidimos qué hacer en cada caso
    switch(Serial.read())
    {
      case '0': //Solicitud de apagado
        digitalWrite(LED_BUILTIN,ledOFF);
        break;
      case '1': //Solicitud de encendido
        digitalWrite(LED_BUILTIN,ledON);
        break;
      //No tendremos en cuenta culquier otro caso
    }
  }
  //Incluimos un refresco de los procesos de fondo para evitar un error WatchDog por falta de
  //refresco en placas ESP
  yield();
}

