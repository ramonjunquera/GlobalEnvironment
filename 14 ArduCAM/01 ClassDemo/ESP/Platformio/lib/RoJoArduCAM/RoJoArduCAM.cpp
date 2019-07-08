/*
  Nombre de la librería: RoJoArduCAM.h
  Versión: 20180711
  Autor: Ramón Junquera
  Descripción:
    Gestión de cámara ArduCAM-Mini-2MP para placas ESP

    ArduCAM es una familia de cámaras con las que se puede trabajar con procesadores lentos como Arduino
    o ESP.
    La gran ventaja de esas cámaras es que capturan la imagen y la guardan en una memoria interna que
    puede ser consultada posteriormente sin limitación de tiempo.
    Habitualmente las cámaras no tienen esta memoria. Es el propio procesador el que debe leer en tiempo
    real lo que está capturando la cámara.
    Puesto que los procesadores que utilizamos no tienen la suficiente velocidad como para obtener y
    guardar esta información en tiempo real, no tenemos más remedio que utilizar cámaras tipo ArduCAM con
    memoria interna.
  
    La librería está hecha para la ArduCAM modelo Mini 2MP.
    Tiene un sensor modelo OV2640. 
    Esta cámara tiene 2 Mpx de resolución. Suficiente como para hacer fotografías a 1600x1200.
    La memoria interna de la cámara es de 384 Kb.
    La profundidad de color es de 16 bits.
    Además de entregar la información sin comprimir (formato bmp), también es capaz de comprimirla en
    formato jpg.

    La cámara tiene dos tipos de conexión: I2C y SPI.
    Puesto que el puerto SPI es mucho más rápido que el SPI, se utiliza para la descarga de la imagen desde
    la memoria interna.
    La conexión I2C se utiliza para enviar instrucciones a la cámara acerca del formato o resolución.
    Son necesarias ambas conexiones para el correcto funcionamiento.
    La máxima velocidad que soporta la conexión SPI es de 8KHz.
    En cambio I2C se puede subir hasta los 400KHz.

    La librería no hace uso del pin CS de SPI por hardware. Este detalle junto con que se crear y finalizan
    las transmisiones cuando son necesarias nos da la flexibilidad de utlizarla con otras librerías que 
    gestionen otros dispositivos SPI sin interferencias.

    Aprovechando que todos los procesadores ESP (Espressif) tienen un sistema de almacenamiento de archivos,
    se simplifica el proceso de captura creando directamente los archivos finales.

    La librería hace uso del archivo RoJoArduCAM.dat que debe encontrarse en el sistema de archivos.
    En este archivo se guardan las colecciones de comandos necesarias para configurar la cámara en 
    sus distintos modos. De esta manera se ahorra memoria para variables.
    Este archivo se genera con el programa RoJoArduCAMdat.
    El archivo que genera se encuentra en la carpeta data para poder subirlo con facilidad al sistema de archivos.
    El archivo contiene las siguientes colecciones de archivos:
      0 = OV2640_160x120_JPEG
      1 = OV2640_176x144_JPEG
      2 = OV2640_320x240_JPEG
      3 = OV2640_352x288_JPEG
      4 = OV2640_640x480_JPEG
      5 = OV2640_800x600_JPEG
      6 = OV2640_1024x768_JPEG
      7 = OV2640_1280x1024_JPEG
      8 = OV2640_1600x1200_JPEG
      9 = OV2640_JPEG_INIT
     10 = OV2640_YUV422
     11 = OV2640_JPEG
     12 = OV2640_QVGA
     13 = BMP_HEADER

    El método setResolution nos permite seleccionar entre las siguientes resoluciones:
      0: 160x120
      1: 176x144
      2: 320x230
      3: 352x288
      4: 640x480
      5: 800x600
      6: 1024x768
      7: 1280x1024
      8: 1600x1200
    Si utilizamos compresión jpg, podemos tomar fotos de cualquiera de los tamaños permitidos.
    Si no utilizamos compresión (bmp), la librería reducirá automáticamente el número de 
    filas de la imagen. El límite viene dado por la cantidad de memoria interna de la cámara.
    Aun así, si capturamos imágenes sin comprimir en las dos resoluciones más altas (1280x1024
    y 1600x1200) no se obtienen imágenes legibles.

    El método begin inicializa la cámara. Tiene como parámetro el pin CS de SPI.
    Devolverá un código de error de la siguiente lista:
      0: No hubo errores
      1: Error al inicializar el sistema de archivos
      2: Falta el archivo RoJoArduCAM.dat
      3: Error de conexión SPI
      4: La cámara no es el modelo esperado

    El método savePhoto guarda la imagen capturada en la memoria de la cámara en un archivo o
    la redimensiona para guardarla en un sprite.
    Devolverá un código de error de la siguiente lista:
      0: Imagen guardada con éxito
      1: Cámara no inicializada
      2: No se ha capturado ninguna imagen
      3: La imagen aun no ha terminado de capturarse
      4: No se puede crear el archivo

    Recordemos la estructura de una cabecera bmp:
      Posición    Contenido  Tipo     Descripción
      0,1         0x4D42     uint16_t Tipo de archivo. Caracteres "BM". Constante
      2,3,4,5     0x00025842 uint32_t Tamaño del archivo en bytes = ancho*alto*2+66 (66 es el tamaño de la cabecera)
      6,7,8,9     0x00000000 uint32_t Reservado. Constante
      10,11,12,13 0x00000042 uint32_t Inicio de datos de imagen. Siempre tras la cabecera. 66. Constante
      14,15,16,17 0x00000028 uint32_t Tipo de cabecera. 40 = para Windows. Constante
      18,19,20,21 0x00000140 uint32_t Anchura en pixels
      22,23,24,25 0x000000F0 uint32_t Altura en pixels
      26,27       0x0001     uint16_t Número de planos = 1 = constante
      28,29       0x0010     uint16_t Tamaños de pixel en bits = 16 = constante
      30,31,32,33 0x00000003 uint32_t Compresión tipo 3 = constante. Definida por máscara de bits al final de la cabecera
      34,35,36,37 0x00025800 uint32_t Tamaño de la imagen en bytes = anchura*altura*2
      38,39,40,41 0x00000EC4 uint32_t Resolución horizontal = número de pixels por metro = constante
      42,43,44,45 0x00000EC4 uint32_t Resolución vertical = número de pixels por metro = constante
      46,47,48,49 0x00000000 uint32_t Tamaño de la tabla de colores indexados en bytes = 0 (no se utiliza) = constante
      50,51,52,53 0x00000000 uint32_t Contador de colores importantes = 0 (todos) = constante
      54,55,56,57 0x0000F800 uint32_t Máscara de R (red  ) = 00000000 00000000 11111000 00000000 = constante
      58,59,60,61 0x000007E0 uint32_t Máscara de G (green) = 00000000 00000000 00000111 11100000 = constante
      62,63,64,65 0x0000001F uint32_t Máscara de B (blue ) = 00000000 00000000 00000000 00011111 = constante
 
    Todos los valores uint se escriben desde LB a HB.
    Los pixel se escriben por filas, de izquierda a derecha.
    Las filas se escriben de abajo arriba.
    Por lo tanto el primer pixel corresponde a la posición inferior izquierda.

    Recordemos la disposición de pines de comunicaciones de los distintos modelos:

            ------SPI------- --I2C--
    Modelo  SCK MISO MOSI CS SCL SDA
    ------- --- ---- ---- -- --- ---
    ESP8266 D5   D6   D7  D8 D1  D2
    ESP32   18   19   23   5 22  21
  
    Nota:
    ArduCAM tine resistencias pullup en el pin CS. Esto significa que si lo conectamos al
    pin SPI definido en el estándar de ESP8266 (D8), se forzará a HIGH a la hora de arrancar.
    Este detalle impide que la placa arranque correctamente.
    En este caso se debe seleccionar otro pin distinto.

    Mejoras pendientes:
    - Unificar los modos de conexión SPI. Es necesario utilizar SPI_MODE0 y SPI_MODE3?
*/

