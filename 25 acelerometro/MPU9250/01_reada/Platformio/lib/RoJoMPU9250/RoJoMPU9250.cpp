/*
  Nombre de la librería: RoJoMPU9250.h
  Versión: 20200305
  Autor: Ramón Junquera
  Descripción:
    Gestión de sensor MPU9250 por I2C
*/

#ifndef RoJoMPU9250_cpp
#define RoJoMPU9250_cpp

#include <RoJoMPU9250.h>

//Escribe un registro por I2C
void RoJoMPU9250::_write(byte id,byte reg,byte value) {
  Wire.beginTransmission(id);
    Wire.write(reg);
    Wire.write(value);
  Wire.endTransmission();
}

//Lee un registro por I2C
byte RoJoMPU9250::_read(byte id,byte reg) {
  Wire.beginTransmission(id);
    Wire.write(reg); //Solicitamos registro
  Wire.endTransmission(false); //Enviamos la solicitud, pero no cerramos la conexión
  Wire.requestFrom(id,(byte)1); //Esperamos recibir 1 byte
  return Wire.read(); //Devolvemos el valor recibido
}

//Lee N bytes de un registro por I2C
void RoJoMPU9250::_readN(byte id,byte reg,byte count,byte *data) {
  Wire.beginTransmission(id);
    Wire.write(reg); //Solicitamos registro
  Wire.endTransmission(false); //Enviamos la solicitud, pero no cerramos la conexión
  Wire.requestFrom(id,count); //Esperamos recibir "count" bytes
  Wire.readBytes(data,count); //Llemos los bytes esperados
}

//Reset de Giróscopo y Acelerómetro (MPU6500)
//    dps: 0=250DPS, 1=500DPS, 2=1000DPS, 3=2000DPS (DPS=Degrees Per Second)
//    g: 0=2G, 1=4G, 2=8G, 3=16G
void RoJoMPU9250::resetGA(byte dps,byte g) {
  //Salimos de modo hibernación y activamos todos los sensores
  _write(_idGA,0x6B,0x00); //PWR_MGMT_1
  delay(100);
  _write(_idGA,0x6B,0x01); //PWR_MGMT_1. Seleccionamos fuente de reloj: 1=automático
  delay(200);
  _write(_idGA,0x1A,0x03); //CONFIG. Low pass filter 41Hz
  _write(_idGA,0x19,0x04); //SMPLRT_DIV. Frecuencia de toma de datos = 200Hz
  _write(_idGA,0x1B,(dps & 3)<<3); //GYRO_CONFIG. Configuración del Giróscopo
  _write(_idGA,0x1C,(g & 3)<<3); //ACCEL_CONFIG. Configuración del Acelerómetro
  _write(_idGA,0x1D,3); //ACCEL_CONFIG2. Configuración del Acelerómetro: filter rate=41Hz
  delay(100);
}

//Reset de Magnetómetro (AK8963)
//Valores de freq: 0=8Hz (default), 1=100Hz
void RoJoMPU9250::resetM(byte freq) {
  _write(_idM,0x0A,0x00); //AK8963_CNTL. Apagamos el Magnetómetro
  delay(10);
  if(freq>0) _write(_idM,0x0A,0x16); //AK8963_CNTL. Resolución de 16bits y modo contínuo a 100Hz
  else _write(_idM,0x0A,0x12); //AK8963_CNTL. Resolución de 16bits y modo contínuo a 8Hz
  delay(10);
}

//Reset de todo a valores por defecto
void RoJoMPU9250::reset() {
  resetGA();
  resetM();
}

//Modo de hibernación para Giróscopo y Acelerómetro
void RoJoMPU9250::sleepGA(bool status) {
  if(!_hasAG) return; //Si no tiene acelerómetro...hemos terminado
  //Leemos el valor actual del registo PWR_MGMT_1 = 0x6B, que es el que marca el estado
  //de hibernación de MPU6500 en su bit 6
  //Eliminamos el valor del bit 6
  //Activamos/Desactivamos el bit 6 en función del parámetro
  //Escribimos el resultado
  _write(_idGA,0x6B,(_read(_idGA,0x6B) & B10111111) | status?B01000000:0);
}

