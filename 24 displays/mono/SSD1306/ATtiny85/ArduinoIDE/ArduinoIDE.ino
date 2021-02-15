/*
  Autor: Ramón Junquera
  Fecha: 20200217
  Tema: Comunicaciones I2C de ATtiny85
  Objetivo: Comprobar la comunicación I2C de Attiny85
  Material adicional: breadboard, cables, placa ATtiny85, display OLED I2C

  Descripción:
  Debemos comprobar si la placa ATtiny85 es capaz de comunicar a través del protocolo I2C
  con otros dispositivos.
  Como prueba de concepto utilizaremos display OLED I2C 0.96" 128x64 SSD1306

  El primer problema que encontramos es que la librería Wire.h que habitualmente se utiliza
  para este tipo de comunicación no es compatible con esta placa.
  Se han creado otras librería mucho más ligeras para este caso:
    TinyWireM.h para usar ATtiny85 como Master
    TinyWireS.h para usar ATtiny85 como Slave
  Estas librerías se instalan automáticamente con los drivers de ATtiny85.

  Hemos tomado las librerías de gestión de este display y las hemos reducido al máximo.
  Sólo han quedado dos funciones
    Init() para detectar e inicializar el display
    Show() para mostrar unas líneas en pantalla

  Estas dos funciones se han adaptado para que usen la librería TinyWireM.h

  Las librerías originales jamás podrán utilizarse en esta placa porque necesitan una memoria
  de vídeo de 1Kb, pero ATtiny85 sólo tiene 0.5Kb para variables.
  Aunque se escriba directamente en la memoria de vídeo del display, también tenemos el
  problema de que no podemos leerla. Por lo tanto las escrituras serían a ciegas.
  Recordando que se escribe por bytes (páginas).
  
  De todas maneras el ejemplo es suficiente para comprobar que es capaz de detectar el display
  y enviar información.

  Si no se detecta el display, el led integrado en placa se quedará parpadeando una vez por segundo.
  Si se detecta, se inicializa, se envía el dibujo de las líneas y se mantiene parpadeando
  el led rápidamente.

  Tal y como se puede ver en el esquema del circuito, los pines I2C son:
    pin 0 = SDA
    pin 2 = CLK
  
  Resultado:
  La placa ATtiny detecta el display OLED vía I2C y dibuja unas líneas horizontales.
*/
#include "RoJoOLED_dev.h" //Librería de gestión del display

//Creamos objeto de gestión
RoJoOLED oled;

//Dibuja líneas verticales
void linesV() {
  for(byte x=0;x<128;x+=8) oled.lineV(x);
}

void setup() {
  //Configuramos el pin del led integrado como salida
  pinMode(1,OUTPUT);
  
  //Si no se puede inicializar...
  if(!oled.begin()) {
    //...hacemos parpadear indefinidamente el led integrado en placa
    while(1) {
      digitalWrite(1,!digitalRead(1));
      delay(1000);
    }
  }
  while(1) {
    //Mostramos líneas verticales
    oled.clear();
    linesV();
    delay(1000);
    
    //Mostramos el dibujo de las líneas horizontales
    oled.clear(17);
    delay(1000);

    //Añadimos la líneas verticales
    linesV();
    delay(1000);
  }
}

void loop() {
  //Hacemos parpadear rápidamente el led integrado para indicar que todo ha ido bien
  digitalWrite(1,!digitalRead(1));
  delay(50);
}
