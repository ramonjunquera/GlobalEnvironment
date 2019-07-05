/*
 * Autor: Ram�n Junquera
 * Descripci�n: Gesti�n chip BCM2835 de Raspberry Pi 3 con comandos de Arduino
 * Versi�n: 20190705
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
 * La librer�a autom�ticamente inicializa la gesti�n a los pines con la
 * instrucci�n ArduinoBegin() al final.
 * Devuelve el resultado en la variable _arduinoBeginAnswer sin
 * comprobarlo.
 * 
 * En teor�a, Un programa siempre deber�a finalizar con la instrucci�n:
 *   ArduinoEnd()
 */

#ifndef RoJoArduino_h
#define RoJoArduino_h

#include <iostream> //Para cout (Serial)
#include <unistd.h> //Para geteuid() y usleep()
#include <sys/mman.h> //Para MAP_FAILED
#include <fcntl.h> //Para O_RDWR 
#include <unordered_map> //Para diccionario de interrupciones
#include <list> //Para lista de peticiones PWM
#include <signal.h> //para signal
#include <sys/time.h> //para itimerval
#include <thread> //Para gestionar threads
using namespace std;

//Constantes globales
#define LOW 0
#define HIGH 1
#define PI 3.14159265
//Pin CS de SPI por defecto. Como RPi tiene 2, supondremos que es el primero
#define SS 8 //SPICS0 

//Definici�n de tipos de datos
typedef uint8_t byte;
typedef string String;
typedef enum
{
  INPUT=0,
  OUTPUT=1,
  INPUT_PULLUP=2,
  INPUT_PULLDOWN=3
} arduinoPinMode;
typedef enum
{
  MSBFIRST=true,
  LSBFIRST=false
} arduinoOrder;

//Definici�n de variables globales
uint32_t *bcm2835_peripherals_base; //Direcci�n base de perif�ricos
uint32_t bcm2835_peripherals_size; //Tama�o de memoria de perif�ricos
uint32_t *bcm2835_peripherals = (uint32_t *)MAP_FAILED; //Puntero a registros de perif�ricos
volatile uint32_t *bcm2835_gpio = (uint32_t *)MAP_FAILED;
volatile uint32_t *bcm2835_pwm = (uint32_t *)MAP_FAILED;
volatile uint32_t *bcm2835_clk = (uint32_t *)MAP_FAILED;
volatile uint32_t *bcm2835_pads = (uint32_t *)MAP_FAILED;
volatile uint32_t *bcm2835_spi0 = (uint32_t *)MAP_FAILED;
volatile uint32_t *bcm2835_bsc0 = (uint32_t *)MAP_FAILED;
volatile uint32_t *bcm2835_bsc1 = (uint32_t *)MAP_FAILED;
volatile uint32_t *bcm2835_st	= (uint32_t *)MAP_FAILED;

//Clase para simulaci�n de puerto serie
class SerialClass
{
  public:
  
  //Inicializaci�n de puerto serie
  void begin(uint32_t baud)
  {
    //No te tiene en cuenta el valor de la velocidad
  }
  
  //Enviar una l�nea con marca de final
  void println(uint64_t v)
  {
    cout << v << endl;
  }
  void println(int64_t v)
  {
    cout << v << endl;
  }
  void println(String v="")
  {
    cout << v << endl;
  }
  
  //Enviar una l�nea sin marca de final
  void print(uint64_t v)
  {
    cout << v << endl;
  }
  void print(int64_t v)
  {
    cout << v << endl;
  }
  void print(String v)
  {
    cout << v << endl;
  }
};
//Creamos el objeto Serial
SerialClass Serial;