#include "Arduino.h"
#if defined(ESP32)
  #include <SPIFFS.h> 
#else
  #include <FS.h>
#endif
#include <Wire.h> //Gestión I2C
#include <SPI.h> //Gestión SPI
#include "RoJoArduCAM.h"
#include "RoJoSprite16.h" //Gestión de sprites color

byte RoJoArduCAM::begin(byte csPin,byte pinSDA,byte pinSCL)
{
  //Inicialización de la cámara
  //Devuelve el código de error
  // 0 = Sin errores
  // 1 = Error al inicializar el sistema de archivos
  // 2 = Falta el archivo RoJoArduCAM.dat
  // 3 = Error de conexión SPI
  // 4 = La cámara no es el modelo esperado

  //Guardamos en la variable interna el pin CS
  _csPin=csPin;
  //El pin CS será de salida
  pinMode(_csPin,OUTPUT);
  
  //Si no podemos inicializar el sistema de archivos...error 1
  if(!SPIFFS.begin()) return 1;
  //Si no existe al archivo RoJoArduCAM.dat...error 2
  if(!SPIFFS.exists("/RoJoArduCAM.dat")) return 2;

  //No se controlará el estado del pin CS por hardware. Lo haremos nosotros
  //Esto nos permite compartir el bus SPI con distintos dispositivos
  SPI.setHwCs(false);

  //Comprobamos la comunicación SPI
  uint16_t counter=0; //Contador de tests realizados
  byte recVal; //Variable para guardar el valor recibido
  do
  {
    //Inicializamos conexiones SPI
    SPI.begin();
    //Iniciamos transacción
	SPI.beginTransaction(_spiSetting3);
    digitalWrite(_csPin,LOW);
      //Comprobamos si la comunicación SPI es correcta
      //Para ello la cámara dispone de un registro especial para hacer pruebas (ARDUCHIP_TEST1=0x00)
      //Escribiremos un valor en ese registro y después pediremos que nos devuelvan el
      //valor que contiene. Si no coincide es que la conexión SPI no funciona correctamente
      //Escribimos un valor en el registro de pruebas
      _writeRegSPI(0x80,0x55); //Reg=ARDUCHIP_TEST1=0x00 | 0x80 = 0x80. Val=0x55=0d85
      //Pedimos valor del mismo registro ARDUCHIP_TEST1=0x00
      recVal=_readRegSPI3(0x00);
    digitalWrite(_csPin,HIGH);
    SPI.endTransaction();
    //Hemos terminado otro test
    counter++;
  }
  //Repetiremos el test si el valor recibido no es el esperado y
  //si no llevamos más de un determinado número de tests realizados
  while(recVal!=0x55 && counter<15000);
  //Hemos terminado de hacer tests
  //Si el valor obtenido no es el esperado...devolvemos error de SPI
  if(recVal!=0x55) return 3;
  //La comunicación SPI es correcta
  
  //Inicializamos el protocolo I2C
  #if defined(ARDUINO_ARCH_AVR) || defined(__arm__) //Si es una placa Arduino o una RPi ...
    //En las placas Arduino o RPi, no se pueden seleccionar los pines I2C
    //Los posibles pines I2C no se tendrán en cuenta
    Wire.begin();
    _pinSCL=SCL;
  #else //No es una placa Arduino ni RPi. Es una ESP.
    //Si se ha indicado pin de I2C (SDA & SCL)...activamos el I2C personalizado
    if(pinSCL<255)
    {
	  Wire.begin(pinSDA,pinSCL);
	  _pinSCL=pinSCL;
	}
    //...y si no...usamos una I2C con los pines por defecto
    else
    {
	  Wire.begin();
	  _pinSCL=SCL;
	}
  #endif
	
  //Habitualmente el bus I2C transmite con una frecuencia de 100KHz.
  //Los procesadores ESP son capaces de alcanzar los 400KHz.
  //ArduCAM da muchos errores de conexión cuando se superan los 100KHz.
  //Por lo tanto, no cambiaremos la velocidad por defecto.
  //Wire.setClock(400000L);

  //Comprobamos si la cámara es el modelo esperado
  counter=0;
  byte chipIdH,chipIdL;
  do
  {
	_writeRegI2C(0xFF,0x01);
    chipIdH=_readRegI2C(0x0A); //OV2640_CHIPID_HIGH=0x0A
    chipIdL=_readRegI2C(0x0B); //OV2640_CHIPID_LOW=0x0B
	counter++;
  }
  //Repetiremos el test si los valores recibidos no son los esperados y
  //si no llevamos más de un determinado número de tests realizados
  //Los valores 0x26=0d38 y 0x42=0d66 corresponden con la ArduCAM Mini-2MP
  while((chipIdH!=0x26 || chipIdL!=0x42) && counter<3000);
  //Hemos terminado de hacer tests
  //Si los valores obtenidos no son los esperados...devolvemos error de modelo
  if ((chipIdH != 0x26) || (chipIdL != 0x42)) return 4;
  //La comunicación I2C es correcta y la cámara es el modelo esperado

  //Todo correcto: SPIFFS, RoJoArduCAM.dat, SPI, I2C y modelo
  _started=true; //La cámara se ha inicializado correctamente
        
  //Aplicamos la configuración inicial
  _currentResId=2; //Resolución por defecto = 320x240 = id:2
  setFormatJPG(); //Formato de archivo = JPG

  //Limpiamos la memoria de la cámara (clear_fifo_flag)
  _writeRegSPI(0x84,0x01); //Reg=ARDUCHIP_FIFO=FIFO and I2C control=0x04 | 0x80 = 0x84. Val=FIFO_CLEAR_MASK=0x01
  //Fijamos el número de frames por captura. Constante. 0x00
  _writeRegSPI(0x81,0x00); //Reg=ARDUCHIP_FRAMES=Bit[2:0]Number of frames to be captured=0x01 | 0x80 = 0x81. Val=0x00
  
  //Todo ok
  return 0;
}

