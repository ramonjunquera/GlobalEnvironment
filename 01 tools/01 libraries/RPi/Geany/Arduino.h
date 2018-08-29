/*
 * Autor: Ram�n Junquera
 * Descripci�n: Gesti�n chip BCM2835 de Raspberry Pi 3 con comandos de Arduino
 * Versi�n: 20180829
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

//#include <iostream> //Para cout en debug
#include <unistd.h> //Para geteuid() y usleep()
#include <sys/mman.h> //Para MAP_FAILED
#include <fcntl.h> //Para O_RDWR 
#include <unordered_map> //Para diccionario de interrupciones
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

bool ArduinoBegin()
{
  //Inicializaci�n de acceso los registros del chip BCM2835
  //Devuelve false ante cualquier cualquier error

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

void ArduinoEnd()
{
  //Finalizamos el acceso a memoria de chip BCM2835

  //Si ya estaba inicializado...
  if(bcm2835_st != MAP_FAILED)
  {
    //Quitamos el mapeo
    munmap(&bcm2835_peripherals,bcm2835_peripherals_size);
    //Eliminamos la �ltima referencia para que quede como se�al
    bcm2835_st = (uint32_t *)MAP_FAILED;
  }
}

uint64_t micros()
{
  //Devuelve el tiempo en microsegundos que lleva en marcha el
  //procesador.
	
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

uint64_t millis()
{
  //Devuelve el tiempo en milisegundos que lleva en marcha el
  //procesador.

  //Llamamos a la func�n micros() y pasamos los microsegundos a
  //milisegundos
  return micros()/1000;
}

void delayMicroseconds(uint64_t us)
{
  //Devuelve el control despu�s de transcurridos los microsegundos
  //indicados.

  //Si no se ha inicializado el chip BCM2835...terminamos;
  if(bcm2835_st == MAP_FAILED) return;
  //El chip BCM2835 se ha inicializado

  //Esperamos el n�mero de microsegundos indicado
  usleep(us);
}

void delay(uint64_t ms)
{
  //Devuelve el control despu�s de transcurridos los milisegundos
  //indicados.

  //Llamamos a la funci�n delayMicroseconds pasando los milisegundos a
  //microsegundos
  delayMicroseconds(ms*1000);
}

void _pinMode(byte pin,byte mode)
{
  //Configura un pin como entrada o salida.
  //Los valores para mode son:
  //  0 = INPUT
  //  1 = OUTPUT

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

void _pinResType(byte pin,byte resType)
{
  //Configura las resistencias de un pin.
  //Los valores de resType son:
  // 0 - sin resitencias
  // 1 - resistencias pull-down
  // 2 - resistencias pull-up

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

void pinMode(byte pin,arduinoPinMode mode)
{
  //Configura el modo de un pin

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

void digitalWrite(byte pin,byte value)
{
  //Escribe el estado de un pin digital
  //Los valores de value son:
  //  LOW = 0
  //  HIGH = 1

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

byte digitalRead(byte pin)
{
  //Lee el estado de un pin digital

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

uint32_t digitalRead()
{
  //Lee el estado de todos los pines digitales
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

void InterruptionFunctionLauncher(bool *running,void (*f)(byte,byte),byte pin,byte status)
{
  //Funci�n que lanza la funci�n f(pin,status) y anota en running
  //cuando termina.
  //Debe ser llamada como thread.
  //Es obligatorio para poder controlar cu�ndo finaliza la funci�n

  //Llamamos a la funci�n
  f(pin,status);
  //Hemos terminado de ejecutar la funci�n
  //Ya no est� en ejecuci�n
  *running=false;
}

void InterruptionThreadLoop()
{
  //Funci�n para el bucle principal de ejecuci�n
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

void detachInterrupt(byte pin)
{
  //Elimina los eventos de un pin

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

void attachInterrupt(byte pin,void (*f)(byte,byte),arduinoEdgeType mode)
{
  //Define la funci�n a la que se debe llamar ante un cambio de estado
  //de un pin mediante una interrupci�n
  //El segundo par�metro espera una funci�n que no devuelve nada y que
  //tiene dos par�metros: el pin y su estado actual

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
  //El dato es el valor PWM del asignado al pin
  //Nota:
  //Cuando asignamos un valor de PWM 0 a un pin, simplemente lo apagamos y
  //eliminamos su referencia del diccionario.
  //Por lo tanto, nunca tendremos nodos con valor 0 en el diccionario.
  //En el bucle principal existe una variable de tipo byte cuyo valor
  //aumenta en cada ciclo (tick) y cuando se desborda vuelve a 0.
  //Gracias a ella podemos controlar cuando un pin debe cambiar de estado.
  //El valor PWM lo entendemos como el n�mero de ticks que el pin est�
  //en estado HIGH. Despu�s pasar� a LOW.
  //Si indicamos un valor PWM de 255, el pin siempre estar� en HIGH.
  //Para evitar que en el tick 255 todos los pines pase a LOW, guardaremos
  //siempre el valor-1 de PWM.
  unordered_map<byte,byte> pwmDicc;
  //Tiempo en microsegundos entre ticks (periodo)
  //Est� relacionado con la frecuencia de PWM
  uint16_t period=0; //Inicialmente utilizamos la m�xima frecuencia
  //Puesto que estamos utilizando un utin16_t para mantener el periodo entre
  //ticks, estamos limitando la frecuencia m�nima que ser�a:
  //  1000000/256/65535=0.0596055~0.06Hz
};

//Creamos un objeto para mantener la gesti�n de PWM
pwmsStruct myPWMs;

void freqPWM(float freq)
{
  //Asigna una nueva frecuencia de PWM en Hz
  //Afecta a todos los pines
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

void pwmThreadLoop()
{
  //Funci�n para el bucle principal de ejecuci�n
  //Debe ser llamada como thread
  //Utiliza el objeto myPWMs definido previamente
  
  //Tick actual
  byte tick=0;

  //Mientras haya alg�n nodo en el diccionario...
  while(myPWMs.pwmDicc.size()>0)
  {
    //Si el tick es el primero (cero), todos los pines se deben encender
    if(!tick)
      //Recorremos todos los nodos del diccionario con un iterator
      for(auto &itr : myPWMs.pwmDicc)
        //Encendemos el pin
        digitalWrite(itr.first,HIGH);
    //El tick no es el primero...
    else
    {
      //Recorremos todos los nodos del diccionario con un iterator
      for(auto &itr : myPWMs.pwmDicc)
        //Si el tick actual coincide con el valor PWM del pin...
        //...el pin se debe apagar
        if(tick==itr.second+1) digitalWrite(itr.first,LOW);      
    }
    //Dormiremos hasta el pr�ximo tick
    delayMicroseconds(myPWMs.period);
    //Hemos terminado de procesar el tick actual. Pasamos al siguiente
    tick++;
  }
}

void analogWrite(byte pin,byte value)
{
  //Define la funci�n a la que se debe llamar para hacer cualquier cambio
  //de estado PWM a un pin
  
  //Si no se ha inicializado el chip BCM2835...terminamos
  if(bcm2835_st == MAP_FAILED) return;
  //El chip BCM2835 se ha inicializado

  //Si ya existe PWM para el pin solicitado...
  if(myPWMs.pwmDicc.count(pin))
  {
    //...si tiene un valor mayor que cero...simplemente lo actualizamos
    if(value) myPWMs.pwmDicc.at(pin)=value-1;
    //Si el valor es cero es que no utilizaremos m�s este pin
    else
    {
      //Eliminamos el nodo del diccionario
      myPWMs.pwmDicc.erase(pin);
      //Ponemos el pin en estado LOW
      digitalWrite(pin,LOW);
      //Si este era el �ltimo nodo del diccionario, ya se detendr� el thread
    }
  }
  //El pin indicado no existe
  else
  {
    //Si el valor es mayor que cero...
    if(value)
    {
      //El pin indicado ser� de salida
      pinMode(pin,OUTPUT);
      //Comenzamos en encendido
      digitalWrite(pin,HIGH);
      //Creamos un nuevo nodo en el diccionario con los datos del pin PWM
      myPWMs.pwmDicc.emplace(pin,value-1);
      //Si el diccionario s�lo tiene el nodo que acabamos de crear...
      if(myPWMs.pwmDicc.size()==1)
      {
        //...es que ha sido el primero. Tendremos que lanzar el thread
        //del loop ahora.

        //Lanzamos el thread a la funci�n principal de gesti�n de PWM
        thread thread1(pwmThreadLoop);
        //Desvinculamos el thread que controla el bucle principal para
        //no tener que preocuparnos por el join. Sabemos que cuando el
        //diccionario est� vac�o, finalizar� el thread
        thread1.detach();
      }
    }
    //El valor indicado es 0...simplemente apagamos el pin
    else digitalWrite(pin,LOW);
  }
}

//Inicializamos el chip BCM2835 para acceso a los pines, obtenemos el 
//resultado, pero no lo comprobamos
bool _arduinoBeginAnswer=ArduinoBegin();

#endif
