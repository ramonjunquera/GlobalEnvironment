/*
  Nombre de la librería: RoJoESP32PWM.h
  Versión: 20170918
  Autor: Ramón Junquera
  Descripción:
    Gestión de PWM para placas ESP32

    A fecha actual el paquete de instrucciones para compatibilizar las placas ESP32 con el IDE
    de Arduino no está totalmente desarrollado.
    Concretamente la instrucción analogWrite para gestionar PWM no existe.
    Se ha creado la función con el mismo nombre y parámetros que la original.
    La única diferencia es que devolverá false si no ha podido completarse.
    
    El ESP32 tiene 8 canales (timer) que permiten conseguir el efecto PWM de manera sencilla.
    Los canales están numerados del 0 al 7.
    Cada canal puede ser programado con su propia amplitud de onda.
    Una vez configurado un canal se asocia a un pin, que tendrá el estado de la onda.
    Por lo tanto, en ESP32 podremos tener un máximo de 8 pines PWM con valores distintos.

    La resolución del nivel es de 8 bits (256 niveles).
        
    La frecuencia de la onda puede variar entre 1220 y 312500 Hz.
    Nosotros utilizamos la máxima.

    La función encarga de:
    - Comprobar si ya existe un canal que tenga asignado el pin solicitado para reutilizarlo.
    - Desactivar canales cuyo nivel es 0
    - Buscar canales libres para nuevas peticiones.
*/

#include <Arduino.h>
#include "RoJoAnalogWriteESP32.h"


bool analogWrite(byte pin,byte level)
{
  //Asigna un nivel PWM a un pin
  //Devuelve false si no ha podido

  //Array con los pines asignados a cada canal
  //Inicialmente todos desasignados (=0)
  static byte _pinsPWM[8]={0,0,0,0,0,0,0,0};

  //Último canal vacío
  byte lastEmpty=0xFF; 

  //El pin indicado tiene ya abierto un canal?
  //Recorremos todos los canales
  for(byte c=0;c<8;c++)
  {
    //Si hemos encontrado un canal asignado a este pin...
    if(_pinsPWM[c]==pin)
    {
      //Si el nivel es distinto de cero...cambiamos el nivel PWM del canal
      if(level) sigmaDeltaWrite(c,level);
      else //Si el nivel es cero...
      {
        //...desasignamos el pin del canal
        sigmaDeltaDetachPin(pin);
        //Ponemos el pin a low
        digitalWrite(pin,LOW);
        //Anotamos que está desasignado
        _pinsPWM[c]=0;
      }
      //Hemos terminado correctamente
      return false;
    }
    else //Este canal no está asignado al pin indicado
    {
      //Si el canal está sin asignar...anotamos que está vacío
      if(_pinsPWM[c]==0) lastEmpty=c;
    }
  }
  //Hemos recorrido todos los canales y ninguno está asignado al pin indicado
  //Si tenemos algún canal desasignado...
  if(lastEmpty!=0xFF)
  {
    //...lo utilizaremos
    //Lo anotamos
    _pinsPWM[lastEmpty]=pin;
    //Fijamos el canal a la máxima frecuencia
    sigmaDeltaSetup(lastEmpty,312500);
    //Fijamos el nivel al solicitado
    sigmaDeltaWrite(lastEmpty,level);
    //Asociamos el pin al canal
    sigmaDeltaAttachPin(pin,lastEmpty);
    //Hemos terminado correctamente
    return false;
  }
  //No tenemos canales desasignado para la petición
  //Devolvemos error
  return true;
}