void RoJoArduCAM::_writeRegSPI(byte reg,byte value)
{
  //Escribe un registro y su valor (2 bytes) por SPI
  SPI.write(reg);
  SPI.write(value);
}

byte RoJoArduCAM::_readRegSPI0(byte reg)
{
  //Lee un registro por SPI con SPI_MODE0
  
  //En SPI_MODE0, antes de leer un registro, la cámara necesita resetear el pin CS
  digitalWrite(_csPin,HIGH);
  digitalWrite(_csPin,LOW);
  
  SPI.write(reg); //Indicamos el registro a leer
  return SPI.transfer(0x00); //Devolvemos el valor recibido
}

byte RoJoArduCAM::_readRegSPI3(byte reg)
{
  //Lee un registro por SPI con SPI_MODE3
  
  SPI.write(reg); //Indicamos el registro a leer
  return SPI.transfer(0x00); //Devolvemos el valor recibido
}

void RoJoArduCAM::_writeRegI2C(byte reg,byte value)
{
  //Escribe un registro y su valor (2 bytes) por I2C
  
  //Detectaremos si la conexión ha sido correcta al obtener el código
  //de error al cerrarla
  //Ante el error 4 (error desconocido) se puede hacer algo:
  // - Fijar el pin de SCL como salida
  // - Reiniciar la clase Wire
  //Ante el error 2 (sin respuesta del dispositivo) no se qué hacer
  
  byte error;
  do
  {
	Wire.beginTransmission(_I2Cid);
    Wire.write(reg);
    Wire.write(value);
    error=Wire.endTransmission();
    yield();
    if(error==4)
    {
		pinMode(_pinSCL,OUTPUT);
		Wire.begin();
	}
  }
  while(error==4);
}

