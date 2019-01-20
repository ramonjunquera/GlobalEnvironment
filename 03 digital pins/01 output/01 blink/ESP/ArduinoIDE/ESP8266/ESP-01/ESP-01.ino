/*
  Autor: Ramón Junquera
  Fecha: 20181031
  Tema: Blink en ESP-01
  Objetivo: Transferir un programa a ESP-01 con Arduino IDE
  Material: placa ESP-01
  Descripción:
    Utilizaremos el led integrado en placa, asociado a GPIO 1 (TX) para hacerlo parpadear cada segundo.
    Se necesitará un circuito de transferencia para enviar el programa al ESP-01
  Resultado:
  El led conectado al ESP-01 parpadea
*/

void setup()
{
  //Configuramos el pin del led integrado como salida
  pinMode(LED_BUILTIN,OUTPUT);
}

void loop()
{
  //Cambiamos el estado del led
  digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN));
  //Esperamos un segundo
  delay(1000);
}

