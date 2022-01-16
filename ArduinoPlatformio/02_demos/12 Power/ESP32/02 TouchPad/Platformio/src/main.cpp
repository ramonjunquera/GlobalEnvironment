
/*
  Tema: Deep Sleep. Despertar con TouchPad
  Fecha: 20211117
  Autor: Ramón Junquera

  Descripción:
  La familia ESP32 tiene una funcionalidad llamada TouchPad que permite detectar ligeros cambios
  de tensión en un pin procedentes de tocarlo con nuestra piel. Es el mismo sistema que utilizan
  las pantallas táctiles capacitivas.
  El módulo RTC IO es capaz de detectar los cambios en los pines TouchPad y por lo tanto, puede
  salir de un Deep Sleep.

  Como constantes globales definimos el pin de que utilizaremos para la entrada TouchPad y el
  grado de sensibilidad que tendrá (threshold). Es un valor uint16_t. Cuanto mayor sea,
  mayor sensibilidad tendrá.

  Seguiremos utilizando la memoria RTC para guardar el contador de reinicios.

  Cada vez que el programa reinicie, hará parpadear el led integrado 3 veces.
  Este tiempo se aprovecha para inicializar el pruerto serie.
  A continuación se envía por el puerto serie la razón por la que se ha salido del Deep Sleep
  y el número de veces que se a reiniciado, guardado en la variable bootCount.

  Nota. Si forzamos un reinicio pulsando el botón de reset, borraremos el contenido de
  todas las memorias. Incluidas las de RTC. Perdiendo el valor de la variable bootCount.
*/

#include <Arduino.h>
//Definición de constantes globales
const byte pinTouchPad=15;
const uint16_t threshold=40;

//Definición de variables globales
RTC_DATA_ATTR uint16_t bootCount=0; //Contador de inicializaciones en memoria RTC

//Devuelve la razón por la que se ha despertado de un Deep Sleep
String wakeupReason() { 
	switch(esp_sleep_get_wakeup_cause()) { //Dependiendo del código...
		case 1: return "programa ULP";
		case 2: return "señal digital externa usando el controlador RTC";
		case 3: return "señal digital externa usando las interfases RTC";
		case 4: return "Timer";
		case 5:
      switch(esp_sleep_get_touchpad_wakeup_status()) {
        case 0: return "Touchpad 4";
        case 1: return "Touchpad 0";
        case 2: return "Touchpad 2";
        case 3: return "Touchpad 15";
        case 4: return "Touchpad 13";
        case 5: return "Touchpad 12";
        case 6: return "Touchpad 14";
        case 7: return "Touchpad 27";
        case 8: return "Touchpad 33";
        case 9: return "Touchpad 32";
        default: return "Touchpad desconocido";
      }
		default : return "razón desconocida (no estábamos en Deep Sleep)";
	}
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN,OUTPUT);
  //Hacemos parpadear el led integrado 3 veces
  for(byte i=6;i>0;i--) {
    digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN));
    delay(500);
  }
  bootCount++; //Aumentamos el número de veces que hemos despertado
  Serial.println("Despertado por "+wakeupReason()+" ("+String(bootCount)+")");

  //Nos preparamos para volver a dormir.
  //Debemos crear una interrupción que salte cuando se detecte una entrada por el pin del TouchPad.
  //La interrupción debe llamar a una función definida por nosotros.
  //En este caso, la función no tiene importancia, porque cuando RTC IO detecte la entrada,
  //finalizará el Deep Sleep y reiniciará el dispositivo. 
  //Aunque definamos la función de interrupción, nunca se llamará.
  //Por esta razón dejamos el parámetro de la función como puntero nulo.
  touchAttachInterrupt(pinTouchPad,nullptr,threshold);
  //Antes de entrar en Deep Sleep, indicamos que puede ser despertado por una interrupción de TouchPad.
  esp_sleep_enable_touchpad_wakeup();
  esp_deep_sleep_start(); //Entramos en modo Deep Sleep
  //Nunca llegaremos a este punto. Al salir del Deep Sleepo reiniciaremos el programa
}

void loop() {
  //Nunca ejecutaremos las siguientes líneas que hacen parpardear el led rapidamente
  for(byte i=40;i>0;i--) {
    digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN));
    delay(50);
  }
}

