/*
 * Autor: Ramón Junquera
 * Descripción: Gestión chip BCM2835 de Raspberry Pi 2, 3 y 4 con comandos de Arduino
 * Versión: 20220216
 * Compatibilidad: Platformio & Qt
 * 
 * Funciones generales:
 *   bool ArduinoStart()
 *   void ArduinoEnd()
 *   uint64_t micros()
 *   uint64_t millis()
 *   void delayMicroseconds(uint64_t us)
 *   void delay(uint64_t ms)
 *   void pinMode(byte pin,arduinoPinMode mode)
 *   byte digitalRead(byte pin)
 *   uint32_t digitalRead()
 *   void digitalWrite(byte pin,byte value)
 *   void shiftOut(byte pinData,byte pinClock,arduinoOrder bitOrder,byte value)
 *   string F(string s)
 *   yield()
 * Interrupciones:
 *   void attachInterrupt(byte pin,void (*f)(byte,byte),arduinoEdgeType mode)
 *   void detachInterrupt(byte pin)
 * PWM:
 *   void analogWrite(bite pin,byte value)
 *   void freqPWM(float freq)
 * Puerto Serie:
 *   void begin(uint32_t baud)
 *   void println(int v);
 *   void println(String s);
 *   void print(int v);
 *   void print(String s);
 * 
 * Nota:
 * La librería automáticamente inicializa la gestión a los pines con la
 * instrucción ArduinoBegin() al final.
 * Devuelve el resultado en la variable _arduinoBeginAnswer sin
 * comprobarlo.
 * 
 * En teoría, Un programa siempre debería finalizar con la instrucción:
 *   ArduinoEnd()
 */

//#ifndef RoJoArduino_h
//#define RoJoArduino_h
#pragma once

#include <iostream> //Para cout (Serial)
#include <unistd.h> //Para geteuid() y usleep()
#include <sys/mman.h> //Para MAP_FAILED
#include <fcntl.h> //Para O_RDWR 
#include <unordered_map> //Para diccionario de interrupciones
#include <list> //Para lista de peticiones PWM
#include <signal.h> //para signal
#include <sys/time.h> //para itimerval
#include <thread> //Para gestionar threads
#include <string.h> //Para memset
#include <stdarg.h> //Para va_start
using namespace std;

//Constantes globales
#define LOW 0
#define HIGH 1
#define PI 3.14159265
//Pin CS de SPI por defecto. Como RPi tiene 2, supondremos que es el primero
#define SS 8 //SPICS0

//Macros
#define String(a) to_string(a)
#define substring(a) substr(a)

//Definición de tipos de datos
typedef uint8_t byte;
typedef string String;
typedef enum {
  INPUT=0,
  OUTPUT=1,
  INPUT_PULLUP=2,
  INPUT_PULLDOWN=3
} arduinoPinMode;
typedef enum {
  MSBFIRST=true,
  LSBFIRST=false
} arduinoOrder;

//Definición de variables globales
static uint32_t *bcm2835_peripherals_base; //Dirección base de periféricos
static uint32_t bcm2835_peripherals_size; //Tamaño de memoria de periféricos
static uint32_t *bcm2835_peripherals = static_cast<uint32_t*>(MAP_FAILED); //Puntero a registros de periféricos
static volatile uint32_t *bcm2835_gpio = static_cast<uint32_t*>(MAP_FAILED);
static volatile uint32_t *bcm2835_pwm = static_cast<uint32_t*>(MAP_FAILED);
static volatile uint32_t *bcm2835_clk = static_cast<uint32_t*>(MAP_FAILED);
static volatile uint32_t *bcm2835_pads = static_cast<uint32_t*>(MAP_FAILED);
static volatile uint32_t *bcm2835_spi0 = static_cast<uint32_t*>(MAP_FAILED);
static volatile uint32_t *bcm2835_bsc0 = static_cast<uint32_t*>(MAP_FAILED);
static volatile uint32_t *bcm2835_bsc1 = static_cast<uint32_t*>(MAP_FAILED);
static volatile uint32_t *bcm2835_st = static_cast<uint32_t*>(MAP_FAILED);

//Clase para simulación de puerto serie
class SerialClass {
  public:
  
