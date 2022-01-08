
/*
  Tema: Deep Sleep. Despertar con una señal digital externa (ext0)
  Fecha: 20211117
  Autor: Ramón Junquera

  Descripción:
  El controlador RTC puede acceder al estado de los pines digitales.
  Concretamente: 0,2,4,12,13,14,15,25,26,27,32,33,34,35,36,37,38,39
  Podemos programarlo para que cuando detecte en un pin concreto un estado digital determinado,
  fuerce la salida de Deep Sleep.

  IMPORTANTE. Este sistema permite definir un único pin para salir del modo Deep Sleep.
  Si necesitamos más de uno, tendremos que utilizar otro método.

  La manera más sencilla de cambiar el estado digital de un pin es mediante un pulsador.
  Recordemos que tenemos a nuestra disposición resistencias internas tanto de pull-up como
  de pull-down. De todas maneras, se incluye en el circuito el ejemplo con una resistencia
  pull-down externa.

  Como constantes globales definimos el pin de que utilizaremos para salir del Deep Sleep.

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
const byte pinWakeup=33;

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
  //Activamos resistencia interna para mantenerlo en estado LOW mientras esté desconectado
  pinMode(pinWakeup,INPUT_PULLDOWN);
  //Indicamos el pin y el estado que debe leerse para salir del Deep Sleep
  //Puesto que mantenemos a pinWakeup en LOW gracias a la resistencia interna de pull-down,
  //esperamos que con la pulsación, su estado pase a HIGH
  esp_sleep_enable_ext0_wakeup((gpio_num_t)pinWakeup,HIGH);
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