//Modo de hibernación para Magnetómetro
void RoJoMPU9250::sleepM(bool status) {
  //El magnetómetro necesita activar la interrupciones para funcionar
  //pero éstas pueden afectar al acelerómetro

  if(_hasM && !status) { //Si tiene magnetómetro y no se quiere desactivar...
    _write(_idM,0x0A,0x12); //Magnetómetro activado
    _write(_idGA,0x37,0x22); //Configuración de interrupciones
    _write(_idGA,0x38,0x01);  //Activación de interrupciones
  } else { //Si se debe desactivar el magnetómetro...
    if(_hasM) _write(_idM,0x0A,0x00); //Si tiene magnetómetro...se desactiva
    _write(_idGA,0x37,0); //Configuración de interrupciones (ninguna)
    _write(_idGA,0x38,0);  //Desactivación de interrupciones
  }
}

//Modo de hibernación para todo
void RoJoMPU9250::sleep(bool status) {
  sleepGA(status);
  sleepM(status);
}

//Inicialización
bool RoJoMPU9250::begin(int pinSDA,int pinSCL) {
  //Los dispositivos ESP permiten crear la comunicación I2C en cualquier pin
  //Por eso diferenciamos por tipo de despositivo...
  #ifdef ARDUINO_ARCH_AVR //Si es una placa Arduino
    //No hay posibilidad de seleccionar los pines I2C. Obviamos los parámetros
    Wire.begin();
  #else //Si es un ESP o RPi
    Wire.begin(pinSDA,pinSCL,400000);
  #endif
  Wire.beginTransmission(_idGA);
  _hasAG=!Wire.endTransmission(); //Anotamos si tiene acelerómetro
  if(!_hasAG) { //Si no tiene acelerómetro...
    _hasM=false; //Tampoco magnetómetro
    return false;
  }

  reset(); //Reseteamos todo
  //resetGA(1,0); //Reseteamos el Giróscopo y el Acelerómetro
  //resetM(); //Reseteamos el Magnetómetro
  
  int16_t data[3];
  _hasM=readM(data); //Anotamos si tiene magnetómetro;
  sleepM(!_hasM); //Si tiene magnetómetro, lo activamos

  autoM=true; //Inicialmente activamos la configuración automática del magnetómetro
  for(byte axis=0;axis<2;axis++) { //Asignamos valores de mínimo, máximo y centro para cada eje (XY)
    _rangeM[0][axis]=32767; //Mínimo
    _rangeM[1][axis]=-32767; //Máximo
    midM[axis]=0; //Centro
  }
  return true; //Todo Ok
}

//Lee datos de Giróscopo
void RoJoMPU9250::readG(int16_t *axis) {
  if(!_hasAG) return; //Si no tiene giróscopo...hemos terminado
  byte data[6];
  _readN(_idGA,0x43,6,data); //GYRO_XOUT_H
  axis[0]=((int16_t)data[0] << 8) | data[1];
  axis[1]=((int16_t)data[2] << 8) | data[3];
  axis[2]=((int16_t)data[4] << 8) | data[5]; 
}

//Lee datos de Acelerómetro
void RoJoMPU9250::readA(int16_t *axis) {
  if(!_hasAG) return; //Si no tiene acelerómetro...hemos terminado
  byte data[6];
  _readN(_idGA,0x3B,6,data); //ACCEL_XOUT_H
  axis[0]=(((int16_t)data[0] << 8) | data[1])+offsetA[0];
  axis[1]=(((int16_t)data[2] << 8) | data[3])+offsetA[1];
  axis[2]=(((int16_t)data[4] << 8) | data[5])+offsetA[2];
}