  //Inicialización de puerto serie
  void begin(uint32_t baud) {
    //No se tiene en cuenta el valor de la velocidad porque la salida
    //del puerto serie está redirigida a la pantalla.
    //Hacemos una operación para que no de error de variable no usada
    baud++;
  }
  
  //Enviar una línea con marca de final
  void println(uint64_t v) {
    cout << v << endl;
  }
  void println(int64_t v) {
    cout << v << endl;
  }
  void println(String v="") {
    cout << v << endl;
  }
  
  //Enviar una línea sin marca de final
  void print(uint64_t v) {
    cout << v;
  }
  void print(int64_t v) {
    cout << v;
  }
  void print(String v) {
    cout << v;
  }
};
//Creamos el objeto Serial
SerialClass Serial;

//Inicialización de acceso los registros del chip BCM2835
//Devuelve false ante cualquier cualquier error
bool ArduinoBegin() {
  //Debemos averiguar la dirección base de la memoria de periféricos y su tamaño.
  //La teoría dice que esta información se encuentra en el archivo binario
  // /proc/device-tree/soc/ranges
  //A partir del byte 4 tenemos la dirección base y a continuación su
  //tamaño. Ambos datos tienen una logitud de 4 bytes.
  //Están escritos de mayor a menor peso, así que no podemos leerlos
  //directamente a un uint32_t. Tenemos que invertir el orden de los
  //bytes.
  //Esto se cumple en las RPi 2 y 3, pero no en la 4.
  //Ejemplo de contenido del archivo a partir de byte 4
  //  3F 00 00 00 01 00 00 00
  //Por lo tanto tenemos que la dirección base es 0x3F000000 con una longitud de 0x01000000
  //En la RPi4 encontramos lo siguiente a partir del byte 4:
  //  00 00 00 00 FE 00 00 00
  //Aunque sabemos que la dirección base es 0xFE000000 con la misma longitud: 0x01000000
  //Para simplificar utilizaremos el archivo sólo para diferenciar un modelo de otro.
  //Concretamente con el byte 5: 3F para RPi3 y 00 para RPi4
  //Después tomaremos las direcciones que correspondan sin mirar más.

  //Definición de variables
  FILE *fp; //Puntero de archivo (file pointer)
  byte b; //Buffer
  int memfd; //Identificador de interface de acceso a memoria (file descriptor)

  //Abrimos el archivo ranges en binario y como sólo lectura
  fp = fopen("/proc/device-tree/soc/ranges","rb");
  //Si no se ha podido abrir,,,terminamos con error
  if(!fp) return false;
  //Hemos podido abrir el archivo

  //Posicionamos el cursor en el byte 4 (quinta posición)
  fseek(fp,4,SEEK_SET);
  //Leemos el byte. Si no podemos...terminamos con error
  if (fread(&b,1,1,fp) != 1) return false;
  fclose(fp); //Hemos podido leer el byte. Hemos terminado con el archivo. Lo cerramos

  //Identificamos el modelo y asignamos la dirección base
  bcm2835_peripherals_base = (b>0)?reinterpret_cast<uint32_t*>(0x3F000000):reinterpret_cast<uint32_t*>(0xFE000000);
  bcm2835_peripherals_size = 0x01000000; //El tamaño es constante

  //Para acceder a los registros de los periféricos podemos abrir la
  //interface /dev/gpiomem o /dev/mem
  //La primera sólo da acceso a los pines (gpio) y está a disposición
  //de cualquier usuario. La segunda da acceso a toda la memoria
  //(gpio,spi,i2c,pwm, etc), pero se necesitan permisos de root.
  //Utilizaremos la segunda. Por lo tanto siempre ejecutaremos las
  //aplicaciones con permisos de root

  //Si no tenemos permisos de root...terminamos con error
  //La función geteuid devuelve el identificador del usuario que está
  //ejecutando la aplicación. root siempre es 0
  if(geteuid()) return false;
  //Somos root

  //Abrimos la interface de acceso a los registros de memoria y
  //tomamos su descriptor
  memfd = open("/dev/mem", O_RDWR | O_SYNC);
  //Si el descriptor no es válido... terminamos con error
  if(memfd<0) return false;
  //Hemos podido abrir la interface de acceso a memoria

  //Mapeamos los registros de los periféricos sobre bcm2835_peripherals
  bcm2835_peripherals = static_cast<uint32_t*>(mmap(nullptr,bcm2835_peripherals_size,(PROT_READ | PROT_WRITE),MAP_SHARED,memfd,reinterpret_cast<int32_t>(bcm2835_peripherals_base)));
  close(memfd); //Ya no necesitamos el acceso a memoria
  //Si no se ha podido mapear... terminamos con error
  if(bcm2835_peripherals == MAP_FAILED) return false;
  //Hemos podido mapear la memoria

  //Calculamos la dirección de los distintos bloques de registros con
  //los que trabajaremos
  bcm2835_gpio = bcm2835_peripherals + 0x80000;
  bcm2835_pwm  = bcm2835_peripherals + 0x83000;
  bcm2835_clk  = bcm2835_peripherals + 0x40400;
  bcm2835_pads = bcm2835_peripherals + 0x40000;
  bcm2835_spi0 = bcm2835_peripherals + 0x81000;
  bcm2835_bsc0 = bcm2835_peripherals + 0x81400; //I2C
  bcm2835_bsc1 = bcm2835_peripherals + 0x201000; //I2C
  bcm2835_st   = bcm2835_peripherals + 0xC00;

  //Todo ok
  return true;
}

