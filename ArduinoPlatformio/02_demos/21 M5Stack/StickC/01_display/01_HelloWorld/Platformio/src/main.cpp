#include <Arduino.h>
#include <M5StickC.h>

void setup() {
  //Inicialización de accesorios del dispositivo
  M5.begin();
  //Escibimos texto con fuente por defecto en la esquina superior izquierda en blanco
  M5.Lcd.print("Hello World");
}

void loop() {
  delay(100);
}