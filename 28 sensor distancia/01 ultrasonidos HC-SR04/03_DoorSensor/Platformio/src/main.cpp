/*
  Autor: Ramón Junquera
  Fecha: 20200218
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

//Declaración de constantes globales
const byte pinTrig=27; //Pin del trigger
const byte pinEcho=14; //Pin del echo
const byte pinBuzzer=13; //Pin del buzzer
const uint16_t alertTime=10; //Número de segundos para que salte la alarma
const byte displaySDA=4,displaySCL=15,displayRES=16; //Pinout de display integrado

//Declaración de variable globales
RoJoSSD1306 display; //Objeto de gestión de display
RoJoSprite v(1); //Sprite monocromo que utilizaremos como memoria de vídeo
uint16_t alertCount=0; //Número de alertas (más de 30 seg)
uint16_t openCount=0; //Número de veces que se ha abierto la puerta
uint16_t openTime=0; //Tiempo total de puerta abierta en segundos
bool alertON=false; //Estamos en modo alarma?
uint32_t openStart=0; //Tiempo de inicio de puerta abierta
uint16_t lastValue; //Último valor mostrado
uint16_t currentDistance=10; //Distancia actual a la puerta
uint16_t lastDistance=10; //Última medida de distancia

//Devuelve la distancia del HC-SR04 en centímetros
uint16_t getDistance() {
  digitalWrite(pinTrig,LOW); //Comenzamos desactivando el sensor
  delayMicroseconds(5); //Esperamos un momento
  digitalWrite(pinTrig,HIGH); //Activamos el pulso de ultrasonidos
  delayMicroseconds(10); //Esperamos un instante para que llegue el eco
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

String twoDigits(byte a) {
  //Convierte el valor a una cadena de 2 dígitos
  String tmp="0"+String(a);
  return tmp.substring(tmp.length()-2,tmp.length());
}

//Paramos el buzzer y mostramos la pantalla de OK
void showOK() {
  digitalWrite(pinBuzzer,LOW); //Apagamos el buzzer
  v.clear();
  v.drawSprite("/ok.spr",25,0);

  //Dibujamos el texto informativo en formato hh:mm:ss a/o
  String info=twoDigits(openTime/3600)+":"+twoDigits((openTime/60)%60)+":"+twoDigits(openTime%60)+" "+String(alertCount)+"/"+String(openCount);
  v.printOver("/10x15.fon",info,{0,0,1},0,48);
  display.drawSprite(&v);

  //El último valor mostrado no es 0
  //Sí la puerta se vuelve a abrir en menos de un segundo, queremos que se refresque la
  //pantalla. Si el último valor es 0, pensará que ya lo está mostrando.
  lastValue=9;
}

//Muestra el contador hacia atrás con el último valor
void showTimer() {
  v.clear();
  v.printOver("/21x31d.fon",String(lastValue),{0,0,1},50,10);
  display.drawSprite(&v);
}

void setup() { 
  Serial.begin(115200); //DEBUG
  pinMode(pinTrig,OUTPUT); //Definimos el pin del trigger como salida (generación del pulso de ultrasonidos)
  pinMode(pinEcho,INPUT); //Definimos el pin del echo como entrada (tiempo de rebote del ultrasonido)
  pinMode(pinBuzzer,OUTPUT); //Definimos el pin del buzzer como salida
  display.begin(displaySDA,displaySCL,displayRES); //Init display
  v.setSize(display.xMax(),display.yMax()); //Dimensionamos memoria de vídeo
  showOK(); //Comenzamos mostrando la pantalla de OK
}

void loop(void) {
  //La distancia obtenida de este sensor no es muy fiable, porque de vez en
  //cuando lee 0 cuando no es cierto.
  //Para evitar lecturas falsas, sólo tendremos en cuenta una medida
  //cuando en dos ciclos consecutivos obtengamos el mismo valor.
  uint16_t d=getDistance(); //Obtenemos la distancia actual
  //Si es igual que la medida anterior...la tenemos en cuenta
  if(d==lastDistance) currentDistance=d;
  lastDistance=d; //La última medida será la actual

  uint32_t now=millis(); //Obtenemos el tiempo actual
  
  if(alertON) { //Si estamos en modo alarma...
    if(currentDistance==0) { //Si la puerta está abierta...
      //Seguimos mostrando el tiempo de apertura
      uint16_t v=(now-openStart)/1000; //Calculamos el valor a mostrar
      if(v!=lastValue) { //Si el valor es distinto al último mostrado...
        //...tenemos que refrescarlo
        lastValue=v; //El último valor será el actual
        showTimer(); //Mostramos el timer
        if(lastValue>alertTime) { //Si llevamos más tiempo del permitido...
          //...tenemos que hacer sonar la alarma
          digitalWrite(pinBuzzer,HIGH); //Activamos el buzzer
        }
      } else { //El valor es el mismo que antes
        //No lo refrescamos
        if((now-openStart)%1000>500) { //Si el buzzer lleva activo más de 500ms...
          digitalWrite(pinBuzzer,LOW); //...lo apagamos
        }
      }
    }
    else { //Si la puerta está cerrada...
      openCount++; //Incrementamos el número de veces que se ha abierto la puerta
      openTime+=lastValue; //Incrementamos el tiempo total de puerta abierta (en segundos)
      if(lastValue>alertTime) { //Si llevábamos más tiempo del permitido con la puerta abierta...
        alertCount++; //...incrementamos el número de veces que ha saltado la alarma
      }
      alertON=false; //Desactivamos el modo alarma
      showOK(); //Mostramos la pantalla de Ok
    }
  } else { //Si no estamos en modo alarma...
    if(currentDistance==0) { //Si la puerta está abierta...
      openStart=now; //Anotamos que la puerta se ha abierto ahora
      alertON=true; //...pasamos a modo alarma
    }
  }
  //Evitamos errores de watchdog
  delay(1);
}