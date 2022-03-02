/*
  Tema: PCA9685 gestor de PWM
  Fecha: 20220216
  Autor: Ramón Junquera
  Objetivo: Encadenar módulos PCA9685
  
  Descripción:
  Es este ejemplo encadenaremos dos módulos PCA9685 y comprobaremos su funcionamiento.
  Como explicamos anteriormente, se pueden encadenar hasta un máximo de 62 módulos PCA9685, obteniendo 64*16=1024 salidas PWM
  En este caso sólo encadenaremos 2 módulos.
  De cada módulo sólo utilizaremos dos canales. El primero y el octavo.

  Direccionamiento.
  La dirección I2C del módulo por defecto es la 0x40.
  Si queremos utilizar más de un módulo simultáneamente necesitaremos que tengan direcciones distintas!
  Para ello tenemos en la parte superior derecha del módulo 6 jumpers etiquetados como A5 A4 A3 A3 A1 y A0
  Si los soldamos cambiará dirección.
  El valor que suma cada jumper es el siguiente:
  
  jumper valor
  ------ -----
    A0     1
    A1     2
    A2     4
    A3     8
    A4    16
    A5    32

  Por ejemplo. Si soldamos el jumper A2, la dirección I2C pasará a ser la 0x40+4=0x44.

  En nuestro ejemplo el primer módulo (el que se conecta directamente a la placa Arduino) tiene la dirección por defecto (0x40)
  y el siguiente tiene la 0x41, porque tiene soldado el jumper A0.

  El módulo tiene los pines de gestión duplicados a ambos lados. Son exáctamente los mismos.
  No es obligatorio que el segundo módulo esté conectado a los pines del lado derecho del anterior.
  Esto es un bus I2C, sólo se deben compartir los dos cables de datos SDA y SCL.
  
  Resultado:
  Comprobamos que podemos gestionar más de un módulo por el bus I2C
*/

#include <Arduino.h>
#include <RoJoPCA9685.h>

//Creamos un objeto de gestión de pwm para cada módulo PCA9685
RoJoPCA9685 pwm40,pwm41(0x41);

void setup() {
  //Inicializamos los objetos de gestión
  pwm40.begin();
  pwm41.begin();
}
 
void loop() {
  int level,delta;
  
  for(byte c=0;c<2;c++) { //Recorreremos los 2 canales activos para el módulo 0x40
    delta=1; //El sentido será ascendente
    level=1; //Comenzamos desde nivel 1
    while(level>0) { //Mientras no volvamos a nivel cero...
      pwm40.setPWM(c*8,level); //Aplicamos nivel al canal
      if(level==4095) delta=-delta; //Si el nivel es el último...cambiamos de dirección
      level+=delta; //Calculamos el siguiente nivel
    }
  }
  for(byte c=0;c<2;c++) { //Recorreremos los 2 canales activos para el módulo 0x41
    delta=1; //El sentido será ascendente
    level=1; //Comenzamos desde nivel 1
    while(level>0) { //Mientras no volvamos a nivel cero...
      pwm41.setPWM(c*8,level); //Aplicamos nivel al canal
      if(level==4095) delta=-delta; //Si el nivel es el último...cambiamos de dirección
      level+=delta; //Calculamos el siguiente nivel
    }
  }
}