//Finalizamos el acceso a memoria de chip BCM2835
void ArduinoEnd() {
  if(bcm2835_st != MAP_FAILED) { //Si ya estaba inicializado...
    //Quitamos el mapeo
    munmap(&bcm2835_peripherals,bcm2835_peripherals_size);
    //Eliminamos la última referencia para que quede como señal
    bcm2835_st = static_cast<uint32_t*>(MAP_FAILED);
  }
}

//Devuelve el tiempo en microsegundos que lleva en marcha el procesador.
uint64_t micros() {
  //Si no se ha inicializado el chip BCM2835...devolvemos 0;
  if(bcm2835_st == MAP_FAILED) return 0;
  //El chip BCM2835 se ha inicializado

  //Definimos las direcciones del reloj de sistema
  //Es de 64 bits. Lo forman 2 registros de 32 bits
  volatile uint32_t *paddr_st_HI = bcm2835_st + 2;
  volatile uint32_t *paddr_st_LO = bcm2835_st + 1;
  //Definimos las variables en las que guardaremos los valores de
  //ambos registros
  uint64_t lo,hi;
    
  __sync_synchronize(); //Bloqueamos acceso a registros
  //Leemos los valores de los registros
  hi = *paddr_st_HI;
  lo = *paddr_st_LO;
  __sync_synchronize(); //Desbloqueamos acceso a registros
  //Componemos el valor completo de 64 bits con los 2 de 32 bits
  //y lo devolvemos
  return (hi<<32) + lo;
}

//Devuelve el tiempo en milisegundos que lleva en marcha el procesador.
uint64_t millis() {
  //Llamamos a la función micros() y pasamos los microsegundos a
  //milisegundos
  return micros()/1000;
}

//Devuelve el control después de transcurridos los microsegundos indicados.
void delayMicroseconds(uint32_t us) {
  //Si no se ha inicializado el chip BCM2835...terminamos;
  if(bcm2835_st == MAP_FAILED) return;
  //El chip BCM2835 se ha inicializado

  usleep(us); //Esperamos el número de microsegundos indicado
}

//Devuelve el control después de transcurridos los milisegundos indicados.
void delay(uint32_t ms) {
  //Llamamos a la función delayMicroseconds pasando los milisegundos a
  //microsegundos
  delayMicroseconds(ms*1000);
}

//Configura un pin como entrada o salida.
//Los valores para mode son:
//  0 = INPUT
//  1 = OUTPUT
void _pinMode(byte pin,byte mode) {
  //El grupo de registros de pines (bcm2836_gpio) comienza con el modo
  //de cada pin.
  //Se necesitan 3 bits para guardar el modo de un pin.
  //Puesto que un registro tiene 4 bytes = 32 bits, se puede guardar
  //el modo de 10 pines en cada registro (10*3=30) despreciando los
  //dos últimos bits.

  //Aparte de los modos INPUT y OUTPUT, hay otros modos adicionales 
  //para seleccionar que son pines reservados de comunicaciones
  //(i2c, spi, etc...)

  //Puntero al registro de modo de pin
  volatile uint32_t *pMode;
  //Calculamos el registro que gestiona nuestro pin
  pMode = bcm2835_gpio + pin/10;
  //Calculamos el desplazamiento dentro del registro para 
  byte shift = (pin % 10) * 3;
  //Tomamos el valor actual del registro, borramos los 3 bits
  //que corresponden a nuestro pin y en esa posición,
  //escribimos el valor del modo
  *pMode = (*pMode & static_cast<uint32_t>(~(0b111 << shift))) | static_cast<uint32_t>(mode << shift);
}

