/*
  Autor: Ramón Junquera
  Tema: Definición de estructuras de datos en memoria de programa en archivos externos
  Objetivo: Acceder a estructuras complejas almacenadas en memoria de programa
  Material adicional: placa Arduino UNO/Nano

  Descripción:
  En este ejemplo utilizaremos estructuras complejas. 
  Una estructura principal que a su vez contiene arrays de otras estructuras.

  Utilizaremos dos archivos .h
  El primero que cargamos contiene la definición de las estructuras que vamos a utilizar.
  El segundo contiene los datos de las constantes con sus valores.

  Lo que debemos hacer para poder leer es copiar la información desde la memoria de 
  programa a la memoria de datos.

  Comenzamos creando una variable del mismo tipo que la constante definida (abcFont).
  Esto reserva la memoria necesaria en la memoria de datos.
  Copiamos sobreescribimos sus datos con los de la memoria de progama.
  Así tendremos la informacción accesible.

  El problema es que tenemos dos elementos de la estructura que son punteros.
  Los punteros apuntan también a la memoria de programa.

  El primer puntero es a un array de bytes.
  Para acceder a sus elementos utilziamos las funciones pgm*.

  El segundo puntero es a un array de otra estructura.
  Para poder acceder hacermos como al principio... creamos una variable del tipo de esa
  estructura y copiamos su contenido desde la constante.
  
  Nota:
  Los archivo .h son ejemplos reales de definición de fuentes
*/

#include "RoJoStructFont.h" //Definición de estructuras
#include "RoJoABC5x7digits.h" //Datos de estructuras

void setup()
{
  //Abrimos comunicaciones serie para debug
  Serial.begin(115200);

  //Creamos la variable que contendrá el valor leido
  abcFont myFont;
  //Calculamos el tamaño de la estructura abcFont y copiamos su contenido a la variable local
  memcpy_P(&myFont,&RoJoABC5x7digits,sizeof(abcFont));
  //Mostramos su contenido
  Serial.print(F("myFont.charMin="));
  Serial.println(myFont.charMin);
  Serial.print(F("myFont.charMax="));
  Serial.println(myFont.charMax);
  Serial.print(F("myFont.pages="));
  Serial.println(myFont.pages);

  //Mostramos los 10 primeros elementos del array de bytes "bitmap"
  for(unsigned int i=0;i<10;i++)
  {
    Serial.print(F("bitmap["));
    Serial.print(i);
    Serial.print(F("]="));
    Serial.println(pgm_read_byte_near(myFont.bitmap + i),BIN);
  }

  //Creamos una variable con la estructura del array "abcProp"
  abcProperties myProp;
  //Mostramos los 5 primeros elementos del array de estructuras abcProp
  for(int i=0;i<5;i++)
  {
    //Calculamos el tamaño de le estructura y copiamos el contenido del elemento del array a la variable local
    memcpy_P(&myProp,myFont.abcProp+i,sizeof(abcProperties));
    //Mostramos su contenido
    Serial.print(F("myFont.abcProp["));
    Serial.print(i);
    Serial.print(F("]={charIndex="));
    Serial.print(myProp.charIndex);
    Serial.print(F(",width="));
    Serial.print(myProp.width);
    Serial.println(F("}"));
  }
}

void loop()
{

}
