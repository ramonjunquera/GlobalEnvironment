/*
  Nombre de la librería: RoJoXPT2046.h
  Versión: 20180520
  Autor: Ramón Junquera
  Descripción:
    Gestión del touch panel XPT2046
  Mejoras futuras:
    Reducir la dispersión de coordenadas de hardware en la primera pulsación
*/

#ifndef RoJoXPT2046_cpp
#define RoJoXPT2046_cpp

#include <Arduino.h>
#include <SPI.h> //Gestión de comunicaciones SPI
#ifdef ARDUINO_ARCH_AVR //Si es un Arduino...
  #error Arduino family has not SPIFFS
#elif defined(ESP32) //Si es un ESP32...
  #include <SPIFFS.h> 
#else //Si cualquier otra cosa (ESP8266 o RPi)...
  #include <FS.h>
#endif
#include "RoJoXPT2046.h" //Gestión del display

void RoJoXPT2046::begin(byte pinCS)
{
  //Inicialización
  
  //Guardamos el pin en la variable privada
  _pinCS=pinCS;
  //El pin CS será de salida
  pinMode(_pinCS,OUTPUT);
  //Enviamos un comando indicando que finalizamos cualquier solicitud anterior
  SPI.beginTransaction(_spiSetting);
  digitalWrite(_pinCS,LOW);
  SPI.transfer16(0x90);
  digitalWrite(_pinCS,HIGH);
  SPI.endTransaction();
}

bool RoJoXPT2046::getRawXY(uint16_t *x,uint16_t *y)
{
  //Obtiene las coordenadas de hardware
  //Devuelve si ha detectado pulsación

  //Solicitamos coordenadas de hardware hasta localizar la pareja con menos diferencia
  //y devolvemos su media

  //Coordenadas anteriores
  uint16_t xLast,yLast;
  //Coordenadas actuales
  uint16_t xCurrent,yCurrent;
  //Pares de coordenadas con la mínima diferencia
  uint16_t x0,y0,x1,y1;
  //Diferencia entre pares
  uint16_t xDiff,yDiff;
  //Diferencia entre coordenada antual y anterior
  uint16_t xCurrentDiff,yCurrentDiff;
  //Respuesta: hay pulsación?
  bool answer=false; //Inicialmente no
  //Inicializamos las diferencias y las últimas coordenadas con valores dummy muy altos
  xDiff=yDiff=xLast=yLast=65000;
  //Flag para continuar en el bucle
  bool keepLoop;
  //Inicio de transacción
  SPI.beginTransaction(_spiSetting);
  //Activamos el pin CS
  digitalWrite(_pinCS,LOW);
  //Enviamos comando para saber si hay pulsación
  //No recogemos nada
  SPI.transfer16(0xB1);
  //Enviamos comando para obtener coordenada x. 0xD1 = 209 = 0b11010001.
  //Recogemos el valor para saber si hay pulsación
  //Si hay pulsación...
  if(SPI.transfer16(0xD1)>500)
  {
    //No tenemos en cuenta la primera de las medidas. Normalmente es 
    //bastante incorrecta. Enviamos de nuevo el mismo comando para
    //obtener la coordenada x
    SPI.transfer16(0xD1);
    //Bucle principal
    do 
    {
      //Introducimos un retraso entre medidas para dar tiempo a que se estabilicen
      //y para que no de errores de WatchDog
      delay(1);
      //Enviamos comando para obtener coordenada y. 0x91 = 145 = 0b10010001.
      //Recojemos la coordenada x anteriormente solicitada
      xCurrent=SPI.transfer16(0x91);
      //Enviamos comando para obtener coordenada x para la próxima vez. 0xD1 = 209 = 0b11010001.
      //Recojemos la coordenada y anteriormente solicitada
      yCurrent=SPI.transfer16(0xD1);
      //Inicialmente queremos salir del bucle
      keepLoop=false;
      //Calculamos las diferencias entre la coordenadas actual y la anterior
      if(xCurrent>xLast) xCurrentDiff=xCurrent-xLast;
      else xCurrentDiff=xLast-xCurrent;
      if(yCurrent>yLast) yCurrentDiff=yCurrent-yLast;
      else yCurrentDiff=yLast-yCurrent;
      //Si la diferencia actual es menor que la almacenada...
      if(xCurrentDiff<xDiff)
      {
        //...anotamos la diferencia actual
        xDiff=xCurrentDiff;
        //Anotamos las coordenadas
        x0=xLast; x1=xCurrent;
        //Tenemos un nuevo mínimo en x. Nos mantenemos en el bucle
        keepLoop=true;
      }
      if(yCurrentDiff<yDiff)
      {
        //...anotamos la diferencia actual
        yDiff=yCurrentDiff;
        //Anotamos las coordenadas
        y0=yLast; y1=yCurrent;
        //Tenemos un nuevo mínimo en y. Nos mantenemos en el bucle
        keepLoop=true;
      }
      //Las coordenadas actuales pasarán a ser las últimas
      xLast=xCurrent;
      yLast=yCurrent;
    }
    while(keepLoop);
    //Guardamos en *x,*y la media de las coordenadas con menos distancia
    *x=(x0+x1)>>1;
    *y=(y0+y1)>>1;
    //Comprobamos una última vez si hay pulsación
    SPI.transfer16(0xB1);
    //Si la hay...daremos las coordenadas como buenas
    if(SPI.transfer16(0xB1)>500) answer=true;
  }
  //Enviamos comando para finalizar peticiones. 0x90 = 144 = 0b10010000
  //No recogemos nada
  SPI.transfer16(0x00);
  //Desactivamos pin CS
  digitalWrite(_pinCS,HIGH);
  //Fin de transacción
  SPI.endTransaction();
  //Devolvemos respuesta
  return answer;
}

