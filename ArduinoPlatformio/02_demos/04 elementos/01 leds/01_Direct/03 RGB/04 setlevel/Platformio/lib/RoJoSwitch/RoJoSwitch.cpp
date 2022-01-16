/*
  Nombre de la librería: RoJoSwitch.h
  Versión: 20210117
  Autor: Ramón Junquera
  Descripción:
    Gestión de un interruptor.
    Utiliza las resistencias PULL_UP internas.
    El interruptor debe ir conectado a tierra y al pin que lee su estado.
*/

#ifndef RoJoSwitch_cpp
#define RoJoSwitch_cpp

#include <RoJoSwitch.h>

//Constructor
//Se pasa el pin al que está conectado el pulsador. La otra pata del pulsador debe ir a GND
RoJoSwitch::RoJoSwitch(byte pin) { 
  _pinSwitch=pin; //Guardamos el pin al que está conectado el interruptor
  pinMode(_pinSwitch,INPUT_PULLUP); //Configuramos el pin de entrada con resistencia de PULLUP
}

//Comprueba el estado del interruptor
void RoJoSwitch::_check() {
  uint32_t now=millis(); //Tomamos nota de la hora actual
  if(digitalRead(_pinSwitch)!=_lastStatus) { //Si ha cambiado el estado del pulsador...
    _lastStatus=!_lastStatus; //Actualizamos es estado con el actual (lo cambiamos)
    if(now-_lastTime>delayTime) { //Si podemos aceptar una nueva pulsación...
      if(_lastStatus) _buttonReleased=true; //Si se acaba de soltar...indicamos que se ha soltado
      else _buttonPressed=true; //Si se acaba de pulsar...indicamos que se ha pulsado
    }
    _lastTime=now; //Tomamos nota del momento del cambio para esperar el tiempo indicado y evitar el efecto rebote
  }
}

//Indica si el interruptor ha sido pulsado
bool RoJoSwitch::pressed() {
  _check(); //Llamamos a la función que refresca el estado actual del interruptor
  if(_buttonPressed) { //Si ha sido pulsado...
    _buttonPressed=false; //...limpiamos el flag, porque ya ha sido consultado
    return true; //Devolvemos la respuesta
  }
  return false; //No ha sido pulsado
}

//Indica si el interruptor ha sido soltado
bool RoJoSwitch::released() {
  _check(); //Llamamos a la función que refresca el estado actual del interruptor
  if(_buttonReleased) { //Si ha sido soltado...
    _buttonReleased=false; //...limpiamos el flag, porque ya ha sido consultado
    return true; //Devolvemos la respuesta
  }
  return false; //No ha sido soltado
}

//Indica si el interruptor está siendo pulsado
bool RoJoSwitch::pressing() {
  _check(); //Llamamos a la función que refresca el estado actual del interruptor
  //Devolvemos el estado en el encuentra interruptor actualmente (el inverso porque es PULLUP)
  return !_lastStatus; 
}

#endif
