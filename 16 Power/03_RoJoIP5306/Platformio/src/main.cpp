/*
  Autor: Ramón Junquera
  Fecha: 20190920
  Tema: Gestión de batería con IP5306 
  Objetivo: Ejemplo de uso de librería
  Material: M5Stack Fire

  Descripción:
    Ejemplo simple de las funcionalidades principales de la librería.
    Muestra en pantalla los siguiente valores:
    - Está conectado a alimentación
    - Si está cargado a tope
    - % de carga de batería
    Los refresca cada segundo.
*/

#include <Arduino.h>
#include <RoJoILI9341.h> //Librería de gestión del display ILI9341
#include <RoJoIP5306.h> //Gestión de recarga de batería

//Definición de variables globales
RoJoILI9341 display;
RoJoIP5306 bat;

void setup() {
  display.begin(33,27,14,32); //Inicialización de display para M5Stack Fire
  display.rotation(1); //Botones abajo en M5Stack Fire
  bat.begin(); //Inicialización de gestor de batería
}

String SiNo(bool value) {
  if(value) return "SI";
  return "NO";
}

void loop() {
  display.clear();
  display.printOver("/10x15.fon","Esta cargando? : "+SiNo(bat.isCharging()),0xFFFF);
  display.printOver("/10x15.fon","Carga completa? : "+SiNo(bat.isChargeFull()),0xFFFF,0,16);
  display.printOver("/10x15.fon","Nivel de bateria : "+String(bat.getBatteryLevel()*25)+"%",0xFFFF,0,32);
  delay(1000);
}
