/*
  Autor: Ramón Junquera
  Fecha: 20210416
  Tema: Transformada de Fourier
  Descripción:
    Varios ejemplos para demostración de uso de librería.
*/

#include <Arduino.h>
#include <RoJoFFT.h> //Transformada de Fourier

//Declaración de variable globales
RoJoFFT FFT; //Objeto de gestión de la transformada de Fourier
RoJoFloatMatrix ampli; //Matriz de amplitudes
RoJoFloatMatrix freqs; //Matriz de frecuencias calculadas

//Calcula y muestra los resultados
void test(RoJoFloatMatrix *ampli,float samplingFreq) {
  Serial.println("muestras="+String(ampli->cols()));
  Serial.println("frecuencia de muestreo="+String(samplingFreq));
  //Cálculo de frecuencias máximas
  byte errorCode=FFT.begin(ampli,samplingFreq,&freqs);
  if(errorCode) Serial.println("errorCode ejemplo 1="+String(errorCode));
  //Listamos todas las frecuencias detectadas junto con su amplitud
  Serial.println("# / frecuencia / amplitud");
  for(uint16_t i=0;i<freqs.rows();i++) {
    Serial.print(String(i)+" / ");
    Serial.print(freqs.m[i][0],6); //Mostramos frecuencia
    Serial.print(" / ");
    Serial.println(freqs.m[i][1],6); //Mostramos amplitud
  }
  Serial.println(); //Línea de separación entre ejemplos
}

//Prueba estándar
void test01() {
  byte samples2pow=7; //2⁷=128 muestras
  float signalFrequency=1000;
  float samplingFrequency=20000;
  float amplitude=100;
  
  uint16_t samples=1<<samples2pow; //Cálculo del número de muestras
  //Matriz fila con tantas columnas como muestras. Si no se puede...error
  if(!ampli.redim(1,samples)) Serial.println("Sin memoria para matriz de amplitudes en test01");
  float cycles=samples*signalFrequency/samplingFrequency; //Cálculo de ciclos
  for (uint16_t i=0;i<samples;i++) { //Recorremos todas las muestras...
    ampli.m[0][i]=amplitude*sin(i*2*PI*cycles/samples); //Calculamos las muestras de la función seno
  }
  Serial.println("test01. frecuencia única");
  Serial.println("frecuencia="+String(signalFrequency));
  Serial.println("amplitud="+String(amplitude));
  test(&ampli,samplingFrequency);
  ampli.end(); //Liberamos memoria de matriz de amplitudes
}

//Duplicamos la amplitud de test01
void test02() {
  byte samples2pow=7; //2⁷=128 muestras
  float signalFrequency=1000;
  float samplingFrequency=20000;
  float amplitude=200; //El doble que en el ejemplo anterior
  
  uint16_t samples=1<<samples2pow; //Cálculo del número de muestras
  //Matriz fila con tantas columnas como muestras. Si no se puede...error
  if(!ampli.redim(1,samples)) Serial.println("Sin memoria para matriz de amplitudes en test02");

  float cycles=samples*signalFrequency/samplingFrequency; //Cálculo de ciclos
  for (uint16_t i=0;i<samples;i++) { //Recorremos todas las muestras...
    ampli.m[0][i]=amplitude*sin(i*2*PI*cycles/samples); //Calculamos las muestras de la función seno
  }
  Serial.println("test02. frecuencia única");
  Serial.println("frecuencia="+String(signalFrequency));
  Serial.println("amplitud="+String(amplitude));
  test(&ampli,samplingFrequency);
  ampli.end(); //Liberamos memoria de matriz de amplitudes
}

//Duplicamos la frecuencia de test01
void test03() {
  byte samples2pow=7; //2⁷=128 muestras
  float signalFrequency=2000;
  float samplingFrequency=20000;
  float amplitude=100;
  
  uint16_t samples=1<<samples2pow; //Cálculo del número de muestras
  //Matriz fila con tantas columnas como muestras. Si no se puede...error
  if(!ampli.redim(1,samples)) Serial.println("Sin memoria para matriz de amplitudes en test03");

  float cycles=samples*signalFrequency/samplingFrequency; //Cálculo de ciclos
  for (uint16_t i=0;i<samples;i++) { //Recorremos todas las muestras...
    ampli.m[0][i]=amplitude*sin(i*2*PI*cycles/samples); //Calculamos las muestras de la función seno
  }
  Serial.println("test03. frecuencia única");
  Serial.println("frecuencia="+String(signalFrequency));
  Serial.println("amplitud="+String(amplitude));
  test(&ampli,samplingFrequency);
  ampli.end(); //Liberamos memoria de matriz de amplitudes
}