//Configura las resistencias de un pin.
//Los valores de resType son:
// 0 - sin resitencias
// 1 - resistencias pull-down
// 2 - resistencias pull-up
void _pinResType(byte pin,byte resType) {
  //Para cambiar el estado de las resistencias debemos seguir los
  //siguientes pasos...
  //Escribimos el tipo de resistencia en el registro bcm2835_gpio+37
  //Activamos los bits correspondiente a los pines a los que afectar�
  //en los relojes de resistencias.
  //Le damos tiempo (10 microsegundos)
  //Desactivamos todos los bits de los relojes de resistencias

  //Cuando se activa un bit de alguno de los registros de reloj de
  //resistencias, internamente se lee el registro bcm2835_gpio+37 y se
  //aplica.
  //Por eso debemos esperar un momento. Para asegurarnos que ha tenido
  //tiempo de reaccionar.
  //Después volvemos a desactivar la máscara del reloj de resistencias.

  //Raspberry Pi tiene 64 pines.
  //Para poder controlarlos todos necesitamos al menos un bit por pin.
  //Como un registro tiene 32 bits, necesitaremos dos registros.
  //Los dos registros de reloj de resistencias son:
  //  bcm2835_gpio+38 y bcm2835_gpio+39.
  //El primero controla los 32 primeros pines y el segundo el resto.

  //Puntero a registro de tipo de resistencias
  volatile uint32_t *pResType = bcm2835_gpio+37;
  //Puntero a registro de reloj de resistencias
  //Calculamos el reloj que le corresponde
  volatile uint32_t *pResClk = bcm2835_gpio + 38 + pin/32;
  //Anotamos el tipo de resistencia a aplicar
  *pResType = resType;
  //Aplicamos la m�scara al reloj con el desplazamiento correcto
  *pResClk = 1 << (pin % 32);
  //Esperamos 10 microsegundos
  delayMicroseconds(10);
  //Limpiamos el reloj de resistencias
  *pResClk = 0;
}

//Configura el modo de un pin
void pinMode(byte pin,arduinoPinMode mode) {
  //Si no se ha inicializado el chip BCM2835...terminamos
  if(bcm2835_st == MAP_FAILED) return;
  //El chip BCM2835 se ha inicializado

  switch(mode) {
    case INPUT:
      //Configuramos el pin como entrada (0=INPUT)
      _pinMode(pin,0);
      //Desactivamos cualquier tipo de resistencias para este pin
      //  0 = sin resistencias
      _pinResType(pin,0);
      break;
    case OUTPUT:
      //Configuramos el pin como salida (1=OUTUT)
      _pinMode(pin,1);
      break;
    case INPUT_PULLUP:
      //Configuramos el pin como entrada (0=INPUT)
      _pinMode(pin,0);
      //Activamos resistencias de pull-up (= 2)
      _pinResType(pin,2);
      break;
    case INPUT_PULLDOWN:
      //Configuramos el pin como entrada (0=INPUT)
      _pinMode(pin,0);
      //Activamos resistencias de pull-down (= 1)
      _pinResType(pin,1);
      break;
    }
}

//Escribe el estado de un pin digital
//Los valores de value son:
//  LOW = 0
//  HIGH = 1
void digitalWrite(byte pin,byte value) {
  //Los registros para asignar el estado a un pin digital dependen del
  //estado.
  //Si queremos asignar un estado LOW, comenzaremos en bcm2835_gpio+10
  //Y si queremos HIGH, comenzamos en bcm2835_gpio+7
  //Sólo tenemos que asignar al registro la máscara de los pines
  //afectados

  //Si no se ha inicializado el chip BCM2835...terminamos;
  if(bcm2835_st == MAP_FAILED) return;
  //El chip BCM2835 se ha inicializado

  //Creamos el puntero con la dirección del registro afectado
  //teniendo en cuenta la acción solicitada y el pin
  volatile uint32_t* paddr = bcm2835_gpio + (value==LOW?10:7);
  //Calculamos la máscara del pin afectado y la aplicamos
  *paddr = (1 << pin);
}