//Lee datos de Magnetómetro
//Devuelve true si los datos leidos son correctos
bool RoJoMPU9250::readM(int16_t *axis) {
  byte data[7];
  //Si los datos ya están listos para ser leidos...
  if(_read(_idM,0x02) & 0x01) { //AK8963_ST1
    //Leemos los valores de los 3 ejes. 2 bytes por eje
    //Leemos un byte extra que nos indicará si los datos son correctos
    _readN(_idM,0x03,7,data); //AK8963_XOUT_L
    //Si no se ha activado el bit de overflow en el último de los bytes leidos...
    //Si los datos leidos son correctos...
    if((data[6] & 0x08)==0) {
      //Leemos los datos recibidos y los parseamos
      axis[0]=((int16_t)data[1] << 8) | data[0];
      axis[1]=((int16_t)data[3] << 8) | data[2];
      axis[2]=((int16_t)data[5] << 8) | data[4];
      
      if(autoM) { //Si está activado el modo de autocalibración...
        bool changed=false;
        for(byte a=0;a<2;a++) { //Recorremos los ejes horizontales (XY)
          if(axis[a]<_rangeM[0][a]) { //Si el valor actual es menor que el mínimo...
            _rangeM[0][a]=axis[a]; //..éste será el nuevo mínimo
            changed=true;
          }
          if(axis[a]>_rangeM[1][a]) { //Si el valor actual es mayor que el máximo...
            _rangeM[1][a]=axis[a]; //..éste será el nuevo máximo
            changed=true;
          }
          //Si hay algún cambio en este eje...recalculamos el centro
          if(changed) midM[a]=((int32_t)_rangeM[0][a]+(int32_t)_rangeM[1][a])/2;
        }
      }
      return true; //Todo correcto
    }
  }
  return false; //Los datos leidos no son correctos
}

//Lee el azimut en grados sexagesimales
//Devuelve -1 si no lo ha podido calcular
int16_t RoJoMPU9250::readAzimut() {
  if(!_hasM) return -1; //Si no tenemos magnetómetro...hemos terminado
  int16_t angle=-1; //Inicialmente no tenemos una respuesta
  int16_t data[3]; //Datos leidos del Magnetómetro
  if(readM(data)) { //Si conseguimos leer los datos del acelerómetro...
    //Calculamos las coordenadas relativas de la medida actual respecto a la media de medidas realizadas
    int16_t x=data[0]-midM[0],y=data[1]-midM[1];
    
    if(x<0) angle=-atan((float)y/(float)x)*180/PI; //Para x negativo
    else if (x>0) angle=180-atan((float)y/(float)x)*180/PI; //Para x positivo
    else angle=(y<0)?270:90; //para x cero

    angle+=offsetAzimut; //Aplicamos la corrección teniendo en cuenta la posición geográfica
    angle=(angle+360)%360; //Los valores se deben encontrar en el rango [0,359]
  }
  return angle;
}

//Lee ángulos de aceleración. Devuelve valores en grados
void RoJoMPU9250::angleA(float *angleX,float *angleY) {
  if(!_hasAG) return; //Si no tiene nacelerómetro...hemos terminado
  int16_t dataA[3]; //Datos de aceleración
  readA(dataA); //Leemos valores de aceleración
  float Ax=dataA[0],Ay=dataA[1],Az=dataA[2]; //Pasamos a valores flotantes
  float Az2=Az*Az; //Cuadrado de Az
  const float rad2deg=180/PI; //Constante de conversión de radianes a grados
  *angleX=atan2(Ay,sqrt(Az2+Ax*Ax))*rad2deg;
  *angleY=atan2(Ax,sqrt(Az2+Ay*Ay))*rad2deg;
}

//Tiene magnetómetro?
bool RoJoMPU9250::hasM() {
  return _hasM;
}

//Tiene acelerómetro?
bool RoJoMPU9250::hasAG() {
  return _hasAG;
}

#endif