//Reducimos la frecuencio de muestreo a la mitad de test01
void test04() {
  byte samples2pow=7; //2⁷=128 muestras
  float signalFrequency=1000;
  float samplingFrequency=10000;
  float amplitude=100;
  
  uint16_t samples=1<<samples2pow; //Cálculo del número de muestras
  //Matriz fila con tantas columnas como muestras. Si no se puede...error
  if(!ampli.redim(1,samples)) Serial.println("Sin memoria para matriz de amplitudes en test04");

  float cycles=samples*signalFrequency/samplingFrequency; //Cálculo de ciclos
  for (uint16_t i=0;i<samples;i++) { //Recorremos todas las muestras...
    ampli.m[0][i]=amplitude*sin(i*2*PI*cycles/samples); //Calculamos las muestras de la función seno
  }
  Serial.println("test04. frecuencia única");
  Serial.println("frecuencia="+String(signalFrequency));
  Serial.println("amplitud="+String(amplitude));
  test(&ampli,samplingFrequency);
  ampli.end(); //Liberamos memoria de matriz de amplitudes
}

//Duplicamos el número de muestras de test01
void test05() {
  byte samples2pow=8; //2⁸=256 muestras
  float signalFrequency=1000;
  float samplingFrequency=20000; //20 muestras por ciclo
  float amplitude=100;
  
  uint16_t samples=1<<samples2pow; //Cálculo del número de muestras
  //Matriz fila con tantas columnas como muestras. Si no se puede...error
  if(!ampli.redim(1,samples)) Serial.println("Sin memoria para matriz de amplitudes en test05");

  float cycles=samples*signalFrequency/samplingFrequency; //Cálculo de ciclos
  for (uint16_t i=0;i<samples;i++) { //Recorremos todas las muestras...
    ampli.m[0][i]=amplitude*sin(i*2*PI*cycles/samples); //Calculamos las muestras de la función seno
  }
  Serial.println("test05. frecuencia única");
  Serial.println("frecuencia="+String(signalFrequency));
  Serial.println("amplitud="+String(amplitude));
  test(&ampli,samplingFrequency);
  ampli.end(); //Liberamos memoria de matriz de amplitudes
}

//Multiplicamos por 4 el número de muestras de test01
void test06() {
  byte samples2pow=9; //2⁹=512 muestras
  float signalFrequency=1000;
  float samplingFrequency=20000; //20 muestras por ciclo
  float amplitude=100;
  
  uint16_t samples=1<<samples2pow; //Cálculo del número de muestras
  //Matriz fila con tantas columnas como muestras. Si no se puede...error
  if(!ampli.redim(1,samples)) Serial.println("Sin memoria para matriz de amplitudes en test06");

  float cycles=samples*signalFrequency/samplingFrequency; //Cálculo de ciclos
  for (uint16_t i=0;i<samples;i++) { //Recorremos todas las muestras...
    ampli.m[0][i]=amplitude*sin(i*2*PI*cycles/samples); //Calculamos las muestras de la función seno
  }
  Serial.println("test06. frecuencia única");
  Serial.println("frecuencia="+String(signalFrequency));
  Serial.println("amplitud="+String(amplitude));
  test(&ampli,samplingFrequency);
  ampli.end(); //Liberamos memoria de matriz de amplitudes
}

//Reducimos la frecuencia de muestreo de test01 a 3 muestras por ciclo
void test07() {
  byte samples2pow=7; //2⁷=128 muestras
  float signalFrequency=1000;
  float samplingFrequency=3000; //3 muestras por ciclo
  float amplitude=100;
  
  uint16_t samples=1<<samples2pow; //Cálculo del número de muestras
  //Matriz fila con tantas columnas como muestras. Si no se puede...error
  if(!ampli.redim(1,samples)) Serial.println("Sin memoria para matriz de amplitudes en test07");

  float cycles=samples*signalFrequency/samplingFrequency; //Cálculo de ciclos
  for (uint16_t i=0;i<samples;i++) { //Recorremos todas las muestras...
    ampli.m[0][i]=amplitude*sin(i*2*PI*cycles/samples); //Calculamos las muestras de la función seno
  }
  Serial.println("test07. frecuencia única");
  Serial.println("frecuencia="+String(signalFrequency));
  Serial.println("amplitud="+String(amplitude));
  test(&ampli,samplingFrequency);
  ampli.end(); //Liberamos memoria de matriz de amplitudes
}