//Lee el estado de un pin digital
byte digitalRead(byte pin) {
  //Los estados de los pines digitales se son representados por bits
  //en un par de registros consecutivos a partir de la dirección
  //  bcm2835_gpio+13
  //Cada uno de ellos muestra el estado de 32 pines.

  //Si no se ha inicializado el chip BCM2835...devolvemos LOW;
  if(bcm2835_st == MAP_FAILED) return LOW;
  //El chip BCM2835 se ha inicializado

  //Calculamos la dirección del registro
  volatile uint32_t* paddr = bcm2835_gpio + 13;
  //Calculamos la máscara, comprobamos que tenemos alguna coincidencia
  //y devolvemos el valor correspondiente
  return (*paddr & (1 << pin))?HIGH:LOW;
}

//Lee el estado de todos los pines digitales
uint32_t digitalRead()
{
  //Es suficiente con leer un registro. No tenemos más de 32 pines
  //Esta instrucción no pertenece al estandar de Arduino

  //Si no se ha inicializado el chip BCM2835...devolvemos LOW;
  if(bcm2835_st == MAP_FAILED) return LOW;
  //El chip BCM2835 se ha inicializado

  //Calculamos la dirección del registro que corresponde a los pines
  volatile uint32_t* paddr = bcm2835_gpio+13;
  //Devolvemos su contenido
  return *paddr;
}

void shiftOut(byte pinData,byte pinClock,arduinoOrder bitOrder,byte value) {
  //Si no se ha inicializado el chip BCM2835...terminamos;
  if(bcm2835_st == MAP_FAILED) return;
  //El chip BCM2835 se ha inicializado

  if(bitOrder==MSBFIRST) {
    for(int i=7;i>=0;i--) {
      digitalWrite(pinData,value & (1 << i));
      digitalWrite(pinClock,HIGH);
      digitalWrite(pinClock,LOW);
    }
  }
  else {
    for(int i=0;i<8;i++) {
      digitalWrite(pinData,value & (1 << i));
      digitalWrite(pinClock,HIGH);
      digitalWrite(pinClock,LOW);
    }
  }
}

string F(string s) {
  //En Arduino se utiliza la función F para incluir el texto constante
  //del parámetro en la memoria del programa y no en la de variables
  //En Linux no es necesario. La función sólo se define para guardar
  //compatibilidad.
  //Devolvemos el texto tal cual
  return s;
}

void yield() {
  //En Arduino, la familia ESP8266 utiliza esta función para
  //refrescar los procesos de fondo. Si no se hace se puede
  //bloquear el programa.
  //En Linux no es necesario. La función sólo se define para guardar
  //compatibilidad
}

//Funciones para interrupciones

//Puesto que en Linux no se gestionan demasiado bien las interrupciones
//por hardware, se ha optado por utilizar una función propia dentro de un 
//thread que se encarga de comprobar contínuamente el estado de los 
//pines. Esto libera el timer para otros usos.
//Si no se monitoriza ningún pin, el thread finaliza automáticamente.
//Cuando se crea un nueva monitorización de pin con attachInterrupt se
//toma nota de la funcion a la que se llamará.
//La estructura que mantiene toda la información de una interrupción es
//interruptionStruct.
//Las máscaras de los pines con interrupción se guardan en
//edgeRisingMask y edgeFallingMask.
//Desde el thread, cuando se lanza una función de interrupción también 
//se usa otro thread.
//Todos los threads se desvinculan (detach) para no tener que hacer join.

//Tipos de interrupciones por hardware.
typedef enum {
  CHANGE=3,
  RISING=2,
  FALLING=1
} arduinoEdgeType;

//Estructura con la información de una interrupción
struct interruptionStruct {
  //Puntero a función de interrupción.
  //Siempre tiene dos parámetros: el pin y el estado
  //El estado es 0 o 1
  void (*f)(byte,byte); 
  //La función se está ejecutando?. Por defecto no.
  bool running=false; 
};