//Inicializaci�n de acceso los registros del chip BCM2835
//Devuelve false ante cualquier cualquier error
bool ArduinoBegin()
{
  //Definici�n de variables
  FILE *fp; //Puntero de archivo (file pointer)
  byte b[4]; //Buffer
  int memfd; //Identificador de interface de acceso a memoria (file descriptor)

  //Debemos averiguar la direcci�n base de la memoria de perif�ricos y
  //su tama�o. Esta informaci�n se encuentra en el archivo binario
  // /proc/device-tree/soc/ranges
  //A partir del byte 4 tenemos la direcci�n base y a continuaci�n su
  //tama�o. Ambos datos tienen una logitud de 4 bytes.
  //Est�n escritos de mayor a menor peso, as� que no podemos leerlos
  //directamente a un uint32_t. Tenemos que invertir el orden de los
  //bytes.

  //Abrimos el archivo ranges en binario y como s�lo lectura
  fp = fopen("/proc/device-tree/soc/ranges","rb");
  //Si no se ha podido abrir,,,terminamos con error
  if(fp==NULL) return false;
  //Hemos podido abrir el archivo

  //Posicionamos el cursor en la cuarta posici�n: direcci�n base de perif�ricos
  fseek(fp,4,SEEK_SET);
  //Si no podemos leer 4 bytes...terminamos con error
  if (fread(b,1,4,fp) != 4) return false;
  //Hemos podido leer los 4 bytes

  //Invertimos el orden de los bytes leidos y guardamos el resultado
  //en el puntero global definido previamente.
  bcm2835_peripherals_base = (uint32_t *)(b[0] << 24 | b[1] << 16 | b[2] << 8 | b[3] << 0);
  //A continuaci�n tenemos el tama�o de la memoria de perif�ricos
  //tambi�n con una longitud de 4 bytes. Reaprovechamos el buffer
  //Si no podemos leer 4 bytes...terminamos con error
  if (fread(b,1,4,fp) != 4) return false;
  //Hemos podido leer los 4 bytes

  //Asignamos su valor al puntero de direcci�n base de perif�ricos
  //Como antes, invertimos el orden de los bytes y guardamos el tama�o
  //en la variable global previamente definida
  bcm2835_peripherals_size = (b[0] << 24 | b[1] << 16 | b[2] << 8 | b[3] << 0);
  //Hemos terminado con el archivo. Lo cerramos
  fclose(fp);

  //Para acceder a los registros de los perif�ricos podemos abrir la
  //interface /dev/gpiomem o /dev/mem
  //La primera s�lo da acceso a los pines (gpio) y est� a disposici�n
  //de cualquier usuario. La segunda da acceso a toda la memoria
  //(gpio,spi,i2c,pwm, etc), pero se necesitan permisos de root.
  //Utilizaremos la segunda. Por lo tanto siempre ejecutaremos las
  //aplicaciones con permisos de root

  //Si no tenemos permisos de root...terminamos con error
  //La funci�n geteuid devuelve el identificador del usuario que est�
  //ejecutando la aplicaci�n. root siempre es 0
  if(geteuid()) return false;
  //Somos root

  //Abrimos la interface de acceso a los registros de memoria y
  //tomamos su descriptor
  memfd = open("/dev/mem", O_RDWR | O_SYNC);
  //Si el descriptor no es v�lido... terminamos con error
  if(memfd<0) return false;
  //Hemos podido abrir la interface de acceso a memoria

  //Mapeamos los registros de los perif�ricos sobre bcm2835_peripherals
  bcm2835_peripherals = (uint32_t *)mmap(NULL,bcm2835_peripherals_size,(PROT_READ | PROT_WRITE),MAP_SHARED,memfd,(uint32_t)bcm2835_peripherals_base);
  //Ya no necesitamos el acceso a memoria
  close(memfd);
  //Si no se ha podido mapear... terminamos con error
  if(bcm2835_peripherals == MAP_FAILED) return false;
  //Hemos podido mapear la memoria

  //Calculamos la direcci�n de los distintos bloques de registros con
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
void ArduinoEnd()
{
  //Si ya estaba inicializado...
  if(bcm2835_st != MAP_FAILED)
  {
    //Quitamos el mapeo
    munmap(&bcm2835_peripherals,bcm2835_peripherals_size);
    //Eliminamos la �ltima referencia para que quede como se�al
    bcm2835_st = (uint32_t *)MAP_FAILED;
  }
}

//Devuelve el tiempo en microsegundos que lleva en marcha el procesador.
uint64_t micros()
{
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
    
  //Bloqueamos acceso a registros
  __sync_synchronize();
  //Leemos los valores de los registros
  hi = *paddr_st_HI;
  lo = *paddr_st_LO;
  //Desbloqueamos acceso a registros
  __sync_synchronize();
  //Componemos el valor completo de 64 bits con los 2 de 32 bits
  //y lo devolvemos
  return (hi<<32) + lo;
}

//Devuelve el tiempo en milisegundos que lleva en marcha el procesador.
uint64_t millis()
{
  //Llamamos a la func�n micros() y pasamos los microsegundos a
  //milisegundos
  return micros()/1000;
}

//Devuelve el control despu�s de transcurridos los microsegundos indicados.
void delayMicroseconds(uint64_t us)
{
  //Si no se ha inicializado el chip BCM2835...terminamos;
  if(bcm2835_st == MAP_FAILED) return;
  //El chip BCM2835 se ha inicializado

  //Esperamos el n�mero de microsegundos indicado
  usleep(us);
}

//Devuelve el control despu�s de transcurridos los milisegundos indicados.
void delay(uint64_t ms)
{
  //Llamamos a la funci�n delayMicroseconds pasando los milisegundos a
  //microsegundos
  delayMicroseconds(ms*1000);
}

//Configura un pin como entrada o salida.
//Los valores para mode son:
//  0 = INPUT
//  1 = OUTPUT
void _pinMode(byte pin,byte mode)
{
  //El grupo de registros de pines (bcm2836_gpio) comienza con el modo
  //de cada pin.
  //Se necesitan 3 bits para guardar el modo de un pin.
  //Puesto que un registro tiene 4 bytes = 32 bits, se puede guardar
  //el modo de 10 pines en cada registro (10*3=30) despreciando los
  //dos �ltimos bits.

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
  //que corresponden a nuestro pin y en esa posici�n,
  //escribimos el valor del modo
  *pMode = (*pMode & ~(0b111 << shift)) | (mode << shift);
}

//Configura las resistencias de un pin.
//Los valores de resType son:
// 0 - sin resitencias
// 1 - resistencias pull-down
// 2 - resistencias pull-up
void _pinResType(byte pin,byte resType)
{
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
  //Despu�s volvemos a desactivar la m�scara del reloj de resistencias.

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
void pinMode(byte pin,arduinoPinMode mode)
{
  //Si no se ha inicializado el chip BCM2835...terminamos
  if(bcm2835_st == MAP_FAILED) return;
  //El chip BCM2835 se ha inicializado

  switch(mode)
  {
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
void digitalWrite(byte pin,byte value)
{
  //Los registros para asignar el estado a un pin digital dependen del
  //estado.
  //Si queremos asignar un estado LOW, comenzaremos en bcm2835_gpio+10
  //Y si queremos HIGH, comenzamos en bcm2835_gpio+7
  //S�lo tenemos que asignar al registro la m�scara de los pines
  //afectados

  //Si no se ha inicializado el chip BCM2835...terminamos;
  if(bcm2835_st == MAP_FAILED) return;
  //El chip BCM2835 se ha inicializado

  //Creamos el puntero con la direcci�n del registro afectado
  //teniendo en cuenta la acci�n solicitada y el pin
  volatile uint32_t* paddr = bcm2835_gpio + (value==LOW?10:7);
  //Calculamos la m�scara del pin afectado y la aplicamos
  *paddr = (1 << pin);
}

//Lee el estado de un pin digital
byte digitalRead(byte pin)
{
  //Los estados de los pines digitales se son representados por bits
  //en un par de registros consecutivos a partir de la direcci�n
  //  bcm2835_gpio+13
  //Cada uno de ellos muestra el estado de 32 pines.

  //Si no se ha inicializado el chip BCM2835...devolvemos LOW;
  if(bcm2835_st == MAP_FAILED) return LOW;
  //El chip BCM2835 se ha inicializado

  //Calculamos la direcci�n del registro
  volatile uint32_t* paddr = bcm2835_gpio + 13;
  //Calculamos la m�scara, comprobamos que tenemos alguna coincidencia
  //y devolvemos el valor correspondiente
  return (*paddr & (1 << pin))?HIGH:LOW;
}

//Lee el estado de todos los pines digitales
uint32_t digitalRead()
{
  //Es suficiente con leer un registro. No tenemos m�s de 32 pines
  //Esta instrucci�n no pertenece al estandar de Arduino

  //Si no se ha inicializado el chip BCM2835...devolvemos LOW;
  if(bcm2835_st == MAP_FAILED) return LOW;
  //El chip BCM2835 se ha inicializado

  //Calculamos la direcci�n del registro que corresponde a los pines
  volatile uint32_t* paddr = bcm2835_gpio + 13;
  //Devolvemos su contenido
  return *paddr;
}

void shiftOut(byte pinData,byte pinClock,arduinoOrder bitOrder,byte value)
{
  //Si no se ha inicializado el chip BCM2835...terminamos;
  if(bcm2835_st == MAP_FAILED) return;
  //El chip BCM2835 se ha inicializado

  if(bitOrder==MSBFIRST)
  {
    for(int i=7;i>=0;i--)
    {
      digitalWrite(pinData,value & (1 << i));
      digitalWrite(pinClock,HIGH);
      digitalWrite(pinClock,LOW);
    }
  }
  else
  {
    for(int i=0;i<8;i++)
    {
      digitalWrite(pinData,value & (1 << i));
      digitalWrite(pinClock,HIGH);
      digitalWrite(pinClock,LOW);
    }
  }
}

string F(string s)
{
  //En Arduino se utiliza la funci�n F para incluir el texto constante
  //del par�metro en la memoria del programa y no en la de variables
  //En Linux no es necesario. La funci�n s�lo se define para guardar
  //compatibilidad.
  //Devolvemos el texto tal cual
  return s;
}

void yield()
{
  //En Arduino, la familia ESP8266 utiliza esta funci�n para
  //refrescar los procesos de fondo. Si no se hace se puede
  //bloquear el programa.
  //En Linux no es necesario. La funci�n s�lo se define para guardar
  //compatibilidad
}

//Funciones para interrupciones

//Puesto que en Linux no se gestionan demasiado bien las interrupciones
//por hardware, se ha optado por utilizar una funci�n propia dentro de un 
//thread que se encarga de comprobar cont�nuamente el estado de los 
//pines. Esto libera el timer para otros usos.
//Si no se monitoriza ning�n pin, el thread finaliza autom�ticamente.
//Cuando se crea un nueva monitorizaci�n de pin con attachInterrupt se
//toma nota de la funci�n a la que se llamar�.
//La estructura que mantiene toda la informaci�n de una interrupci�n es
//interruptionStruct.
//Las m�scaras de los pines con interrupci�n se guardan en
//edgeRisingMask y edgeFallingMask.
//Desde el thread, cuando se lanza una funci�n de interrupci�n tambi�n 
//se usa otro thread.
//Todos los threads se desvinculan (detach) para no tener que hacer join.

//Tipos de interrupciones por hardware.
typedef enum
{
  CHANGE=3,
  RISING=2,
  FALLING=1
} arduinoEdgeType;

//Estructura con la informaci�n de una interrupci�n
struct interruptionStruct
{
  //Puntero a funci�n de interrupci�n.
  //Siempre tiene dos par�metros: el pin y el estado
  //El estado es 0 o 1
  void (*f)(byte,byte); 
  //La funci�n se est� ejecutando?. Por defecto no.
  bool running=false; 
};

//Estructura con la informaci�n de todas las interrupciones
struct interruptionsStruct
{
  //Diccionario de interrupciones
  //La key es el pin
  //El dato es la estructura de interrupci�n
  unordered_map<byte,interruptionStruct> interruptionDicc;
  //M�scara de interrupciones RISING
  uint32_t edgeRisingMask=0; //Inicialmente ning�n pin con interrupci�n
  //M�scara de interrupciones FALLING
  uint32_t edgeFallingMask=0; //Inicialmente ning�n pin con interrupci�n
};

//Creamos un objeto para mantener todas la interrupciones
interruptionsStruct myInterruptions;

//Funci�n que lanza la funci�n f(pin,status) y anota en running cuando termina.
void InterruptionFunctionLauncher(bool *running,void (*f)(byte,byte),byte pin,byte status)
{
  //Debe ser llamada como thread.
  //Es obligatorio para poder controlar cu�ndo finaliza la funci�n

  //Llamamos a la funci�n
  f(pin,status);
  //Hemos terminado de ejecutar la funci�n
  //Ya no est� en ejecuci�n
  *running=false;
}

//Funci�n para el bucle principal de ejecuci�n
void InterruptionThreadLoop()
{
  //Debe ser llamada como thread
  //Utiliza el objeto myInterruptions definido previamente

  //Estado de pines anterior es el que tenemos ahora mismo
  uint32_t lastStatus=digitalRead();
  //Estado actual de los pines
  uint32_t currentStatus;
  //M�scara de pines que han sufrido cambios
  uint32_t changes;
  //M�scara del pin procesado
  uint32_t maskPin;
  //M�scara de interrupciones
  uint32_t maskInterrupt;

  //Mientras haya alg�n nodo en el diccionario...
  while(myInterruptions.interruptionDicc.size()>0)
  {
    //Leemos el estado actual de los pines
    currentStatus=digitalRead();
    //Calculamos la m�scara de pines que han sufrido cambios
    changes=currentStatus ^ lastStatus;
    //Si hay alg�n cambio...
    if(changes)
    {
      //Calculamos la m�scara de pines que deben hacer saltar la interrupci�n.
      //Los que:
      //han sufrido cambios (changes) y adem�s han pasado a estado HIGH y ten�an activo el edge RISING (currentStatus & edgeRising)
      //o
      //han sufrido cambios (changes) y adem�s han pasado a estado LOW y ten�an activo el edge FALLING (~currentStatus & edgeFalling)
      maskInterrupt = changes & ((currentStatus & myInterruptions.edgeRisingMask) | (~currentStatus & myInterruptions.edgeFallingMask));
      //Si debe saltar alguna interrupci�n...
      if(maskInterrupt)
      {
        //Recorremos todas las entradas del diccionario con un iterator
        for(auto &itr : myInterruptions.interruptionDicc)
        {
          //Calculamos la m�scara del pin
          maskPin = 1 << itr.first;
          //Si el pin actual debe hacer saltar la interrupci�n...
          if(maskInterrupt & maskPin)
          {
            //Si la funci�n no est� en ejecuci�n...
            if(!itr.second.running)
            {
              //Anotamos que el thread ya est� en ejecuci�n
              itr.second.running=true;
              //Ejecutamos la funci�n asociada con un thread
              //Le enviamos como par�metros el puntero de la variable running del nodo,
              //el puntero de la funci�n del nodo, el pin procesado y su estado actual
              //Ejecutamos la funci�n pasando como par�metro sus datos
              thread thread1(InterruptionFunctionLauncher,&(itr.second.running),itr.second.f,itr.first,(currentStatus & maskPin?HIGH:LOW)); 
              //Desvinculamos el thread del programa principal
              thread1.detach();
            }
          }
        }
      }
      //Consideramos el estado actual como �ltimo
      lastStatus=currentStatus;
    }
  }
}

//Elimina los eventos de un pin
void detachInterrupt(byte pin)
{
  //Calculamos la m�scara
  uint32_t mask=(1<<pin);
  //Si ya existe un evento para este pin...
  if(myInterruptions.interruptionDicc.count(pin))
  {
    //...lo eliminaremos
    //Eliminamos cualquier evento RISING
    myInterruptions.edgeRisingMask &= ~mask;
    //Eliminamos cualquier evento FALLING
    myInterruptions.edgeFallingMask &= ~mask;
    //Eliminamos el pin del diccionario
    myInterruptions.interruptionDicc.erase(pin);
    //Si el diccionario ha quedado vac�o, ya terminar� su ejecuci�n
    //No nos preocupamos por ello
  }
}

//Define la funci�n a la que se debe llamar ante un cambio de estado
//de un pin mediante una interrupci�n
//El segundo par�metro espera una funci�n que no devuelve nada y que
//tiene dos par�metros: el pin y su estado actual
void attachInterrupt(byte pin,void (*f)(byte,byte),arduinoEdgeType mode)
{
  //Si no se ha inicializado el chip BCM2835...terminamos
  if(bcm2835_st == MAP_FAILED) return;
  //El chip BCM2835 se ha inicializado

  //Eliminamos cualquier evento de este pin (si es que hay alguno)
  detachInterrupt(pin);

  //Creamos una estructura de interrupci�n
  interruptionStruct i;
  //Asignamos la funci�n
  i.f = f;
  //Creamos un nuevo nodo en el diccionario con los datos de la 
  //interrupci�n
  myInterruptions.interruptionDicc.emplace(pin,i);
  //Calculamos la m�scara
  uint32_t mask= (1<<pin);
  //Si es de FALLING (o CHANGE)...activamos el bit en la m�scara de FALLING
  if(mode & FALLING) myInterruptions.edgeFallingMask |= mask;
  //Si es de RISING (o CHANGE)...activamos el bit en la m�scara de RISING
  if(mode & RISING) myInterruptions.edgeRisingMask |= mask;
  //Si el diccionario s�lo tiene el nodo que acabamos de crear...
  if(myInterruptions.interruptionDicc.size()==1)
  {
    //...es que ha sido el primero. Tendremos que lanzar el thread
    //del loop ahora.

    //Lanzamos el thread a la funci�n principal de gesti�n de
    //interrupciones
    thread thread1(InterruptionThreadLoop);
    //Desvinculamos el thread que controla el bucle principal para
    //no tener que preocuparnos por el join. Sabemos que cuando el
    //diccionario est� vac�o, finalizar� el thread
    thread1.detach();
  }
}

//Funciones para PWM

//Seg�n las especificaciones del procesador BMC2835, es capaz de hacer PWM
//en uno de sus pines. Esta funcionalidad es bastante pobre comparada
//con las prestaciones de cualquier placa Arduino o ESP.
//Crearemos esta funcionalidad por software para poder ser aplicada a
//cualquiera de los pines.
//De la misma manera que se hace con las interrupciones, se crear� un
//thread para esto.
//El thread s�lo existir� mientras haya al menos un pin con PWM activo.

//Estructura con la informaci�n de la gesti�n de PWMs
struct pwmsStruct
{
  //Diccionario de los pines y valores PWM
  //La key es el pin
  //El dato es el valor PWM asignado al pin
  unordered_map<byte,byte> pwmDicc;
  //Tiempo en microsegundos entre ticks (periodo)
  //Est� relacionado con la frecuencia de PWM
  //Puesto que estamos utilizando un utin16_t para mantener el periodo entre
  //ticks, estamos limitando la frecuencia m�nima a:
  //  1000000/256/65535=0.0596055~0.06Hz
  uint16_t period=0; //Inicialmente utilizamos la m�xima frecuencia
  //Listado de peticiones pendientes
  //Aunque es un uint16_t guarda dos valores de tipo byte:
  //- En el byte alto guarda el pin
  //- En el byte bajo garda el valor PWM
  list<uint16_t> pwmRequests;
  //El thread est� en marcha?
  bool running=false;
};

//Creamos un objeto para mantener la gesti�n de PWM
pwmsStruct myPWMs;


//Asigna una nueva frecuencia de PWM en Hz
//Afecta a todos los pines
void freqPWM(float freq)
{
  //La m�nima frecuencia permitida es de 0.06Hz
  //Esta funci�n permitir� ajustar la frecuencia a la utilizada en servomotores

  //Debemos calcular el periodo em microsegundos
  //El periodo es el inverso de la frecuencia
  //Puesto que nos dan la frecuencia en Hz el perido lo obtenemos en segundos
  //Debemos multiplicar por 1E6 para pasarlo a microsegundos
  //Adem�s tenemos una resoluci�n de 256 niveles de PWM as� que tendremos que
  //dividirlo por 256 para calcular el periodo de un tick
  //period=1/freq*1000000/256;
  myPWMs.period=3906.25/freq;
}

//Funci�n para el bucle principal de ejecuci�n
void pwmThreadLoop()
{
  //Debe ser llamada como thread
  //Utiliza el objeto myPWMs definido previamente

  //Mientras haya pines activos o peticiones pendientes...
  while(myPWMs.pwmDicc.size()+myPWMs.pwmRequests.size()>0)
  {
	//...lanzaremos un ciclo completo
	//Al comenzar un ciclo incluimos las peticiones pendientes
	//Mientras haya peticiones pendientes  
    while(myPWMs.pwmRequests.size())
    {
	  //Obtenemos la referencia al primer nodo con un iterator  
	  list<uint16_t>::iterator itr = myPWMs.pwmRequests.begin();
	  //Desglosamos su contenido
	  byte pin=(*itr)>>8;
	  byte value=(*itr)&0xFF;
	  //Si ya existe el pin en el diccionario...
	  if(myPWMs.pwmDicc.count(pin))
      {
	    //Si la petici�n tiene valor...simplemente lo actualizamos
	    if(value) myPWMs.pwmDicc.at(pin)=value;
	    //Si tiene un valor nulo...
	    else
	    {
		  //Eliminamos el nodo del diccionario
		  myPWMs.pwmDicc.erase(pin);
		  //Ponemos el pin en estado LOW
		  digitalWrite(pin,LOW);
	    }
	  }
	  //Si el pin no existe en el diccionario...
	  else
      {
	    //Configuramos el pin de salida
	    pinMode(pin,OUTPUT);
	    //Si el valor es mayor que cero...creamos un nuevo nodo en el diccionario
	    if(value) myPWMs.pwmDicc.emplace(pin,value);
	    //Si el valor en cero...simplemente apagamos el pin
	    else digitalWrite(pin,LOW);
	  }
	  //Hemos terminado de procesar esta petici�n
	  //La eliminamos de la lista de peticiones
	  myPWMs.pwmRequests.erase(itr);
    }
    //Hemos terminado de procesar todas las peticiones pendientes
    //Si hay pines activos...
    if(myPWMs.pwmDicc.size()>0)
    {
	  //Al inicial un ciclo todos los pines se deben activar
      for(auto &itr : myPWMs.pwmDicc) digitalWrite(itr.first,HIGH);
      //Esperamos un periodo hasta el siguiente tick
      delayMicroseconds(myPWMs.period);
      //Haremos un ciclo completo (excepto el primer tick)
      for(byte tick=1;tick>0;tick++)
      {
	    //Recorremos todos los nodos del diccionario con un iterator
        for(auto &itr : myPWMs.pwmDicc)
          //Si el tick actual coincide con el valor PWM del pin...
          //...el pin se debe apagar
          if(tick-1==itr.second) digitalWrite(itr.first,LOW); 
        //Dormiremos hasta el pr�ximo tick
        delayMicroseconds(myPWMs.period);
      }
	}
  }
  //No hay pines activos ni peticiones pendientes
  //El thread dejar� de estar en marcha
  myPWMs.running=false;
}

//Define la funci�n a la que se debe llamar para hacer cualquier cambio
//de estado PWM a un pin
void analogWrite(byte pin,byte value)
{
  //Si no se ha inicializado el chip BCM2835...terminamos
  if(bcm2835_st == MAP_FAILED) return;
  //El chip BCM2835 se ha inicializado

  //A�adimos un nuevo nodo al final de la lista de peticiones
  //La petici�n ser� procesada en el propio thread al inicio de un ciclo
  myPWMs.pwmRequests.push_back((pin<<8)|value);
  
  //Si el thread no est� en marcha...
  if(!myPWMs.running)
  {
	//Lo ponemos en marcha nosotros
	myPWMs.running=true;
    //Lanzamos el thread a la funci�n principal de gesti�n de PWM
    thread thread1(pwmThreadLoop);
    //Desvinculamos el thread que controla el bucle principal para
    //no tener que preocuparnos por el join. Sabemos que cuando el
    //diccionario est� vac�o, finalizar� el thread
    thread1.detach();
  }
}

//Inicializamos el chip BCM2835 para acceso a los pines, obtenemos el 
//resultado, pero no lo comprobamos
bool _arduinoBeginAnswer=ArduinoBegin();

#endif