//Prueba con frecuencias altas
void test08() {
  byte samples2pow=9; //2⁹=512 muestras
  float signalFrequency=4000;
  float samplingFrequency=20000;
  float amplitude=300;
  
  uint16_t samples=1<<samples2pow; //Cálculo del número de muestras
  //Matriz fila con tantas columnas como muestras. Si no se puede...error
  if(!ampli.redim(1,samples)) Serial.println("Sin memoria para matriz de amplitudes en test08");

  float cycles=samples*signalFrequency/samplingFrequency; //Cálculo de ciclos
  for (uint16_t i=0;i<samples;i++) { //Recorremos todas las muestras...
    ampli.m[0][i]=amplitude*sin(i*2*PI*cycles/samples); //Calculamos las muestras de la función seno
  }
  Serial.println("test08. frecuencia única");
  Serial.println("frecuencia="+String(signalFrequency));
  Serial.println("amplitud="+String(amplitude));
  test(&ampli,samplingFrequency);
  ampli.end(); //Liberamos memoria de matriz de amplitudes
}

//Prueba con onda descentrada
void test09() {
  byte samples2pow=9; //2⁹=512 muestras
  float signalFrequency=4000;
  float samplingFrequency=20000;
  float amplitude=300;
  
  uint16_t samples=1<<samples2pow; //Cálculo del número de muestras
  //Matriz fila con tantas columnas como muestras. Si no se puede...error
  if(!ampli.redim(1,samples)) Serial.println("Sin memoria para matriz de amplitudes en test09");

  float cycles=samples*signalFrequency/samplingFrequency; //Cálculo de ciclos
  for (uint16_t i=0;i<samples;i++) { //Recorremos todas las muestras...
    //Descentramos la onda. Está 40 unidades por encima del 0
    ampli.m[0][i]=40+amplitude*sin(i*2*PI*cycles/samples); //Calculamos las muestras de la función seno
  }
  Serial.println("test09. frecuencia única. onda descentrada");
  Serial.println("frecuencia="+String(signalFrequency));
  Serial.println("amplitud="+String(amplitude));
  test(&ampli,samplingFrequency);
  ampli.end(); //Liberamos memoria de matriz de amplitudes
}

//Suma de dos frecuencias
void test10() {
  //Frecuencia de muestreo: 20000Hz
  //Muestras: 128 = 2⁷
  //Frecuencia a 1000Hz de amplitud 100
  //Frecuencia a 2000Hz de amplitud 100
  byte samples2pow=7; //2⁷=128 muestras
  float samplingFrequency=20000; //20 muestras por ciclo
  
  uint16_t samples=1<<samples2pow; //Cálculo del número de muestras
  //Matriz fila con tantas columnas como muestras. Si no se puede...error
  if(!ampli.redim(1,samples)) Serial.println("Sin memoria para matriz de amplitudes en test10");

  Serial.println("test10. 2 frecuencias");
  Serial.println("frecuencia / amplitud");

  float signalFrequency=1000;
  float amplitude=100;
  Serial.println(String(signalFrequency)+" / "+String(amplitude));

  float cycles=samples*signalFrequency/samplingFrequency; //Cálculo de ciclos
  for (uint16_t i=0;i<samples;i++) { //Recorremos todas las muestras...
    ampli.m[0][i]=amplitude*sin(i*2*PI*cycles/samples); //Calculamos las muestras de la primera frecuencia
  }

  signalFrequency=2000;
  amplitude=100;
  Serial.println(String(signalFrequency)+" / "+String(amplitude));

  cycles=samples*signalFrequency/samplingFrequency; //Cálculo de ciclos
  for (uint16_t i=0;i<samples;i++) { //Recorremos todas las muestras...
    ampli.m[0][i]+=amplitude*sin(i*2*PI*cycles/samples); //Calculamos las muestras de la segunda frecuencia
  }
  test(&ampli,samplingFrequency);
  ampli.end(); //Liberamos memoria de matriz de amplitudes
}