void RoJoXPT2046::setConfig(uint16_t xMaxDisplay,uint16_t yMaxDisplay,uint16_t xMinTS,uint16_t yMinTS,uint16_t xMaxTS,uint16_t yMaxTS)
{
  //Guarda en las variables privadas los valores de calibración para futuros cálculos

  //Tamaño del display
  _xMaxDisplay=xMaxDisplay; //Anchura de display
  _yMaxDisplay=yMaxDisplay; //Altura de display
  //Valores mínimos de coordenadas de hardware para que sean visibles
  _xMinTS=xMinTS;
  _yMinTS=yMinTS;
  //Valores máximos de coordenadas de hardware para que sean visibles
  _xMaxTS=xMaxTS;
  _yMaxTS=yMaxTS;
}

bool RoJoXPT2046::getXY(int16_t *x,int16_t *y)
{
  //Obtiene las coordenadas de pantalla
  //Devuelve si ha detectado pulsación

  //Coordenadas de trabajo
  uint16_t x0,y0;
  //Obtenemos las coordenadas de hardware
  //Si no se detecta pulsación...hemos terminado sin pulsación
  if(!getRawXY(&x0,&y0)) return false;
  //Obtenemos las coordenadas de pantalla sin rotación
  x0=(uint16_t)(((_xMinTS-x0)*_xMaxDisplay)/(_xMinTS-_xMaxTS));
  y0=(uint16_t)(((_yMinTS-y0)*_yMaxDisplay)/(_yMinTS-_yMaxTS));
  //Calculamos las coordenadas en base a la rotación
  switch(_rotation)
  {
    case 0: //0 grados
      *x=x0;
      *y=y0;
      break;
    case 1: //90 grados
      *x=y0;
      *y=_xMaxDisplay-x0;
      break;
    case 2: //180 grados
      *x=_xMaxDisplay-x0;
      *y=_yMaxDisplay-y0;
      break;
    case 3: //270 grados
      *x=_yMaxDisplay-y0;
      *y=x0;
      break;
  }
  //Todo correcto
  return true;
}

void RoJoXPT2046::saveConfig()
{
  //Salva la configuración en un archivo

  //Abrimos el archivo indicado como escritura
  File f=SPIFFS.open(_fileConfig,"w");
  //Escribimos los parámetros de configuración: MaxDisplay,MinTS y MaxTS
  //La clase los guarda en variables internas del tipo uint32_t
  //Pero realmente se podrían guardar en uint16_t
  //Las convertimos para guardarlas en el archivo. Ocuparán menos
  uint16_t z=_xMaxDisplay; f.write((byte *)&z,2); //_xMaxDisplay
  z=_yMaxDisplay; f.write((byte *)&z,2); //_yMaxDisplay
  z=_xMinTS; f.write((byte *)&z,2); //_xMinTS
  z=_yMinTS; f.write((byte *)&z,2); //_yMinTS
  z=_xMaxTS; f.write((byte *)&z,2); //_xMaxTS
  z=_yMaxTS; f.write((byte *)&z,2); //_yMaxTS
  //Cerramos el archivo
  f.close();
}

bool RoJoXPT2046::loadConfig()
{
  //Recupera la configuración desde un archivo
  //Devuelve si ha podido cargar el archivo de configuración

  //Abrimos el archivo indicado como sólo lectura
  File f=SPIFFS.open(_fileConfig,"r");
  //Si hubo algún problema...devolvemos error
  if(!f) return false;
  //Recuperaremos los parámetros de configuración: MaxDisplay,MinTS y MaxTS
  //La clase los guarda en variables internas del tipo uint32_t
  //Pero en el archivo se guardan como uint16_t
  uint16_t z;
  f.readBytes((char *)&z,2); _xMaxDisplay=z;
  f.readBytes((char *)&z,2); _yMaxDisplay=z;
  f.readBytes((char *)&z,2); _xMinTS=z;
  f.readBytes((char *)&z,2); _yMinTS=z;
  f.readBytes((char *)&z,2); _xMaxTS=z;
  f.readBytes((char *)&z,2); _yMaxTS=z;
  //Cerramos el archivo
  f.close();
}

void RoJoXPT2046::rotation(byte rotationCode)
{
  //Fija la rotación de pantalla

  //Guardamos el código de rotación
  _rotation=rotationCode%4;
}

#endif