byte RoJoArduCAM::_readRegI2C(byte reg)
{
  //Lee un registro por I2C
  
  byte answer=0;
  Wire.beginTransmission(_I2Cid);
  Wire.write(reg); //Indicamos el registro a leer
  Wire.endTransmission();
  Wire.requestFrom((int)_I2Cid,1); //Solicitamos una respuesta de un byte
  //Si hay algún dato pendiente de recibir...lo anotamos
  if (Wire.available()) answer = Wire.read();
  return answer; //Devolvemos el valor recibido
}

void RoJoArduCAM::_writeRegI2C(byte regIndex)
{
  //Envía la colección de registros de índice regIndex guardada en
  //el archivo RoJoArduCAM.dat

  //Abrimos archivo de datos
  File f=SPIFFS.open(F("/RoJoArduCAM.dat"),"r");
  //Ponemos el puntero de lectura en el registro correcto
  f.seek(regIndex*2,SeekSet);
  //Creamos la variable donde guardaremos la posición de inicio de datos
  uint16_t start;
  //Obtenemos la posición de inicio de datos
  f.readBytes((char *)&start,2);
  //Creamos la variable en la que guardaremos la lngitud de los datos
  uint16_t len;
  //Obtenemos la posición de inicio de datos del siguiente item
  f.readBytes((char *)&len,2);
  //La longitud real es la diferencia de ambos
  len-=start;
  //El número de registros es la mitad de la longitud. Un registro son 2 bytes
  len/=2;
  //Posicionamos el puntero de lectura al inicio de los datos
  f.seek(start,SeekSet);
  //Creamos la variable donde guardaremos el registro y su valor
  byte reg,val;
  //Leemos todos los registros (con sus valores)
  for(uint16_t i=0;i<len;i++)
  {
    //Leemos el registro
    reg=f.read();
    //Leemos su valor
    val=f.read();
    //Enviamos el registro y su valor por I2C
    _writeRegI2C(reg,val);
  }
  //Hemos terminado de utilizar el archivo
  f.close();
}

void RoJoArduCAM::setFormatJPG()
{
  //Fija el formato de salida a JPG

  //Si no se ha inicializado la cámara...hemos terminado
  if(!_started) return;
  
  //Anotamos el formato seleccionado
  _currentIsJPG=true;

  _writeRegI2C(0xFF,0x01);
  _writeRegI2C(0x12,0x80);
  delay(100);
  _writeRegI2C(9); //Envía por I2C la colección de registros 9=OV2640_JPEG_INIT
  _writeRegI2C(10); //Envía por I2C la colección de registros 10=OV2640_YUV422
  _writeRegI2C(11); //Envía por I2C la colección de registros 11=OV2640_JPEG
  _writeRegI2C(0xFF,0x01);
  _writeRegI2C(0x15,0x00);
  _writeRegI2C(_currentResId); //Aplicamos la resolución que tenemos definida actualmente. Obligatorio al cambiar de modo
}

void RoJoArduCAM::setFormatBMP()
{
  //Fija el formato de salida a BMP

  //Si no se ha inicializado la cámara...hemos terminado
  if(!_started) return;
  
  //Anotamos el formato seleccionado
  _currentIsJPG=false;

  _writeRegI2C(0xFF,0x01);
  _writeRegI2C(0x12,0x80);
  delay(100);
  _writeRegI2C(12); //Envía por I2C la colección de registros 12=OV2640_QVGA
  _writeRegI2C(_currentResId); //Aplicamos la resolución que tenemos definida actualmente. Obligatorio al cambiar de modo
}

void RoJoArduCAM::setResolution(byte resId)
{
  //Fija una resolución a la cámara
  //Los códigos de las distintas resoluciones son:
  // 0 = 160x120
  // 1 = 176x144
  // 2 = 320x240
  // 3 = 352x288
  // 4 = 640x480
  // 5 = 800x600
  // 6 = 1024x768
  // 7 = 1280x1024
  // 8 = 1600x1200

  //Si no se ha inicializado la cámara...hemos terminado
  if(!_started) return;

  _writeRegI2C(resId); //Envía por I2C la colección de registros necesarios
  _currentResId=resId; //Anotamos la resolución actual
}

