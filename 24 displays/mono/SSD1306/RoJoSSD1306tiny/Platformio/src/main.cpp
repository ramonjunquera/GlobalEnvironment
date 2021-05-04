/*
  Autor: Ramón Junquera
  Fecha: 20210504
  Tema: Librería para display OLED I2C 0.96" 128x64 SSD1306
  Objetivo: Demo de librería RoJoSSD1306tiny

  Descripción y demostración de funcionalidades de la librería.
  Librería especialmente diseñada para dispositivos con muy pocos
  recursos, como el ATtiny85.

  El primer problema que encontramos es que la librería Wire.h que habitualmente se utiliza
  para este tipo de comunicación no es compatible con esta placa.
  Se han creado otras librería mucho más ligeras para este caso:
    TinyWireM.h para usar ATtiny85 como Master
    TinyWireS.h para usar ATtiny85 como Slave
  Estas librerías se instalan automáticamente con los drivers de ATtiny85.
  Como en Linux no hay instalacióin de drivers, se puedes descargar desde:
    https://github.com/adafruit/TinyWireM

  Hemos tomado las librerías de gestión de este display y las hemos reducido al máximo.
  Sólo han quedado tres funciones
    begin: para detectar e inicializar el display
    clear: para borrar la pantalla
    lineV: para mostrar una línea vertical en pantalla

  Estas funciones se han adaptado para que usen la librería TinyWireM.h

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
  La placa ATtiny detecta el display OLED vía I2C y realiza unos tests grácicos cíclicos
*/
#include <RoJoSSD1306tiny.h> //Librería de gestión del display

//Creamos objeto de gestión
RoJoSSD1306tiny display;

//Dibuja líneas verticales
void linesV() {
  for(byte x=0;x<128;x+=8) display.lineV(x);
}

void setup() {
  pinMode(1,OUTPUT); //Configuramos el pin del led integrado como salida
  if(!display.begin()) { //Si no se puede inicializar...
    while(1) { //...hacemos parpadear indefinidamente el led integrado en placa
      digitalWrite(1,!digitalRead(1));
      delay(1000);
    }
  }
}

void loop() {
  //Mostramos líneas verticales
  display.clear();
  linesV();
  delay(2000);
  
  //Mostramos el dibujo de las líneas horizontales
  display.clear(17);
  delay(2000);

  //Añadimos la líneas verticales
  linesV();
  delay(2000);
}
