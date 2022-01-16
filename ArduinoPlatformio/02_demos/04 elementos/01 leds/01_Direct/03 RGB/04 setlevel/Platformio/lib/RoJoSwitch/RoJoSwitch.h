/*
  Nombre de la librería: RoJoSwitch.h
  Versión: 20210117
  Autor: Ramón Junquera
  Descripción:
    Gestión de un interruptor.
    Utiliza las resistencias PULL_UP internas.
    El interruptor debe ir conectado a tierra y al pin que lee su estado.
*/

#ifndef RoJoSwitch_h
#define RoJoSwitch_h

#include <Arduino.h>

class RoJoSwitch {
  private:
    byte _pinSwitch; //Pin en el que está conectado el interruptor
    uint32_t long _lastTime=0; //Momento del último cambio
    bool _lastStatus=true; //Último estado comprobado. false = pulsado, true = soltado
    bool _buttonPressed=false; //Guardará el flag de si ha ha sido pulsado
    bool _buttonReleased=false; //Guardará el flag de si ha sido soltado
    //Función que hace las comprobaciones y guarda los resultados en las variable privadas
    void _check();
  public:
    RoJoSwitch(byte pin); //Constructor
    uint32_t delayTime=50; //Tiempo de espera en ms para evitar efecto rebote, tanto al pulsar como al soltar
    bool pressed(); //Comprueba si ha sido pulsado
    bool released(); //Comprueba si ha sido soltado
    bool pressing(); //Devuelve el estado actual del interruptor
}; //Punto y coma obligatorio para que no de error

#ifdef __arm__
  #include <RoJoSwitch.cpp> //Para guardar compatibilidad con RPi
#endif

#endif