bool RoJoArduCAM::photoIsReady()
{
  //Hay alguna foto en la memoria de la cámara para descargar?
  //No necesita inicialización SPI

  SPI.beginTransaction(_spiSetting0);
  digitalWrite(_csPin,LOW);
    byte recVal=_readRegSPI0(0x41); //Pedimos valor del mismo registro ARDUCHIP_TRIG=Trigger source=0x41
  digitalWrite(_csPin,HIGH);
  SPI.endTransaction();
  return recVal & 8;
}

void RoJoArduCAM::takePhoto()
{
  //Hace una foto y la guarda en la memoria de la cámara
  //No necesita inicialización SPI

  //Si no se ha inicializado la cámara...hemos terminado
  if(!_started) return;

  SPI.beginTransaction(_spiSetting0);
  digitalWrite(_csPin,LOW);
    //Limpiamos la memoria de la cámara (flush_fifo)
    _writeRegSPI(0x84,0x01); //Reg=ARDUCHIP_FIFO=FIFO and I2C control=0x04 | 0x80 = 0x84. Val=FIFO_CLEAR_MASK=0x01
    //Solicitamos inicio de captura (start_capture)
    _writeRegSPI(0x84,0x02); //Reg=ARDUCHIP_FIFO=FIFO and I2C control=0x04 | 0x80 = 0x84. Val=FIFO_START_MASK=0x02
    //La resolución de la foto de la memoria de la cámara es la actual
    _bufferResId=_currentResId;
    //El formato de la foto en la memoria de la cámara es la actual
    _bufferIsJPG=_currentIsJPG;
    //Hay una nueva foto en la memoria de la cámara
    _photoInCamera=true;
  digitalWrite(_csPin,HIGH);
  SPI.endTransaction();
}

uint32_t RoJoArduCAM::_bufferSize()
{
  //Lee el tamaño de la imagen de la memoria de la cámara
  //No necesita inicialización SPI

  //Si no se ha inicializado la cámara...devolvemos tamaño cero
  if(!_started) return 0;

  SPI.beginTransaction(_spiSetting0);
  digitalWrite(_csPin,LOW);
    uint32_t fileSize = _readRegSPI0(0x42); //FIFO_SIZE1=Camera write FIFO size[7:0]=0x42
    fileSize |= (_readRegSPI0(0x43) << 8); //FIFO_SIZE2=Camera write FIFO size[15:8]=0x43
    fileSize |= ((_readRegSPI0(0x44) & 0x7F) << 16); //FIFO_SIZE3=Camera write FIFO size[18:16]=0x44
  digitalWrite(_csPin,HIGH);
  SPI.endTransaction();
  return fileSize;
}

void RoJoArduCAM::_writeRegFile(byte regIndex,File *destF)
{
  //Copia los datos del índice regIndex guardado en
  //el archivo RoJoArduCAM.dat a un archivo

  //Abrimos archivo de datos
  File f=SPIFFS.open(F("/RoJoArduCAM.dat"),"r");
  //Ponemos el puntero de lectura en el registro correcto
  f.seek(regIndex*2,SeekSet);
  //Creamos la variable donde guardaremos la posición de inicio de datos
  uint16_t start;
  //Obtenemos la posición de inicio de datos
  f.readBytes((char *)&start,2);
  //Creamos la variable en la que guardaremos la lngitud de los datos
  uint16_t len;
  //Obtenemos la posición de inicio de datos del siguiente item
  f.readBytes((char *)&len,2);
  //La longitud real es la diferencia de ambos
  len-=start;
  //Posicionamos el puntero de lectura al inicio de los datos
  f.seek(start,SeekSet);
  //Recorremos todos los datos...los leemos y los escribimos en el archivo destino
  for(uint16_t i=0;i<len;i++) destF->write(f.read());
  //Hemos terminado de utilizar el archivo
  f.close();
}

