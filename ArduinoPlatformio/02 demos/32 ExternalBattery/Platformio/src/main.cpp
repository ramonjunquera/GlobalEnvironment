
/*
  Tema: Medición de voltaje de batería
  Fecha: 20211121
  Autor: Ramón Junquera

  Objetivo:
  Analizar la descarga de la batería en el tiempo.
  Cada 5 minutos se enviará el valor leido por el pin ADC a un bot de Telegram.
  Gracias a que con el mensaje se indica la hora de envío, podremos tomar los
  valores y sus horas y hacer una estadística.
  Podremos obtener información como:
  - Valor ADC de batería en carga
  - Valor ADC de batería con máximo carga
  - Valor ADC de batería mínima para que funcione el dispositivo
  - Linealidad de la descarga
  - Consumo medio por unidad de tiempo
  - Voltaje a partir del cual se puerde la linealidad de la descarga
  - etc...

  Notas:
  - Es obligatorio conectar con resistencias el pin ADC, tanto con la batería como con tierra.
  - El servicio de WiFi inutiliza los pines ADC2. Por eso utilizamos el 35 = ADC1-7
  - Utilizamos Deep Sleep para aumentar la duración de la batería
  - Desconectaremos el pin ADC cuando no se utilice para reducir la pérdidas
  
  Resultados para LOLIN32LITE:
  - Valor ADC de batería con máxima carga: 3714 = 3714/819 = 4.53V
  - Autonomía estimada: 5,7 días
*/

#include <Arduino.h>
#include <WiFi.h> //Librería para gestión de wifi para ESP32
#include <RoJoTelegramBot.h> //Librería para gestión de bots de Telegram
#include <driver/rtc_io.h> //Para rtc_gpio_isolate

//Definición de constantes globales
const char ssid[]="xxx"; //Nombre del punto de acceso (SSID)
const char password[]="xxx"; //Contraseña
const String botToken="1234567877:AAFKRJ1Oca9kFvJTHfCt31B3sdKwaIaQt2E"; //Token del bot
const byte pinADC=35;
#ifdef ARDUINO_LOLIN32LITE
  const byte pinLed=22;
#else
  const byte pinLed=BUILTIN_LED;
#endif

//Definición de variables globales
RoJoTelegramBot bot; //Objeto de gestión del bot

void setup() {
  Serial.begin(115200);
  bot.begin(botToken); //Inicializamos el bot
  pinMode(pinLed,OUTPUT); //Configuramos el pin del led como salida
  digitalWrite(pinLed,LOW); //Encendemos el led
  WiFi.mode(WIFI_STA); //Fijamos el modo de conexión a un punto de acceso
  WiFi.disconnect(); //Forzamos a su desconexión (por si estaba conectado anteriormente)
  delay(100); //Esperamos un momento a que desconecte
  WiFi.begin(ssid,password); //Pedimos conexión al punto de acceso
  while (WiFi.status() != WL_CONNECTED) delay(100); //Esperamos hasta que conecte el wifi
  //Conexión Wifi establecida
  digitalWrite(pinLed,HIGH); //Apagamos el led

  //Leemos y promediamos valor ADC
  uint16_t analog=0;
  for(byte i=16;i>0;i--) {
    analog+=analogRead(pinADC);
    delay(1);
  }
  analog/=16;

  //Informamos de valor leido
  Serial.println(analog);
  bot.sendMessage("3396975",String(analog)); //Me envía el resultado de ADC

  rtc_gpio_isolate((gpio_num_t)pinADC); //Desactivamos pinADC

  //Desactivamos Wifi
  WiFi.disconnect(true,true); //Desconectamos WiFi, desactivamos WiFi, eliminamos punto de acceso
  
  //Nos preparamos para volver a dormir
  esp_sleep_enable_timer_wakeup(300000000); //Tiempo de siesta en microsegundos = 5min = 5*60*1000000
  esp_deep_sleep_start(); //Entramos en modo Deep Sleep
  //Cuando salgamos del Deep Sleep lo haremos reiniciando
  //El programa nunca superará este punto
}

void loop() {
  //Nada especial que hacer. Nunca llegaremos a este punto
}

