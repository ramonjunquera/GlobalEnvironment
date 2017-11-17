//Estructuras utilizadas en RoJoABC

//Definición de tipo byte
typedef unsigned char byte;

//Estructura para guardar la información de las propiedades de un carácter
typedef struct 
{ 
  unsigned short charIndex; //Índice de inicio en el array gráfico
  byte width; //Anchura en pixels
} abcProperties; //Propiedades de los caracteres

//Estructura para guardar toda la información de una fuente
typedef struct 
{
	byte *bitmap; //Array de gráficos de caracteres
	abcProperties *abcProp; //Array de propiedades de caracteres
	byte charMin; //Primer carácter ASCII incluido en librería
	byte charMax; //Último carácter ASCII incluido en librería
	byte pages; //Altura en páginas
} abcFont;