byte RoJoArduCAM::savePhoto(String filename,uint32_t *resX,uint32_t *resY)
{
  //Si hay alguna imagen en la memoria de la cámara, la guarda en un archivo.
  //El nombre del archivo no debe incluir extensión. Se añade automáticamente.
  //En resX y resY devuelve el tamaño de la imágen guardada, porque no tiene que
  //coincidir en filas con la resolución fijada.
  //Devuelve el código de error:
  // 0 = foto guardada con éxito
  // 1 = cámara no inicializada
  // 2 = no se ha capturado ninguna imagen
  // 3 = la imagen aun no ha terminado de capturarse
  // 4 = no se puede crear el archivo

  byte errorCode=0; //Inicialmente no hay errores
  uint32_t fileSize=0; //Tamaño de la imagen de la memoria de la cámara

  //Si no se ha inicializado la cámara...Error. Cámara no inicializada
  if(!_started) errorCode=1;
  else
  {
    //La cámara ha sido inicializada
    //Si no hay ninguna foto en la memoria de la cámara...Error. No se ha tomado ninguna foto
    if(!_photoInCamera) errorCode=2;
    else
    {
      //Hay una imagen en la cámara
      //Si no ha terminado de hacer la foto y guardarla en su memoria...Error. Aun no ha terminado la captura
      if(!photoIsReady()) errorCode=3;
      else
      {
        //La captura ha finalizado
        //Componemos el nombre del archivo final en función del formato
        filename="/"+filename+(_bufferIsJPG?".jpg":".bmp");
        //El método open de SPIFFS sólo permite cadenas en formato C
        File f = SPIFFS.open(filename.c_str(),"w");
        //Si no se ha creado el archivo...Error. No se puede crear el archivo
        if (!f) errorCode=4;
        else
        {
          //Se ha podido crear el archivo
          //Si se trata de una imagen jpg...obtenemos su tamaño
          if(_bufferIsJPG) fileSize=_bufferSize();
          //Anotamos la resolución real de la imagen de la memoria de la cámara
          *resX=_resX[_bufferResId];
          *resY=_resY[_bufferResId];
          
          SPI.beginTransaction(_spiSetting0);
          digitalWrite(_csPin,LOW);
            //Leeremos la imagen de la memoria de la cámara
            SPI.write(0x3C); //BURST_FIFO_READ=Burst FIFO read operation=0x3C
            
            //La información se extrae de manera distinta dependiendo del formato
            if(_bufferIsJPG) //Es un jpg...
            {
              //El primer byte no se tiene en cuenta
              SPI.transfer(0x00);
              //Recorremos todos los bytes de la imagen...leemos SPI y lo escribimos en el archivo
              for(uint32_t i=0;i<fileSize;i++) f.write(SPI.transfer(0x00));
            }
            else //Es un bmp
            {
              //Se trata de datos en formato bmp
              //En este formato NO se incluye la cabecera del archivo como en jpg
              //Los datos están sin comprimir
              //Necesitamos escribir la cabecera nosotros mismos
              //La profundidad de color es de 16bits. 2 bytes por pixel
              //Los datos llegan al revés (de abajo arriba)
              //No funcionan las dos resoluciones más altas
              //Si el tamaño total de la imagen no entra en la memoria de la cámara, sólo se 
              //tomarán las primeras líneas.

              //Comenzaremos copiando la cabecera del archivo bmp
              _writeRegFile(13,&f); //13 = índice de cabecera bmp en RoJoArduCAM.dat

              //La memoria de la cámara es de 384Kb
              //384Kb = 384*1024 = 393216 bytes, pero como son 2 bytes por pixel sólo entran 393216/2=196608 pixels
              //Si la imagen es mayor que la memoria de la cámara...
              if(*resX * *resY > 196608)
              {
                //...no se ha podido almacenar toda la imagen en la memoria de la cámara
                //Calcularemos cuántas líneas podemos conseguir
                *resY = 196608 / *resX;
              }
              //Obligatorio no tener en cuenta el primer byte
              SPI.transfer(0x00);

              //Si guardamos la información tal y como se recibe de la cámara obtendremos una imagen invertida
              //(girada 180 grados).
              //Para corregirlo al vuelo leeremos tanto las filas como las columnas en orden inverso.
              //La primera línea leida corresponde con la línea superior de la imagen (cuando está derecha).
              //Pero como un bmp guarda las filas de las imágenes en orden inverso, debemos escribirla al final!
              //Podemos resolver la situación de 4 maneras dintintas:
              //1.
              //Almacenamos en memoria la imagen completa para después volcarla al archivo en orden inverso al
              //recibido.
              //Ventajas: Muy rápido
              //Inconveniente: Ocupa demasiada memoria. Un máximo de 384Kn (tamaño de la memoria de la cámara).
              //2.
              //Volcamos la información recibida en un archivo temporal. Después leemos el archivo en orden
              //inverso posicionando la lectura contínuamente para volvarlo en el orden correcto en el 
              //archivo definitivo.
              //Ventajas: Ocupa muy poca memoria.
              //Inconvenientes: Se necesita gestionar un archivo temporal (crear y borrar). La lectura inversa
              //  del archivo temporal es lenta porque se reposiona la lectura continuamente.
              //3.
              //Creamos el archivo definitivo con el tamaño adecuado, pero lleno de basura. Entonces leemos
              //la información y la vamos guardando en orden inverso.
              //Ventaja: Ocupa muy poca memoria.
              //Inconvenientes: Lento porque necesitamos crear el archivo final con el tamaño adecuado desde
              //  un principio, además cambiamos la posición de escritura continuamente porque es inversa.
              //4.
              //Creamos el archivo definitivo con el tamaño adecuado, pero lleno de basura. Creamos un buffer
              //del tamaño de una línea. Leemos cada línea en orden inverso en el buffer. Volcamos el buffer
              //en la posición adecuada del archivo.
              //Ventajas: Ocupa relativamente poca memoria (máximo 1600*2 bytes = 3Kb). Es bastante rápido porque
              //  la escritura sólo se posiciona una vez por línea.
              //Inconveniente: Se neceita crear el archivo final con el tamaño adecuado desde un principio.

              //Finalmente utilizaremos la opción 4, que parece la más compensada.

              //Si pedimos la creación de un archivo de escritura y a continuación movemos la posición de escritura,
              //no se dimensionará correctamente el archivo.
              //No se creará automáticamente un archivo de ese tamaño lleno de ceros (o basura), como ocurre en otros
              //sistemas.
              //El sistem se archivos de los ESP es más rudimentario.
              //No tenemos más remedio que escribir en el archivo manualmente hasta que alcance el tamaño deseado.
              //Después podremos reescribir la información como queramos.
              //Esto es un doble trabajo (y tiempo), pero no se puede hacer de otra manera.

              int32_t rowLength = *resX*2; //Tamaño en bytes de una fila. Para no calcularlo varias veces
              //Creamos un array que guardará la información de los pixels de una fila
              byte row[rowLength];
              //Escribimos el tamaño completo del archivo con basura
              for (uint32_t y=0; y<*resY;y++) f.write(row,rowLength);
              //Recorremos las filas al revés
              for (int32_t y = *resY-1; y>=0; y--)
              {
                //Recorremos las columnas al revés...y llenamos el array de fila
                for (int32_t x = rowLength-1; x>=0; x--) row[x]=SPI.transfer(0x00);
                //Calculamos la posición de escritura de esta fila
                f.seek(y*rowLength+66,SeekSet);
                //Escribimos la línea completa
                f.write(row,rowLength);
                yield();
              }
              //Hemos terminado de escribir el archivo .bmp
              //Tenemos que modificar su cabecera para que coincida con su contenido (tamaño y resolución)
              //Calculamos el tamaño de la imagen
              fileSize= *resY * rowLength;
              f.seek(34,SeekSet); 
              f.write((byte*)&fileSize,4); //Posición 34. Tamaño de la imagen
              //Calculamos el tamaño total del archivo
              fileSize+=66; //Le añadimos el tamaño de la cabecera
              f.seek(2,SeekSet); 
              f.write((byte*)&fileSize,4); //Posición 2. Tamaño total del archivo
              //Escribimos resolución de la imagen
              f.seek(18,SeekSet); 
              f.write((byte*)resX,4); //Posición 18. Anchura en pixels
              f.write((byte*)resY,4); //Posición 22. Altura en pixels
            }
            //Limpiamos memoria de la cámara
            _writeRegSPI(0x84,0x01); //clear_fifo_flag. Reg=ARDUCHIP_FIFO=FIFO and I2C control=0x04 | 0x80 = 0x84. Value=FIFO_CLEAR_MASK=0x01
          digitalWrite(_csPin,HIGH);
          SPI.endTransaction();
          f.close();
        }
        //La imagen ya no estará disponible
        _photoInCamera=false;
      }
    }
  }
  return errorCode;
}