//Reducimos la amplitud de la segunda onda a la mitad
void test11() {
  //Frecuencia de muestreo: 20000Hz
  //Muestras: 128 = 2⁷
  //Frecuencia a 1000Hz de amplitud 100
  //Frecuencia a 2000Hz de amplitud 50
  byte samples2pow=7; //2⁷=128 muestras
  float samplingFrequency=20000; //20 muestras por ciclo
  
  uint16_t samples=1<<samples2pow; //Cálculo del número de muestras
  //Matriz fila con tantas columnas como muestras. Si no se puede...error
  if(!ampli.redim(1,samples)) Serial.println("Sin memoria para matriz de amplitudes en test11");

  Serial.println("test11. 2 frecuencias");
  Serial.println("frecuencia / amplitud");

  float signalFrequency=1000;
  float amplitude=100;
  Serial.println(String(signalFrequency)+" / "+String(amplitude));

  float cycles=samples*signalFrequency/samplingFrequency; //Cálculo de ciclos
  for (uint16_t i=0;i<samples;i++) { //Recorremos todas las muestras...
    ampli.m[0][i]=amplitude*sin(i*2*PI*cycles/samples); //Calculamos las muestras de la primera frecuencia
  }

  signalFrequency=2000;
  amplitude=50;
  Serial.println(String(signalFrequency)+" / "+String(amplitude));

  cycles=samples*signalFrequency/samplingFrequency; //Cálculo de ciclos
  for (uint16_t i=0;i<samples;i++) { //Recorremos todas las muestras...
    ampli.m[0][i]+=amplitude*sin(i*2*PI*cycles/samples); //Calculamos las muestras de la segunda frecuencia
  }
  test(&ampli,samplingFrequency);
  ampli.end(); //Liberamos memoria de matriz de amplitudes
}

//Utilizamos 4 veces más de muestras
void test12() {
  //Frecuencia de muestreo: 20000Hz
  //Muestras: 512 = 2⁹
  //Frecuencia a 1000Hz de amplitud 100
  //Frecuencia a 2000Hz de amplitud 50
  byte samples2pow=9; //2⁹=512 muestras
  float samplingFrequency=20000; //20 muestras por ciclo
  
  uint16_t samples=1<<samples2pow; //Cálculo del número de muestras
  //Matriz fila con tantas columnas como muestras. Si no se puede...error
  if(!ampli.redim(1,samples)) Serial.println("Sin memoria para matriz de amplitudes en test12");

  Serial.println("test12. 2 frecuencias");
  Serial.println("frecuencia / amplitud");

  float signalFrequency=1000;
  float amplitude=100;
  Serial.println(String(signalFrequency)+" / "+String(amplitude));

  float cycles=samples*signalFrequency/samplingFrequency; //Cálculo de ciclos
  for (uint16_t i=0;i<samples;i++) { //Recorremos todas las muestras...
    ampli.m[0][i]=amplitude*sin(i*2*PI*cycles/samples); //Calculamos las muestras de la primera frecuencia
  }

  signalFrequency=2000;
  amplitude=50;
  Serial.println(String(signalFrequency)+" / "+String(amplitude));

  cycles=samples*signalFrequency/samplingFrequency; //Cálculo de ciclos
  for (uint16_t i=0;i<samples;i++) { //Recorremos todas las muestras...
    ampli.m[0][i]+=amplitude*sin(i*2*PI*cycles/samples); //Calculamos las muestras de la segunda frecuencia
  }
  test(&ampli,samplingFrequency);
  ampli.end(); //Liberamos memoria de matriz de amplitudes
}

//Probamos con frecuencias desfasadas (no comienzan en 0)
void test13() {
  //Frecuencia de muestreo: 20000Hz
  //Muestras: 512 = 2⁹
  //Frecuencia a 1000Hz de amplitud 100
  //Frecuencia a 2000Hz de amplitud 50
  byte samples2pow=9; //2⁹=512 muestras
  float samplingFrequency=20000; //20 muestras por ciclo
  
  uint16_t samples=1<<samples2pow; //Cálculo del número de muestras
  //Matriz fila con tantas columnas como muestras. Si no se puede...error
  if(!ampli.redim(1,samples)) Serial.println("Sin memoria para matriz de amplitudes en test13");

  Serial.println("test13. 2 frecuencias. Ondas desfasadas");
  Serial.println("frecuencia / amplitud");

  float signalFrequency=1000;
  float amplitude=100;
  Serial.println(String(signalFrequency)+" / "+String(amplitude));

  float cycles=samples*signalFrequency/samplingFrequency; //Cálculo de ciclos
  for (uint16_t i=0;i<samples;i++) { //Recorremos todas las muestras...
    ampli.m[0][i]=amplitude*sin(i*2*PI*cycles/samples); //Calculamos las muestras de la primera frecuencia
  }

  signalFrequency=2000;
  amplitude=50;
  Serial.println(String(signalFrequency)+" / "+String(amplitude));

  cycles=samples*signalFrequency/samplingFrequency; //Cálculo de ciclos
  for (uint16_t i=0;i<samples;i++) { //Recorremos todas las muestras...
    //Incluimo un desfase de 1 radián!  
    ampli.m[0][i]+=amplitude*sin(1+i*2*PI*cycles/samples); //Calculamos las muestras de la segunda frecuencia
  }
  test(&ampli,samplingFrequency);
  ampli.end(); //Liberamos memoria de matriz de amplitudes
}

