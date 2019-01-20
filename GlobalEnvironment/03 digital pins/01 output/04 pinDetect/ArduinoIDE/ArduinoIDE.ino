/*
  Autor: Ramón Junquera
  Tema: Pines digitales de salida
  Fecha: 20180627
  Objetivo: Detección de los pines de salida útiles
  Material: placa Arduino o ESP

  Descripción:
  Las placas Arduino tienen muy bien definidos los pines y sus características.
  En las placas ESP no ocurre esto.
  Tenemos pines identificados, pero que no podemos utilizar como pines digitales de salida.
  Para saber cuáles podemos usar y cuáles no, desarrollamos un programa que hace uso de la comunicación serie.

  El programa recibe por el puerto serie un valor numérico entero que asume que es el pin a activar.
  Decodifica el valor e informa para confirmar.
  Configura el pin como salida y lo hace parpadear (cambiará su estado) contínuamente hasta recibir un nuevo valor.

  Con este programa y un simple led que iremos colocando en los pines que deseemos probar, podemos detectar si funcionan correctamente.
*/

//Pin que estamos haciendo parpadear. Inicialmente ninguno físico.
byte pin=255;

void setup()
{
  Serial.begin(115200);
  Serial.println("Running");
}

void loop()
{
  //Si hay algo en el buffer de entrada del puerto serie...
  if(Serial.available())
  {
    //...esperaremos un momento más para que se reciba todo el texto
    delay(100);
    //Intentamos obtener un número entero
    pin=Serial.parseInt();
    //Configuramos el pin como salida digital
    pinMode(pin,OUTPUT);ç
    //Informamos
    Serial.println("Blinking pin "+String(pin));
  }
  //Cambiamos el estado al pin
  digitalWrite(pin,!digitalRead(pin));
  delay(300);
}