byte RoJoArduCAM::savePhoto(uint32_t resX,uint32_t resY,RoJoSprite16 *sprite)
{
  //Guarda la imagen capturada en un sprite
  //Se indica el tamaño que debe tener el sprite
  //Se redimensionará la imagen al tamaño del sprite
  //Devuelve el código de error:
  // 0 = foto guardada con éxito
  // 1 = cámara no inicializada
  // 2 = no se ha capturado ninguna imagen
  // 3 = la imagen aun no ha terminado de capturarse
  // 4 = la imagen está comprimida
 
  byte errorCode=0; //Inicialmente no hay errores
  uint32_t imgX,imgY; //Resolución de la imagen capturada
  uint16_t colorH,colorL,color;
  
  //Si no se ha inicializado la cámara...Error. Cámara no inicializada
  if(!_started) errorCode=1;
  else
  {
    //La cámara ha sido inicializada
    //Si no hay ninguna foto en la memoria de la cámara...Error. No se ha tomado ninguna foto
    if(!_photoInCamera) errorCode=2;
    else
    {
      //Hay una imagen en la cámara
      //Si no ha terminado de hacer la foto y guardarla en su memoria...Error. Aun no ha terminado la captura
      if(!photoIsReady()) errorCode=3;
      else
      {
        //La captura ha finalizado
        //Si la imagen capturada es jpg...formato incorrecto
        if(_bufferIsJPG) errorCode=4;
        else
        {
          //La imagen no está comprimida
          //Anotamos la resolución de la imagen capturada
          imgX=_resX[_bufferResId];
          imgY=_resY[_bufferResId];
          //Si la imagen excede la memoria de la cámara...ajustaremos el número de filas válidas
          if(imgX*imgY>196608) imgY=196608/imgX;
          //Vaciamos el sprite y le asignamos el tamaño solicitado
          (*sprite).setSize(resX,resY);
          SPI.beginTransaction(_spiSetting0);
          digitalWrite(_csPin,LOW);
            //Leeremos la imagen de la memoria de la cámara
            SPI.write(0x3C); //BURST_FIFO_READ=Burst FIFO read operation=0x3C
            //Obligatorio no tener en cuenta el primer byte
            SPI.transfer(0x00);
            //Recorremos todas las filas
            for(uint16_t y=0;y<imgY;y++)
            {
              //Recorremos todas las columnas
              for(int16_t x=imgX-1;x>=0;x--)
              {
                colorH=SPI.transfer(0x00);
                colorL=SPI.transfer(0x00);
                color=colorL | colorH<<8;
                (*sprite).drawPixel((x*resX)/imgX,(y*resY)/imgY,color);
              }
            }
            //Limpiamos memoria de la cámara
            _writeRegSPI(0x84,0x01); //clear_fifo_flag. Reg=ARDUCHIP_FIFO=FIFO and I2C control=0x04 | 0x80 = 0x84. Value=FIFO_CLEAR_MASK=0x01
          digitalWrite(_csPin,HIGH);
          SPI.endTransaction();
          //La imagen ya no estará disponible
          _photoInCamera=false;
        }
      }
    }
  }
  return errorCode;
}

