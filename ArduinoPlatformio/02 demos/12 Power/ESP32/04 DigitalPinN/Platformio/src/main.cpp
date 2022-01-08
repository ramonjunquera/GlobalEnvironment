
/*
  Tema: Deep Sleep. Despertar con una o varias señales digitales externas (ext1)
  Fecha: 20211117
  Autor: Ramón Junquera

  Objetivo:
  Salir de Deep Sleep con la pulsación de cualquiera de los dos botones del circuito.

  Descripción:
  El sistema de gestión de periféricos de RTC no puede acceder al estado de todos
  los pines digitales, solamente a: 32,33,34,35,36,37,38,39.
  La gran ventaja es que puede forzar la salida de Deep Sleep con el cambio de
  estado de uno o varios de ellos.

  La manera más sencilla de cambiar el estado digital de un pin es mediante un pulsador.
  Recordemos que tenemos a nuestra disposición resistencias internas tanto de pull-up como
  de pull-down. De todas maneras, se incluye en el circuito el ejemplo con una resistencia
  pull-down externa.

  Como constantes globales definimos los pines que utilizaremos para salir del Deep Sleep
  (a los que están conectados los pulsadores).
  Creamos una tercera constante que será la máscara que indica los pines afectados.
  Esta máscara es un uint64_t y cada posición corresponde a un pin.

  Seguiremos utilizando la memoria RTC para guardar el contador de reinicios.

  Cada vez que el programa reinicie, hará parpadear el led integrado 3 veces.
  Este tiempo se aprovecha para inicializar el pruerto serie.
  A continuación se envía por el puerto serie la razón por la que se ha salido del Deep Sleep
  y el número de veces que se a reiniciado, guardado en la variable bootCount.

  Nota. Si forzamos un reinicio pulsando el botón de reset, borraremos el contenido de
  todas las memorias. Incluidas las de RTC. Perdiendo el valor de la variable bootCount.
*/

#include <Arduino.h>
#include <driver/rtc_io.h> //Para rtc_gpio_hold_en

//Definición de constantes globales
const byte pinWakeup1=32;
const byte pinWakeup2=25;
//Definimos la máscara de los pines para los que queremos comprobar su estado
//                            3333333333222222222211111111110000000000
//                            9876543210987654321098765432109876543210
//const uint64_t pinsWakeup=0b0000001100000000000000000000000000000000;
const uint64_t pinsWakeup=((uint64_t)1<<pinWakeup1)+((uint64_t)1<<pinWakeup2);

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
  //Activamos resistencias internas para mantener los pines en estado LOW mientras estén desconectados
  pinMode(pinWakeup1,INPUT_PULLDOWN);
  pinMode(pinWakeup2,INPUT_PULLDOWN);
  //Cuando trabajamos con interfases/periféricos RTC no es suficiente con definir el modo del pin,
  //necesitamos que el modo se mantenga al entrar en Deep Sleep
  rtc_gpio_hold_en((gpio_num_t)pinWakeup1);
  rtc_gpio_hold_en((gpio_num_t)pinWakeup2);
  //Nota: Si utilizamos resistencias de pull-down externas sobra esta cofiguración.
  //Indicamos la máscara de pines a tener en cuenta y el estado que hará salir del Deep Sleep
  //Podemos salir de Deep Sleep con una de las siguientes condiciones:
  // - ESP_EXT1_WAKEUP_ALL_LOW  : Todos los pines de la máscara se encuentran en estado LOW
  // - ESP_EXT1_WAKEUP_ANY_HIGH : Alguno de los pines de la máscara tiene estado HIGH
  esp_sleep_enable_ext1_wakeup(pinsWakeup,ESP_EXT1_WAKEUP_ANY_HIGH);
  esp_deep_sleep_start(); //Entramos en modo Deep Sleep
  //Nunca llegaremos a este punto. Al salir del Deep Sleep reiniciaremos el programa
}

void loop() {
  //Nunca ejecutaremos las siguientes líneas que hacen parpardear el led rapidamente
  for(byte i=40;i>0;i--) {
    digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN));
    delay(50);
  }
}

