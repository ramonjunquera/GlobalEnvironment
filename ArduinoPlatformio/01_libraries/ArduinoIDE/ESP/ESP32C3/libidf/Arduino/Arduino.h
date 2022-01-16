/*
  Tema: Librería de emulación de Arduino para framework ESP-IDF
  Fecha: 20220116
  Autor: Ramón Junquera
*/

#ifndef RoJoArduino_h
#define RoJoArduino_h

#include <driver/gpio.h> //Para trabajar con pines
#include <freertos/FreeRTOS.h> //Funciones generales
#include <freertos/task.h> //Para delay()

//Constantes globales
#define LOW 0
#define HIGH 1
#define PI 3.14159265

//Definición de tipos de datos
typedef uint8_t byte;
typedef enum {
  INPUT=0,
  OUTPUT=1,
  INPUT_PULLUP=2,
  INPUT_PULLDOWN=3
} arduinoPinMode;

//Devuelve el control después de transcurridos los milisegundos indicados.
void delay(uint32_t ms) {
  vTaskDelay(ms/portTICK_RATE_MS);
}

//Configura el modo de un pin
void pinMode(byte pin,arduinoPinMode mode) {
  gpio_config_t io_conf; //Instanciamos estructura de configuración de pin
  //La llenamos con valores estándar
  io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pin_bit_mask = (1ULL << pin);
  io_conf.pull_down_en = 0;
  io_conf.pull_up_en = 0;

  switch(mode) { //Ajustamos parámetros dependiendo del modo
    case INPUT:
      io_conf.mode = GPIO_MODE_INPUT;
      break;
    case OUTPUT:
      break;
    case INPUT_PULLUP:
      io_conf.mode = GPIO_MODE_INPUT;
      io_conf.pull_up_en = 1;
      break;
    case INPUT_PULLDOWN:
      io_conf.mode = GPIO_MODE_INPUT;
      io_conf.pull_down_en = 1;
      break;
  }
  gpio_config(&io_conf); //Aplicamos configuración
}

//Escribe el estado de un pin digital
//Los valores de value son: LOW=0 HIGH=1
void digitalWrite(byte pin,byte value) {
  gpio_set_level(pin,value);
}

//Referenciamos las funciones principales definidas en el programa principal
extern void setup();
extern void loop();

//Simulamos la estructura de ejecución típica de Arduino
int app_main(int argc, char **argv) {
  setup();
  while(true) {
    loop();
  }
}

#endif