//Suma de tres frecuencias
void test14() {
  //Frecuencia de muestreo: 20000Hz
  //Muestras: 512 = 2⁹
  //Frecuencia a 1000Hz de amplitud 100
  //Frecuencia a 2000Hz de amplitud 50
  //Frecuencia a 4000Hz de amplitud 300
  byte samples2pow=9; //2⁹=512 muestras
  float samplingFrequency=20000; //20 muestras por ciclo
  
  uint16_t samples=1<<samples2pow; //Cálculo del número de muestras
  //Matriz fila con tantas columnas como muestras. Si no se puede...error
  if(!ampli.redim(1,samples)) Serial.println("Sin memoria para matriz de amplitudes en test13");

  Serial.println("test14. 3 frecuencias");
  Serial.println("frecuencia / amplitud");

  float signalFrequency=1000;
  float amplitude=100;
  Serial.println(String(signalFrequency)+" / "+String(amplitude));

  float cycles=samples*signalFrequency/samplingFrequency; //Cálculo de ciclos
  for (uint16_t i=0;i<samples;i++) { //Recorremos todas las muestras...
    ampli.m[0][i]=amplitude*sin(i*2*PI*cycles/samples); //Calculamos las muestras de la primera frecuencia
  }

  signalFrequency=2000;
  amplitude=50;
  Serial.println(String(signalFrequency)+" / "+String(amplitude));

  cycles=samples*signalFrequency/samplingFrequency; //Cálculo de ciclos
  for (uint16_t i=0;i<samples;i++) { //Recorremos todas las muestras...
    ampli.m[0][i]+=amplitude*sin(i*2*PI*cycles/samples); //Calculamos las muestras de la segunda frecuencia
  }

  signalFrequency=4000;
  amplitude=300;
  Serial.println(String(signalFrequency)+" / "+String(amplitude));

  cycles=samples*signalFrequency/samplingFrequency; //Cálculo de ciclos
  for (uint16_t i=0;i<samples;i++) { //Recorremos todas las muestras...
    ampli.m[0][i]+=amplitude*sin(i*2*PI*cycles/samples); //Calculamos las muestras de la primera frecuencia
  }

  test(&ampli,samplingFrequency);
  ampli.end(); //Liberamos memoria de matriz de amplitudes
}

void setup() {
  Serial.begin(115200);
  delay(3000);

  //Pruebas con frecuencias únicas
  //          signalFreq  ampli samplingFreq samples notas
  test01(); //      1000    100        20000     128
  test02(); //      2000    200        20000     128
  test03(); //      2000    100        20000     128
  test04(); //      1000    100        10000     128
  test05(); //      1000    100        20000     256
  test06(); //      1000    100        20000     512
  test07(); //      1000    100         3000     128
  test08(); //      4000    300        20000     512
  test09(); //      4000    300        20000     512 onda descentrada
  //Pruebas con 2 frecuencias
  //          signalFreq1 ampli1 signalFreq2 ampli2 samplingFreq samples notas
  test10(); //       1000    100        2000    100        20000     128
  test11(); //       1000    100        2000     50        20000     128
  test12(); //       1000    100        2000     50        20000     512
  test13(); //       1000    100        2000     50        20000     512 ondas desfasadas
  //Pruebas con 3 frecuencias
  //          signalFreq1 ampli1 signalFreq2 ampli2 signalFreq3 ampli3 samplingFreq samples
  test14();  //      1000    100        2000     50        4000    300        20000     512
}

void loop() {
  //Nada que hacer aquí
}

