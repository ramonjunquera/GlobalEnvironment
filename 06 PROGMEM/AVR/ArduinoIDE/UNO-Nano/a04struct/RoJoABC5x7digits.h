//Datos de la fuente de 5x7 pixels. Sólo dígitos

const byte RoJoABC5x7digitsBitmap[] PROGMEM =
{
      B00111110, //0x30 = '0'
      B01010001,
      B01001001,
      B01000101,
      B00111110,
      B00000000, //0x31 = '1'
      B01000010,
      B01111111,
      B01000000,
      B00000000,
      B01000010, //0x32 = '2'
      B01100001,
      B01010001,
      B01001001,
      B01000110,
      B00100001, //0x33 = '3'
      B01000001,
      B01000101,
      B01001011,
      B00110001,
      B00011000, //0x34 = '4'
      B00010100,
      B00010010,
      B01111111,
      B00010000,
      B00100111, //0x35 = '5'
      B01000101,
      B01000101,
      B01000101,
      B00111001,
      B00111100, //0x36 = '6'
      B01001010,
      B01001001,
      B01001001,
      B00110000,
      B00000011, //0x37 = '7'
      B00000001,
      B01110001,
      B00001001,
      B00000111,
      B00110110, //0x38 = '8'
      B01001001,
      B01001001,
      B01001001,
      B00110110,
      B00000110, //0x39 = '9'
      B01001001,
      B01001001,
      B00101001,
      B00011110
};

//Definición de propiedades de los caracteres
//Indica el índice del array gráfico donde comienza el carácter y su anchura
//En esta fuente todos los caracteres tienen la misma anchura (=5)
const abcProperties RoJoABC5x7digitsProps[] PROGMEM =
{
  {0,5},  //0x30 = '0'
  {5,5},  //0x31 = '1'
  {10,5}, //0x32 = '2'
  {15,5}, //0x33 = '3'
  {20,5}, //0x34 = '4'
  {25,5}, //0x35 = '5'
  {30,5}, //0x36 = '6'
  {35,5}, //0x37 = '7'
  {40,5}, //0x38 = '8'
  {45,5}  //0x39 = '9'
};  

const abcFont RoJoABC5x7digits PROGMEM =
{
  (byte *)RoJoABC5x7digitsBitmap //Bitmap
  ,(abcProperties *)RoJoABC5x7digitsProps //Propiedades
  ,'0' //Primer carácter
  ,'9' //Último carácter
  ,1 //Altura en páginas = 8 pixels
};

