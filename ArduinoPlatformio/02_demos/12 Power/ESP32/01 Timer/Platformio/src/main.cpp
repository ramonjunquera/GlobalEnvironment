
/*
  Tema: Deep Sleep. Despertar con Timer
  Fecha: 20211117
  Autor: Ramón Junquera

  Descripción:
  La función esp_sleep_enable_timer_wakeup indica el tiempo que debe permanecer en Deep Sleep.
  Su parámetro es un uint64_t con el tiempo en microsegundos.
  Su valor máximo sería:
    18446744073709551615 µs = (2*64-1)
    18446744073709551,615 ms
    18446744073709,551 s
    307445734561,825 min
    5124095576,030 h
    213503982,334 días
    584942,417 años
    5849,424 siglos
    584,942 milenios
  Conclusión. Tenemos suficiente rango como para seleccionar el valor que más nos convenga.

  Para poder recuperar valores entre reinicios, podemos utilizar el sistema de almacenamiento
  de archivos o la memoria RTC.
  Para crear una variable dentro de la memoria RTC debemos añadir el prefijo RTC_DATA_ATTR a
  la definición.

  Cada vez que el programa reinicie, hará parpadear el led integrado 3 veces.
  Este tiempo se aprovecha para inicializar el pruerto serie.
  A continuación se envía por el puerto serie la razón por la que se ha salido del Deep Sleep
  y el número de veces que se a reiniciado, guardado en la variable bootCount.

  Nota. Si forzamos un reinicio pulsando el botón de reset, borraremos el contenido de
  todas las memorias. Incluidas las de RTC. Perdiendo el valor de la variable bootCount.
*/

#include <Arduino.h>
//Definición de constantes globales
const uint64_t timeToSleep=5*1000000; //Duración de la siesta en microsegundos

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

  //Nos preparamos para volver a dormir
  esp_sleep_enable_timer_wakeup(timeToSleep); //Indicamos el tiempo de siesta
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

