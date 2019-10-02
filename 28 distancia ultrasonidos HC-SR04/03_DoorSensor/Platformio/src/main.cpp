/*
  Autor: Ramón Junquera
  Fecha: 20190515
  Componentes: ESP32 OLED TTGO 16Mb, HC-SR04, buzzer activo
  Descripción:
    Ejemplo de detector de puerta abierta.
    Utilizamos este modelo de placa porque ya tiene integrado el display.
    El dispositivo se pode deelante de una puerta.
    Con el sensor de proximidad se controla si la puerta está abierta o cerrada.
    El sensor de limita a detectar objetos a una distancia máxima de 26cm.
    Esto o hacemos fijando un timeout de 2ms como tiempo que tarda el sonido en
    ir, rebotar y regresar al sensor de ultrasonidos.
    Si el valor de distancia es 0, entendemos que la puerta está abierta porque
    no se ha podido obtener un valor inferior a 26cm.
    Utilizamos un buzzer activo para simplificar el circuito.
    En una de las constantes de configuración se define el tiempo máximo que
    puede estar abierta la puerta sin que salta la alarma (buzzer).
    Cuando la puerta está cerrada, en la pantalla se puede ver un OK, el tiempo
    total que la puerta ha estado abierta, el número de veces que ha saltado la 
    alarma y el número de veces que se ha abierto la puerta.
    Si la puerta está abierta, se muestra un contador en segundos del tiempo
    que la puerta lleva abierta.
*/

#include <Arduino.h>
#include <RoJoSSD1306.h> //Librería de gestión del display SSD1306
#include <RoJoSprite.h> //Librería de gestión de sprites monocromos
#include <RoJoABC.h> //Gestión de fuentes

//Declaración de constantes globales
const byte pinTrig=27; //Pin del trigger
const byte pinEcho=14; //Pin del echo
const byte pinBuzzer=13; //Pin del buzzer
const uint16_t alertTime=10; //Número de segundos para que salte la alarma

//Declaración de variable globales
RoJoSSD1306 display;
uint16_t alertCount=0; //Número de alertas (más de 30 seg)
uint16_t openCount=0; //Número de veces que se ha abierto la puerta
uint16_t openTime=0; //Tiempo total de puerta abierta en segundos
bool alertON=false; //Estamos en modo alarma?
uint32_t openStart=0; //Tiempo de inicio de puerta abierta
uint16_t lastValue; //Último valor mostrado

uint16_t getDistance()
{
  //Devuelve la distancia del HC-SR04 en centímetros

  //Comenzamos desactivando el sensor
  digitalWrite(pinTrig,LOW);
  //Esperamos un momento
  delayMicroseconds(5);
  //Activamos el pulso de ultrasonidos
  digitalWrite(pinTrig,HIGH);
  //Esperamos un instante para que llegue el eco
  delayMicroseconds(10);
  //Apagamos el trigger. Realmente no es necesario, porque ya lo hacemos al principio.
  digitalWrite(pinTrig,LOW);
  //Con pulseIn solicitamos el tiempo entre dos pulsos de un pin
  //Concretamente en el pin del eco
  //Tiempo entre dos pulsos HIGH
  //Con un tiempo de espera máximo de 2000 microsegundos = 2 milisegundos
  //Devuelve el tiempo transcurrido en microsegundos
  //Calculamos la distancia multiplicando la constante por el tiempo transcurrido entre dos pulsos
  //El resultado vendrá en centímetros
  //Con una limitación de 2ms, no se tendrán en cuenta distancias superiores a 26cm (aprox)
  //pero es más que suficiente para el ejemplo actual
  return 0.01715*pulseIn(pinEcho,HIGH,2000);
}

String twoDigits(byte a)
{
  //Convierte el valor a una cadena de 2 dígitos
  String tmp="0"+String(a);
  return tmp.substring(tmp.length()-2,tmp.length());
}

