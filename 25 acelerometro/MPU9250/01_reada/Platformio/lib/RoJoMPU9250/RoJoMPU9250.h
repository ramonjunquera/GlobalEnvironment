/*
  Nombre de la librería: RoJoMPU9250.h
  Versión: 20200305
  Autor: Ramón Junquera
  Descripción:
    Gestión de sensor MPU9250 por I2C
  Compatibilidad:
    Giróscopo/Acelerómetro: MPU6050, MPU6886
    Magnetómetro: AK8963

  Magnetómetro.
    El magnetómetro está compuesto por tres elementos magnetoresistivos, posicionados
    cada uno en un eje cartesiano.
    El campo magnético terrestre no tiene demasiada intensidad, teniendo en cuenta que
    la aproximación de un imán, o un elemento metálico (como un conector de alimentación),
    es capaz de desvirtuar la medida.
    Se denomina azimut al ángulo que forma la dirección del norte con nuestra dirección
    en sentido horario.
    Cuando nuestro objetivo es determinar nuestro azimut nos encontramos con varios problemas:
    - El norte magnético terrestre está en contínuo movimiento, y sólo algunas veces coincide
      con el norte geográfico. Para simplificar el problema, asumiremos que el norte magnético
      coincide con el geográfico.
    - Las líneas de campo magnético terrestre no están distribuidas de manera uniforme.
      Podemos encontrar un mapa bastante actualizado en:
        https://www.ngdc.noaa.gov/geomag/WMM/data/WMM2015/WMM2015_D_MERC.pdf
      Para obtener una aproximación de corrección al azimut teniendo en cuenta nuestra
      posición geográfica podemos visitar la web:
        http://www.magnetic-declination.com/
      En España la corrección es muy pequeña (inferior al grado). Podríamos considerar que es 0.
      El valor de la corrección se tiene en cuenta en la variable fizAzimut de la clase.
    - La coordenada Z del magnetómetro mide la intensidad del campo magnético en dirección
      perpendicular al suelo (cuando el dispositivo está horizontal). Este valor no tiene
      relevancia para el cálculo del azimut, por lo tanto lo descartaremos.
    - Si los únicos datos que podemos obtener son los valores de intensidad de campo en los 
      ejes X e Y, entendemos que si alineamos un eje con el norte magnético obtendremos la medida
      más alta posible. Si lo alineamos, pero en sentido contrario obtendremos la medida más baja
      posible. En cualquier otro caso, la medida estará entre estos límites.
      Por lo tanto, si queremos saber la orientación de la medida recien tomada, necesitamos
      conocer de antemano los límites del rango de medidas.
      Las coordenadas de punto medio del rango de medidas será aquella que nos servirá de base
      para compara cualquier medida tomada.
      En la clase, estas coordenadas se encuentran en midM[]:
        midM[0] = coordenada x del punto medio
        midM[1] = coordenada y del punto medio
      La clase tiene una variable adicional llamada autoM que indica si el cálculo del punto
      medio se realiza de forma automática o manual.
      De forma manual (autoM==false) se toma directamente el contenido de midM sin hacer más cálculos.
      De forma automática, cada vez que se solicite una medida al magnetómetro se comparará con
      unos valores mínimos y máximos de cada eje, se actualizarán si es necesario y se recalcularán
      las coordenadas del punto medio.
      Estos valores de límite de rango son privados de la clase y no pueden ser consultados
      desde el exterior.
      Si se utiliza el sistema de calibración automática, antes de tener en cuenta las medidas, se
      debería dar una vuelta completa al dispositivo, manteniéndolo en horizontal, para que
      fije límites y obtenga valores fiables para el punto medio.
*/

#ifndef RoJoMPU9250_h
#define RoJoMPU9250_h

#include <Arduino.h>
#include <Wire.h>

class RoJoMPU9250 {
  private:
    const byte _idGA=0x68; //Identificador I2C de MPU6500 (Giróscopo y Acelerómetro)
    const byte _idM=0x0C; //Identificador I2C de AK8963 (Magnetómetro)
    void _write(byte id,byte reg,byte value); //Escribe un registro por I2C
    byte _read(byte id,byte reg); //Lee un registro por I2C
    void _readN(byte id,byte reg,byte count,byte *data); //Lee N bytes de un registro por I2C
    int16_t _rangeM[2][2]; //Rango de medidas XY del Magnetómetro [min/max][axis]
    bool _hasM,_hasAG; //Tiene magnetómetro / acelerómetro?
  public:
    void resetGA(byte dps=3,byte g=3); //Reset de Giróscopo y Acelerómetro
    void resetM(byte freq=1); //Reset de Magnetómetro
    void reset(); //Resetea todo a valores por defecto
    bool begin(int pinSDA=-1,int pinSCL=-1); //Inicialización
    void sleepGA(bool status=true); //Modo de hibernación de Giróscopo y Acelerómetro
    void sleepM(bool status=true); //Modo de hibernación de Magnetómetro
    void sleep(bool status=true); //Modo de hibernación de todo
    void readG(int16_t *axis); //Lee datos de Giróscopo
    void readA(int16_t *axis); //Lee datos de Acelerómetro
    bool readM(int16_t *axis); //Lee datos de Magnetómetro
    bool autoM=true; //Calibración automática del magnetómetro
    int16_t midM[2]; //Coordenadas medias las muestras del Magnetómetro
    int16_t readAzimut(); //Lee el azimut en grados sexagesimales
    int16_t offsetAzimut=0; //Corrección del azimut dependiendo de la posición geográfica
    int16_t offsetA[3]={0,0,0}; //Corrección de los valores de aceleración
    void angleA(float *angleX,float *angleY); //Lee ángulos de aceleración
    bool hasM(); //Tiene magnetómetro?
    bool hasAG(); //Tiene acelerómetro?
    
}; //Punto y coma obligatorio para que no de error

#ifdef __arm__
  #include <RoJoMPU9250.cpp> //Para guardar compatibilidad con RPi
#endif

#endif

