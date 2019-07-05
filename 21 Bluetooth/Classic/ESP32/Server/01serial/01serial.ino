/*
  Autor: Ramón Junquera
  Tema: Bluetooth
  Fecha: 20180819
  Objetivo: Conectar el puerto serie con bluetooth
  Material: ESP32

  Descripción:
  Conectaremos el bluetooth con el puerto serie, en ambas direcciones.
  No necesitamos ningún circuito especial puesto que el dispositivo bluetooth está integrado
  en el microprocesador. 

  Resultado:
  Podemos recibir y enviar información entre un cliente bluetooth y el puerto serie
*/

#include <Arduino.h>
#include "BluetoothSerial.h"

//Variables globales
BluetoothSerial BT; //Objeto de gestión del bluetooth

void setup()
{
  Serial.begin(115200);
  BT.begin("ESP32"); //Nombre del dispositivo bluetooth
  Serial.println("\nBluetooth activado");
}

void loop()
{
  //Si hay datos pendientes de recibir por el puerto serie...recogemos un byte y lo enviamos por el bluetooth
  if (Serial.available()) BT.write(Serial.read());
  //Si hay datos pendientes de recibir por el bluetooth...recogemos un byte y lo enviamos por el puerto serie
  if (BT.available()) Serial.write(BT.read());
  //Esperamos un momento para que no se sature
  delay(20);
}