//Estructura con la información de todas las interrupciones
struct interruptionsStruct {
  //Diccionario de interrupciones
  //La key es el pin
  //El dato es la estructura de interrupción
  unordered_map<byte,interruptionStruct> interruptionDicc;
  //Máscara de interrupciones RISING
  uint32_t edgeRisingMask=0; //Inicialmente ningún pin con interrupción
  //Máscara de interrupciones FALLING
  uint32_t edgeFallingMask=0; //Inicialmente ningún pin con interrupción
};

//Creamos un objeto para mantener todas la interrupciones
static interruptionsStruct myInterruptions;

//Función que lanza la función f(pin,status) y anota en running cuando termina.
void InterruptionFunctionLauncher(bool *running,void (*f)(byte,byte),byte pin,byte status) {
  //Debe ser llamada como thread.
  //Es obligatorio para poder controlar cuándo finaliza la función

  //Llamamos a la función
  f(pin,status);
  //Hemos terminado de ejecutar la función
  //Ya no está en ejecución
  *running=false;
}

//Función para el bucle principal de ejecución
void InterruptionThreadLoop() {
  //Debe ser llamada como thread
  //Utiliza el objeto myInterruptions definido previamente

  //Estado de pines anterior es el que tenemos ahora mismo
  uint32_t lastStatus=digitalRead();
  //Estado actual de los pines
  uint32_t currentStatus;
  //Máscara de pines que han sufrido cambios
  uint32_t changes;
  //Máscara del pin procesado
  uint32_t maskPin;
  //Máscara de interrupciones
  uint32_t maskInterrupt;

  //Mientras haya algún nodo en el diccionario...
  while(myInterruptions.interruptionDicc.size()>0) {
    //Leemos el estado actual de los pines
    currentStatus=digitalRead();
    //Calculamos la máscara de pines que han sufrido cambios
    changes=currentStatus ^ lastStatus;
    //Si hay algún cambio...
    if(changes) {
      //Calculamos la máscara de pines que deben hacer saltar la interrupción.
      //Los que:
      //han sufrido cambios (changes) y además han pasado a estado HIGH y tenían activo el edge RISING (currentStatus & edgeRising)
      //o
      //han sufrido cambios (changes) y además han pasado a estado LOW y tenían activo el edge FALLING (~currentStatus & edgeFalling)
      maskInterrupt = changes & ((currentStatus & myInterruptions.edgeRisingMask) | (~currentStatus & myInterruptions.edgeFallingMask));
      //Si debe saltar alguna interrupción...
      if(maskInterrupt) {
        //Recorremos todas las entradas del diccionario con un iterator
        for(auto &itr : myInterruptions.interruptionDicc) {
          //Calculamos la máscara del pin
          maskPin = 1 << itr.first;
          //Si el pin actual debe hacer saltar la interrupción...
          if(maskInterrupt & maskPin) {
            //Si la función no está en ejecución...
            if(!itr.second.running) {
              //Anotamos que el thread ya está en ejecución
              itr.second.running=true;
              //Ejecutamos la función asociada con un thread
              //Le enviamos como parámetros el puntero de la variable running del nodo,
              //el puntero de la función del nodo, el pin procesado y su estado actual
              //Ejecutamos la función pasando como parámetro sus datos
              thread thread1(InterruptionFunctionLauncher,&(itr.second.running),itr.second.f,itr.first,(currentStatus & maskPin?HIGH:LOW)); 
              //Desvinculamos el thread del programa principal
              thread1.detach();
            }
          }
        }
      }
      //Consideramos el estado actual como último
      lastStatus=currentStatus;
    }
  }
}

//Elimina los eventos de un pin
void detachInterrupt(byte pin) {
  //Calculamos la máscara
  uint32_t mask=(1<<pin);
  //Si ya existe un evento para este pin...
  if(myInterruptions.interruptionDicc.count(pin)) {
    //...lo eliminaremos
    //Eliminamos cualquier evento RISING
    myInterruptions.edgeRisingMask &= ~mask;
    //Eliminamos cualquier evento FALLING
    myInterruptions.edgeFallingMask &= ~mask;
    //Eliminamos el pin del diccionario
    myInterruptions.interruptionDicc.erase(pin);
    //Si el diccionario ha quedado vacío, ya terminará su ejecución
    //No nos preocupamos por ello
  }
}

