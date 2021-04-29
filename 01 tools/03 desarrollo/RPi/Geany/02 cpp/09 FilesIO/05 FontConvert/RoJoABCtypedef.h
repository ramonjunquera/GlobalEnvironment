//Estructuras utilizadas en RoJoABC

//Definici�n de tipo byte
typedef unsigned char byte;

//Estructura para guardar la informaci�n de las propiedades de un car�cter
typedef struct 
{ 
  unsigned short charIndex; //�ndice de inicio en el array gr�fico
  byte width; //Anchura en pixels
} abcProperties; //Propiedades de los caracteres

//Estructura para guardar toda la informaci�n de una fuente
typedef struct 
{
	byte *bitmap; //Array de gr�ficos de caracteres
	abcProperties *abcProp; //Array de propiedades de caracteres
	byte charMin; //Primer car�cter ASCII incluido en librer�a
	byte charMax; //�ltimo car�cter ASCII incluido en librer�a
	byte pages; //Altura en p�ginas
} abcFont;