void showOK()
{
  //Paramos el buzzer y mostramos la pantalla de OK

  //Apagamos el buzzer
  digitalWrite(pinBuzzer,LOW);

  //Variables
  RoJoSprite tmpSprite;
  RoJoABC font;

  //Limpiamos pantalla
  display.videoMem->clear();
  
  //Dibujamos el OK
  tmpSprite.load("/ok.spr");
  display.videoMem->drawSpritePage((128-tmpSprite.width())/2,0,&tmpSprite,4);

  //Dibujamos el texto informativo en formato hh:mm:ss a/o
  String info=twoDigits(openTime/3600)+":"+twoDigits((openTime/60)%60)+":"+twoDigits(openTime%60)+" "+String(alertCount)+"/"+String(openCount);
  font.print("/10x15.fon",info,&tmpSprite);
  display.videoMem->drawSpritePage((128-tmpSprite.width())/2,6,&tmpSprite,4);

  //Refrescamos pantalla
  display.show();
  //Borramos el sprite utilizado
  tmpSprite.clear();

  //El último valor mostrado no es 0
  //Sí la puerta se vuelve a abrir en menos de un segundo, queremos que se refresque la
  //pantalla. Si el último valor es 0, pensará que ya lo está mostrando.
  lastValue=9;
}

void showTimer()
{
  //Muestra el contador hacia atrás con el último valor

  //Variables
  RoJoSprite tmpSprite;
  RoJoABC font;

  //Limpiamos pantalla
  display.videoMem->clear();
  
  //Dibujamos el valor
  String info=twoDigits(openTime/3600)+":"+twoDigits((openTime/60)%60)+":"+twoDigits(openTime%60)+" "+String(alertCount)+"/"+String(openCount);
  font.print("/21x33d.fon",String(lastValue),&tmpSprite);
  display.videoMem->drawSpritePage((128-tmpSprite.width())/2,2,&tmpSprite,4);

  //Refrescamos pantalla
  display.show();
  //Borramos el sprite utilizado
  tmpSprite.clear();
}

void setup()
{ 
  //Activamos el puerto serie para mensajes de debug
  Serial.begin(115200);

  //Definimos el pin del trigger como salida (generación del pulso de ultrasonidos)
  pinMode(pinTrig,OUTPUT);
  //Definimos el pin del echo como entrada (tiempo de rebote del ultrasonido)
  pinMode(pinEcho,INPUT);
  //Definimos el pin del buzzer como salida
  pinMode(pinBuzzer,OUTPUT);
  //Inicializamos el display con los pines en los que está conectado la placa
  //En este modelo no son seleccionables y no coinciden con los estandar:
  //void begin(byte pinCS_SD,byte pinSDA,byte pinSCL,byte pinRST);
  //Nota es este ejemplo no se utiliza una tarjeta SD para almacenar los archivos
  //por lo tanto el pinCS_SD no se tiene en cuenta. Podríamos escribir cualquier valor
  display.begin(SS,4,15,16);
  //Comenzamos mostrando la pantalla de OK
  showOK();
}

void loop(void)
{
  //Obtenemos la distancia actual
  uint16_t d=getDistance();
  //Obtenemos el tiempo actual
  uint32_t now=millis();
  
  //Si estamos en modo alarma...
  if(alertON)
  {
    //Si la puerta está abierta...
    if(d==0)
    {
      //Seguimos mostrando el tiempo de apertura
      //Calculamos el valor a mostrar
      uint16_t v=(now-openStart)/1000;
      //Si el valor es distinto al último mostrado...
      if(v!=lastValue)
      {
        //...tenemos que refrescarlo
        //El último valor será el actual
        lastValue=v;
        //Mostramos el timer
        showTimer();
        //Si llevamos más tiempo del permitido...
        if(lastValue>alertTime)
        {
          //...tenemos que hacer sonar la alarma
          //Activamos el buzzer
          digitalWrite(pinBuzzer,HIGH);
        }
      }
      else //El valor es el mismo que antes
      {
        //No lo refrescamos
        //Si el buzzer lleva activo más de 500ms...
        if((now-openStart)%1000>500)
        {
          //...lo apagamos
          digitalWrite(pinBuzzer,LOW);
        }
      }
    }
    else //Si la puerta está cerrada...
    {
      //Incrementamos el número de veces que se ha abierto la puerta
      openCount++;
      //Incrementamos el tiempo total de puerta abierta (en segundos)
      openTime+=lastValue;
      //Si llevábamos más tiempo del permitido con la puerta abierta...
      if(lastValue>alertTime)
      {
        //...incrementamos el número de veces que ha saltado la alarma
        alertCount++;
      }
      //Desactivamos el modo alarma
      alertON=false;
      //Mostramos la pantalla de Ok
      showOK();
    }
  }
  else //Si no estamos en modo alarma...
  {
    //Si la puerta está abierta...
    if(d==0)
    {
      //Anotamos que la puerta se ha abierto ahora
      openStart=now;
      //...pasamos a modo alarma
      alertON=true;
    }
  }
  //Evitamos errores de watchdog
  delay(1);
}