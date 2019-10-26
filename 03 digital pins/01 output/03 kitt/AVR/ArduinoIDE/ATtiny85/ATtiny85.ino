/*
  Autor: Ramón Junquera
  
  Tema: Escritura de señales digitales en un ATtiny85
  
  Objetivo: Probar las salidas digitales
  
  Material adicional: ATtiny85, breadboard, 5 leds, 5 resistencias de 220 ohmios

  Descripción:
  Aunque la Attiny85 tiene 6 pines numerados del 0 al 5, sólamente podemos utilizar
  los 5 primeros como salidas digitales.
  Tal y como describíamos en el ejercicio anterior, el pin 5 corresponde a la función
  de reset.
  El reset se produce cuando el pin 5 se pone en estado LOW.

  La descripción de los pines es la siguiente:
    Pin 0 → I2C SDA, PWM
    Pin 1 → PWM, LED integrado
    Pin 2 → I2C SCK, Analog(A1), INT0
    Pin 3 → Analog(A3)
    Pin 4 → PWM, Analog(A2)
    Pin 5 → RESET

  Se adjunta en la carpeta del proyecto una imagen que esquematiza todas las posibilidades
  de cada pin.

  Por lo tanto, dejaremos el pin 5 sin conectar.
  Si lo conectamos, comprobaremos que la placa no es reconocida por el ordenador como
  dispositivo USB y por lo tanto no se transfiere el programa.

  El programa intenta utilizar todos los pines digitales para demostrar que funcionan
  correctamente.

  Encenderemos los leds secuencialmente de uno en uno y al llegar a un extremo, cambiaremos
  el sentido de avance (efecto Kitt).

  Una vez trasferido el programa, se puede alimentar tanto por el USB como por los pines GND & 5V

  Se incluye el archivo con el diseño del ATtiny85 en formato Fritzing en la carpeta del proyecto.
  No es exáctamente el mismo modelo, pero los pines sí lo son.

  Resultado:
  Vemos el efecto Kitt con 5 leds
*/

int delta=1;
byte currentLed=0;

void setup()
{                
  //Inicializamos todos los pines como salidas
  for(byte p=0;p<5;p++) pinMode(p, OUTPUT);
}

void loop()
{
  //Encendemos el led
  digitalWrite(currentLed, HIGH);
  //Esperamos un momento
  delay(200);
  //Apagamos el led
  digitalWrite(currentLed, LOW);
  //Pasamos al siguiente led
  currentLed+=delta;
  //Si hemos llegado a un extremo...cambiamos la dirección
  if(currentLed==0 || currentLed==4) delta*=-1;
}