//Define la función a la que se debe llamar ante un cambio de estado
//de un pin mediante una interrupción
//El segundo parámetro espera una función que no devuelve nada y que
//tiene dos parámetros: el pin y su estado actual
void attachInterrupt(byte pin,void (*f)(byte,byte),arduinoEdgeType mode) {
  //Si no se ha inicializado el chip BCM2835...terminamos
  if(bcm2835_st == MAP_FAILED) return;
  //El chip BCM2835 se ha inicializado

  //Eliminamos cualquier evento de este pin (si es que hay alguno)
  detachInterrupt(pin);

  //Creamos una estructura de interrupción
  interruptionStruct i;
  //Asignamos la función
  i.f = f;
  //Creamos un nuevo nodo en el diccionario con los datos de la 
  //interrupción
  myInterruptions.interruptionDicc.emplace(pin,i);
  //Calculamos la máscara
  uint32_t mask= (1<<pin);
  //Si es de FALLING (o CHANGE)...activamos el bit en la máscara de FALLING
  if(mode & FALLING) myInterruptions.edgeFallingMask |= mask;
  //Si es de RISING (o CHANGE)...activamos el bit en la máscara de RISING
  if(mode & RISING) myInterruptions.edgeRisingMask |= mask;
  //Si el diccionario sólo tiene el nodo que acabamos de crear...
  if(myInterruptions.interruptionDicc.size()==1) {
    //...es que ha sido el primero. Tendremos que lanzar el thread
    //del loop ahora.

    //Lanzamos el thread a la función principal de gestión de
    //interrupciones
    thread thread1(InterruptionThreadLoop);
    //Desvinculamos el thread que controla el bucle principal para
    //no tener que preocuparnos por el join. Sabemos que cuando el
    //diccionario está vacío, finalizará el thread
    thread1.detach();
  }
}

//Funciones para PWM

//Según las especificaciones del procesador BMC2835, es capaz de hacer PWM
//en uno de sus pines. Esta funcionalidad es bastante pobre comparada
//con las prestaciones de cualquier placa Arduino o ESP.
//Crearemos esta funcionalidad por software para poder ser aplicada a
//cualquiera de los pines.
//De la misma manera que se hace con las interrupciones, se creará un
//thread para esto.
//El thread sólo existirá mientras haya al menos un pin con PWM activo.

//Estructura con la información de la gestión de PWMs
struct pwmsStruct {
  //Diccionario de los pines y valores PWM
  //La key es el pin
  //El dato es el valor PWM asignado al pin
  unordered_map<byte,byte> pwmDicc;
  //Tiempo en microsegundos entre ticks (periodo)
  //Está relacionado con la frecuencia de PWM
  //Puesto que estamos utilizando un utin16_t para mantener el periodo entre
  //ticks, estamos limitando la frecuencia mínima a:
  //  1000000/256/65535=0.0596055~0.06Hz
  uint16_t period=0; //Inicialmente utilizamos la máxima frecuencia
  //Listado de peticiones pendientes
  //Aunque es un uint16_t guarda dos valores de tipo byte:
  //- En el byte alto guarda el pin
  //- En el byte bajo garda el valor PWM
  list<uint16_t> pwmRequests;
  //El thread está en marcha?
  bool running=false;
};

//Creamos un objeto para mantener la gestión de PWM
static pwmsStruct myPWMs;


//Asigna una nueva frecuencia de PWM en Hz
//Afecta a todos los pines
void freqPWM(float freq) {
  //La mínima frecuencia permitida es de 0.06Hz
  //Esta función permitirá ajustar la frecuencia a la utilizada en servomotores

  //Debemos calcular el periodo em microsegundos
  //El periodo es el inverso de la frecuencia
  //Puesto que nos dan la frecuencia en Hz el perido lo obtenemos en segundos
  //Debemos multiplicar por 1E6 para pasarlo a microsegundos
  //Además tenemos una resolución de 256 niveles de PWM así que tendremos que
  //dividirlo por 256 para calcular el periodo de un tick
  //period=1/freq*1000000/256;
  myPWMs.period=static_cast<uint16_t>(3906.25f/freq);
}

