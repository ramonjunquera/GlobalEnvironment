/*
  Nombre de la librería: RoJoAnalogWriteESP32.h
  Versión: 20170918
  Autor: Ramón Junquera
  Descripción:
    Gestión de PWM para placas ESP32
*/

//Comprobamos que la placa es compatible
#if !defined(ESP32)
  #error Library RoJoAnalogWriteESP32 is only compatible with ESP32 family devices
#endif  

#ifndef RoJoAnalogWriteESP32_h
#define RoJoAnalogWriteESP32_h

#include <Arduino.h>

bool analogWrite(byte pin,byte level); //Asigna un nivel PWM a un pin

#endif