void RoJoArduCAM::setLight(byte lightId)
{
  //Fija un tipo de iluminación de la siguiente tabla
  // 0: auto
  // 1: soleado
  // 2: nublado
  // 3: oficina
  // 4: hogar

  //Si no se ha inicializado la cámara...hemos terminado
  if(!_started) return;
  //Si no es un código válido...hemos terminado
  if(lightId>4) return;

  //En el archivo RoJoArduCAM.dat tenemos siguientes grupos de comandos
  // 14 = iluminación auto
  // 15 = iluminación soleado
  // 16 = iluminación nublado
  // 17 = iluminación oficina
  // 18 = iluminación hogar
  //Directamente los leemos y enviamos por I2C
  _writeRegI2C(lightId + 14);
}

void RoJoArduCAM::setSaturation(int8_t s)
{
  //Fija la saturación de color
  //Sólo se permiten valores entre -2 y 2
  
  //Si no se ha inicializado la cámara...hemos terminado
  if(!_started) return;
  //Si no es un código válido...hemos terminado
  if(abs(s)>2) return;

  //En el archivo RoJoArduCAM.dat tenemos siguientes grupos de comandos
  // 19 = saturación -2
  // 20 = saturación -1
  // 21 = saturación 0
  // 22 = saturación +1
  // 23 = saturación +2
  //Directamente los leemos y enviamos por I2C
  _writeRegI2C(s + 21);
}

void RoJoArduCAM::setBrightness(int8_t b)
{
  //Fija el nivel de brillo
  //Sólo se permiten valores entre -2 y 2
  
  //Si no se ha inicializado la cámara...hemos terminado
  if(!_started) return;
  //Si no es un código válido...hemos terminado
  if(abs(b)>2) return;

  //En el archivo RoJoArduCAM.dat tenemos siguientes grupos de comandos
  // 24 = brillo -2
  // 25 = brillo -1
  // 26 = brillo 0
  // 27 = brillo +1
  // 28 = brillo +2
  //Directamente los leemos y enviamos por I2C
  _writeRegI2C(b + 26);
}

void RoJoArduCAM::setContrast(int8_t c)
{
  //Fija el nivel de contraste
  //Sólo se permiten valores entre -2 y 2
  
  //Si no se ha inicializado la cámara...hemos terminado
  if(!_started) return;
  //Si no es un código válido...hemos terminado
  if(abs(c)>2) return;

  //En el archivo RoJoArduCAM.dat tenemos siguientes grupos de comandos
  // 29 = contraste -2
  // 30 = contraste -1
  // 31 = contraste 0
  // 32 = contraste +1
  // 33 = contraste +2
  //Directamente los leemos y enviamos por I2C
  _writeRegI2C(c + 31);
}
void RoJoArduCAM::setEffect(byte effectId)
{
  //Fija un efecto especial de la siguiente tabla
  // 0: ninguno
  // 1: antiguo
  // 2: azulado
  // 3: verdoso
  // 4: rojizo
  // 5: blanco y negro
  // 6: negativo

  //Si no se ha inicializado la cámara...hemos terminado
  if(!_started) return;
  //Si no es un código válido...hemos terminado
  if(effectId>6) return;

  //En el archivo RoJoArduCAM.dat tenemos siguientes grupos de comandos
  // 34 = efecto ninguno
  // 35 = efecto antiguo
  // 36 = efecto azulado
  // 37 = efecto verdoso
  // 38 = efecto rojizo
  // 39 = efecto blanco y negro
  // 40 = efecto negativo
  //Directamente los leemos y enviamos por I2C
  _writeRegI2C(effectId + 34);
}