//Función para el bucle principal de ejecución
void pwmThreadLoop() {
  //Debe ser llamada como thread
  //Utiliza el objeto myPWMs definido previamente

  //Mientras haya pines activos o peticiones pendientes...
  while(myPWMs.pwmDicc.size()+myPWMs.pwmRequests.size()>0) {
	//...lanzaremos un ciclo completo
	//Al comenzar un ciclo incluimos las peticiones pendientes
	//Mientras haya peticiones pendientes  
    while(myPWMs.pwmRequests.size()) {
      //Obtenemos la referencia al primer nodo con un iterator  
      list<uint16_t>::iterator itr = myPWMs.pwmRequests.begin();
      //Desglosamos su contenido
      byte pin=(*itr)>>8;
      byte value=(*itr)&0xFF;
      //Si ya existe el pin en el diccionario...
      if(myPWMs.pwmDicc.count(pin)) {
        //Si la petición tiene valor...simplemente lo actualizamos
        if(value) myPWMs.pwmDicc.at(pin)=value;
        //Si tiene un valor nulo...
        else {
        //Eliminamos el nodo del diccionario
        myPWMs.pwmDicc.erase(pin);
        //Ponemos el pin en estado LOW
        digitalWrite(pin,LOW);
        }
      }
      //Si el pin no existe en el diccionario...
      else {
        //Configuramos el pin de salida
        pinMode(pin,OUTPUT);
        //Si el valor es mayor que cero...creamos un nuevo nodo en el diccionario
        if(value) myPWMs.pwmDicc.emplace(pin,value);
        //Si el valor es cero...simplemente apagamos el pin
        else digitalWrite(pin,LOW);
      }
      //Hemos terminado de procesar esta petición
      //La eliminamos de la lista de peticiones
      myPWMs.pwmRequests.erase(itr);
    }
    //Hemos terminado de procesar todas las peticiones pendientes
    //Si hay pines activos...
    if(myPWMs.pwmDicc.size()>0) {
	  //Al inicial un ciclo todos los pines se deben activar
      for(auto &itr : myPWMs.pwmDicc) digitalWrite(itr.first,HIGH);
      //Esperamos un periodo hasta el siguiente tick
      delayMicroseconds(myPWMs.period);
      //Haremos un ciclo completo (excepto el primer tick)
      for(byte tick=1;tick>0;tick++) {
	    //Recorremos todos los nodos del diccionario con un iterator
        for(auto &itr : myPWMs.pwmDicc)
          //Si el tick actual coincide con el valor PWM del pin...
          //...el pin se debe apagar
          if(tick-1==itr.second) digitalWrite(itr.first,LOW); 
        //Dormiremos hasta el próximo tick
        delayMicroseconds(myPWMs.period);
      }
    }
  }
  //No hay pines activos ni peticiones pendientes
  //El thread dejará de estar en marcha
  myPWMs.running=false;
}

//Define la función a la que se debe llamar para hacer cualquier cambio
//de estado PWM a un pin
void analogWrite(byte pin,byte value) {
  //Si no se ha inicializado el chip BCM2835...terminamos
  if(bcm2835_st == MAP_FAILED) return;
  //El chip BCM2835 se ha inicializado

  //Añadimos un nuevo nodo al final de la lista de peticiones
  //La petición será procesada en el propio thread al inicio de un ciclo
  //myPWMs.pwmRequests.push_back(static_cast<byte>(pin<<8)|value);
  myPWMs.pwmRequests.push_back(static_cast<uint16_t>(pin<<8)+value);
  
  //Si el thread no está en marcha...
  if(!myPWMs.running) {
    //Lo ponemos en marcha nosotros
    myPWMs.running=true;
    //Lanzamos el thread a la función principal de gestión de PWM
    thread thread1(pwmThreadLoop);
    //Desvinculamos el thread que controla el bucle principal para
    //no tener que preocuparnos por el join. Sabemos que cuando el
    //diccionario está vacío, finalizará el thread
    thread1.detach();
  }
}

//Inicializamos el chip BCM2835 para acceso a los pines, obtenemos el 
//resultado, pero no lo comprobamos
static bool _arduinoBeginAnswer=ArduinoBegin();

#ifndef QT_GUI_LIB //Si no es una aplicación Qt...
  //Referenciamos las funciones principales definidas en el programa principal
  extern void setup();
  extern void loop();

  //Simulamos la estructura de ejecución típica de Arduino
  int main(int argc, char **argv) {
    setup();
    while(true) {
      loop();
    }
  }
#endif

//#endif
