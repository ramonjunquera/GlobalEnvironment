 /*
  Definición de estructuras de fuentes
*/

//Definición de estructuras de datos para almacenar fuentes
typedef struct //Estructura para guardar la información de las propiedades de un carácter
{ 
  int charIndex; //Índice de inicio en el array gráfico
  byte width; //Anchura en pixels
} abcProperties; //Propiedades de los caracteres

typedef struct //Estructura para guardar toda la información de una fuente
{ 
  byte *bitmap; //Array de gráficos de caracteres
  abcProperties *abcProp; //Array de propiedades de caracteres
  byte charMin; //Primer carácter ASCII incluido en librería
  byte charMax; //Último carácter ASCII incluido en librería
  byte pages; //Altura en páginas
} abcFont;
