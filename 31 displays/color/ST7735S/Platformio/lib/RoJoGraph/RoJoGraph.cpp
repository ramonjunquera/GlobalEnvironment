/*
  Nombre de la librería: RoJoGraph.h
  Versión: 20190906
  Autor: Ramón Junquera
  Descripción:
    Funciones gráficas avanzadas
*/

#ifndef RoJoGraph_cpp
#define RoJoGraph_cpp

#include <RoJoGraph.h>

//Inicialización
void RoJoGraph::begin() {
  //Creamos este método porque en ESP32 no se puede inicializar SPIFFS
  //dentro de un constructor. Da un error en tiempo de ejecución
  //Este método debe ser llamado siempre por las clases hijas

  //Si no utilizamos SD (si utilizamos SPIFFS)...
  #ifndef ROJO_PIN_CS_SD
    //...lo inicializamos
    SPIFFS.begin();
  #endif
}

//Anchura de display (dummy)
uint16_t RoJoGraph::xMax() {
  //Necesario para calcular rangos visibles del display y clear
  return 0;
}

//Altura de display (dummy)
uint16_t RoJoGraph::yMax() {
  //Necesario para calcular rangos visibles del display y clear
  return 0;
}

//Calcula el rango visible de un objeto
//Parámetros:
//  x,y : coordenadas del objeto
//  xx,yy : anchura y altura del objeto
//Respuesta: estructura con las coordenadasde display ocupadas por objeto
displayRange RoJoGraph::visibleRange(int16_t x,int16_t y,uint16_t xx,uint16_t yy) {
  //Anotamos la anchura y altura del display
  uint16_t dxx=xMax();
  uint16_t dyy=yMax();
  //Creamos estructura de respuesta
  displayRange answer;
  //Si el objeto está fuera de pantalla...no será visible
  if(x>=(int16_t)dxx || y>=(int16_t)dyy || x+xx-1<0 || y+yy-1<0) answer.visible=false;
  else { //El área es total o parcialmente visible
    answer.visible=true;
    //Calculamos la zona de pantalla que ocupará
    answer.x1=x; if(x<0) answer.x1=0;
    answer.y1=y; if(y<0) answer.y1=0;
    answer.x2=x+xx-1; if(answer.x2>=dxx) answer.x2=dxx-1;
    answer.y2=y+yy-1; if(answer.y2>=dyy) answer.y2=dyy-1;
  }
  //Devolvemos respuesta
  return answer;
}

//Obtiene el código de color en base a sus componentes
uint16_t RoJoGraph::getColor(byte r,byte g,byte b) {
  //La profundidad de color de cada canal es:
    
  //  Canal : color : bits
  //  -----   -----   ----
  //    R     red       5
  //    G     green     6
  //    B     blue      5
  //La composición final de los 16 bits quedará así:
  // RRRRRGGG GGGBBBBB
    
  uint16_t c = r >> 3;
  c <<= 6;
  c |= g >> 2;
  c <<= 5;
  c |= b >> 3;
  return c;
}

//Descompone un color en sus componentes
void RoJoGraph::getComponents(uint16_t color,byte *r,byte *g,byte *b) {
  //El color lo tenemos en formato
  // RRRRRGGG GGGBBBBB
  //Cuando se pasa un color en formato rgb de 24 bits (3 bytes) a 16 bits (2 bytes)
  //siempre se pierde información.
  //Si reconvertimos un color de 16 bits a 24 bits, posiblemente no obtengamos los
  //mismos valores originales que se usaron en formato 24 bits.
  *r = (color >> 8) & 0b11111000;
  *g = (color >> 3) & 0b11111100;
  *b = (color << 3) & 0b11111000;
}

//Dibuja un pixel (dummy)
//Devuelve true si el pixel es visible
bool RoJoGraph::drawPixel(int16_t x,int16_t y,uint16_t color) {
  //Necesario porque es la base para todas las funciones gráficas de RoJoGraphs
  
  //Estructura:
  //  Comprobación si hay área visible
  //  Si no es visible terminamos con error
  //  Comandos para dibujo de un pixel en el display
  //  Todo Ok
  return false;
}

//Dibuja una línea utilizando el algoritmo de Bresenham
//Devuelve true si lo consigue
void RoJoGraph::line(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t color) {
  int16_t tmp; //Para swap
  const bool steep=abs(y2-y1)>abs(x2-x1);
  
  if(steep) {
    tmp=x1;x1=y1;y1=tmp;
    tmp=x2;x2=y2;y2=tmp;
  }
 
  if(x1>x2) {
    tmp=x1;x1=x2;x2=tmp;
    tmp=y1;y1=y2;y2=tmp;
  }
 
  int16_t dx=x2-x1;
  int16_t dy=abs(y2-y1);
 
  int16_t err=dx/2;
  const int16_t ystep=y1<y2?1:-1;
 
  for(;x1<=x2;x1++) {
    if(steep) drawPixel(y1,x1,color);
    else drawPixel(x1,y1,color);
 
    err-=dy;
    if(err<0) {
      y1+=ystep;
      err+=dx;
    }
  }
}

//Imprime el texto indicado basado en una fuente en el display
//Se indica el color del texto
//Respuesta: true si lo consigue
bool RoJoGraph::printOver(String filenameFon,String text,uint16_t textColor,int16_t x,int16_t y) {
  #ifdef ROJO_PIN_CS_SD //Si se utiliza SD...
    SD.begin(ROJO_PIN_CS_SD);
    File f=SD.open(filenameFon,FILE_READ); //Abrimos el archivo en la SD
  #else //Si utilizamos SPIFFS...
    //...ya se inicializó en el constructor
    File f=SPIFFS.open(filenameFon,"r"); //Abrimos el archivo en SPIFFS
  #endif
  //Si hubo algún problema...hemos terminado
  if(!f) return false;
  //Definimos variables de trabajo
  byte charMin; //Primer carácter ASCII incluido en librería
  byte charMax; //Último carácter ASCII incluido en librería
  byte charCurrent; //Carácter procesado
  byte charWidth; //Anchura de carácter procesado
  byte charPages=0; //Altura en páginas
  byte pageByte; //Byte gráfico actualmente procesado
  uint16_t charOffset; //Offset de datos gráficos del carácter en el archivo de fuentes
  //Leemos ASCII del primer carácter de la fuente
  f.readBytes((char *)&charMin,1);
  //Leemos ASCII del último carácter de la fuente
  f.readBytes((char *)&charMax,1);
  //Leemos el número de páginas
  f.readBytes((char *)&charPages,1);

  //Variable para guardar la posición x en la que estamos
  //escribiendo en el sprite
  uint16_t xx=0;
  //Recorremos todos los caracteres del string
  for(byte i=0;i<text.length();i++) {
    //Extraemos el carácter procesado
    charCurrent=text[i];
    //Si el carácter está dentro del rango de la fuente...
    //(si no pertenece a la fuente, no lo tenemos en cuenta)
    if(charCurrent>=charMin && charCurrent<=charMax) {
      //Si no es el primer carácter...dejaremos una separación
      if(xx>0) xx++;
      //Obtenemos la anchura del carácter
      f.seek(3+3*(charCurrent-charMin));
      f.read((byte *)&charWidth,1);
      //Obtenemos el índice de inicio de los datos gráficos del carácter
      f.read((byte *)&charOffset,2);
      //Posicionamos el offset para leer los datos gráficos
      f.seek(charOffset);
      //Recorremos todas las páginas de altura
      for(byte p=0;p<charPages;p++) {
        //Recorremos las columnas que forman el carácter
        for(byte c=0;c<charWidth;c++) {
          //Leemos el byte de la página
          f.read((byte *)&pageByte,1);
          //Recorremos todos sus bits
          for(byte b=0;b<8;b++) {
            //Si el pixel está activo...lo activamos
            if((1<<b) & pageByte) drawPixel(x+xx+c,y+p*8+b,textColor);
          }
        }
      }
      //El siguiente caracter lo comenzaremos a escribir a una distancia
      //igual a la anchura del carácter escrito
      xx+=charWidth;
    }
  }
  //Hemos terminado de utilizar el archivo de fuentes
  f.close();
  //Todo correcto
  return true;
}

//Información sobre el tamaño de un texto
//Devuelve true si la respuesta es válida
bool RoJoGraph::infoPrint(String filenameFon,String text,uint16_t *width,uint16_t *heigth) {
  #ifdef ROJO_PIN_CS_SD //Si se utiliza SD...
    SD.begin(ROJO_PIN_CS_SD);
    File f=SD.open(filenameFon,FILE_READ); //Abrimos el archivo en la SD
  #else //Si utilizamos SPIFFS...
    //...ya se inicializó en el constructor
    File f=SPIFFS.open(filenameFon,"r"); //Abrimos el archivo en SPIFFS
  #endif
  //Si hubo algún problema...hemos terminado
  if(!f) return false;
  //Definimos variables de trabajo
  byte charMin; //Primer carácter ASCII incluido en librería
  byte charMax; //Último carácter ASCII incluido en librería
  byte charCurrent; //Carácter procesado
  byte charWidth; //Anchura de carácter procesado
  byte charPages=0; //Altura en páginas
  //Leemos ASCII del primer carácter de la fuente
  f.readBytes((char *)&charMin,1);
  //Leemos ASCII del último carácter de la fuente
  f.readBytes((char *)&charMax,1);
  //Leemos el número de páginas
  f.readBytes((char *)&charPages,1);

  //Calculamos la anchura del sprite = suma de la anchura de todos los
  //caracteres y espacios separadores
  //Inicialmente la anchura del sprite es 0
  *width=0;
  //Recorremos todos los caracteres del texto
  for(uint16_t i=0;i<text.length();i++)
  {
    //Extraemos el carácter procesado
    charCurrent=text[i];
    //Si el carácter pertenece a los caracteres de la fuente...
    //(si no pertenece a la fuente, no lo tenemos en cuenta)
    if(charCurrent>=charMin && charCurrent<=charMax)
    {
      //...este carácter será representado
      //Si este no es el primer carácter, añadiremos una
      //columna de separación.
      if(*width>0) (*width)++;
      //Obtenemos la anchura del carácter
      f.seek(3+3*(charCurrent-charMin));
      f.read((byte *)&charWidth,1);
      //Añadimos la anchura del carácter
      *width+=charWidth;
    }
  }
  //Tenemos calculada la anchura del sprite en *width

  //Hemos terminado de utilizar el archivo de fuentes
  f.close();
  //La altura en pixels serán las páginas*8
  *heigth=(uint16_t)charPages*8;
  //Todo Ok
  return true;
}

//Dibuja un rectángulo relleno
//Devuelve true si tiene parte visible
bool RoJoGraph::block(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t color) {
  //Función genérica. Dibuja el bloque pixel a pixel

  //Intercambiamos coordenadas en orden incorrecto
  if(x1>x2) {int16_t tmp;tmp=x1;x1=x2;x2=tmp;}
  if(y1>y2) {int16_t tmp;tmp=y1;y1=y2;y2=tmp;}
  //Comprobamos si hay área visible
  displayRange r=visibleRange(x1,y1,x2-x1+1,y2-y1+1);
  //Si no es visible...terminamos
  if(!r.visible) return false;
  //El bloque tiene parte visible

  //Para evitar cambio de tipos en el bucle
  uint16_t rx2=r.x2,ry2=r.y2;
  //Recorremos todas las filas y columnas y dibujamos el pixel
  for(uint16_t y=r.y1;y<ry2;y++)
    for(uint16_t x=r.x1;x<rx2;x++)
      drawPixel(x,y,color);
  //Todo Ok
  return true;
}

//Borra el área de dibujo
void RoJoGraph::clear(uint16_t color) {
  block(0,0,xMax()-1,yMax()-1,color);
}

//Dibuja un rectángulo sin relleno
//Devuelve si tiene parte visible
bool RoJoGraph::rect(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t borderColor) {
  //Dibujamos el borde superior
  bool visible=block(x1,y1,x2,y1,borderColor);
  //Si tiene borde inferior...
  if(y1!=y2) {
    //...la dibujamos
    visible|=block(x1,y2,x2,y2,borderColor);
    //Si tiene bordes laterales...
    if(y2-y1>1) {
      //...dibujamos el borde izquierdo
      visible|=block(x1,y1+1,x1,y2-1,borderColor);
      //Si tiene borde derecho...lo dibujamos
      if(x1!=x2) visible|=block(x2,y1+1,x2,y2-1,borderColor);
    }
  }
  //Respondemos si tiene parte visible
  return visible;
}

//Dibuja un triángulo sin relleno
void RoJoGraph::triangle(int16_t x1,int16_t y1,int16_t x2,int16_t y2,int16_t x3,int16_t y3,uint16_t borderColor) {
  line(x1,y1,x2,y2,borderColor);
  line(x1,y1,x3,y3,borderColor);
  line(x2,y2,x3,y3,borderColor);
}

//Dibuja un triángulo relleno
void RoJoGraph::triangleFill(int16_t x0,int16_t y0,int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t fillColor) {
  int16_t tmp; //Para swap

  //Ordenamos en vertical: y0 <= y1 <= y2
  if(y0>y1) {
    tmp=x0;x0=x1;x1=tmp;
    tmp=y0;y0=y1;y1=tmp;
  }
  if(y1>y2) {
    tmp=x1;x1=x2;x2=tmp;
    tmp=y1;y1=y2;y2=tmp;
  }
  if(y0>y1) {
    tmp=x0;x0=x1;x1=tmp;
    tmp=y0;y0=y1;y1=tmp;
  }

  //Inicio y final de segmento horizontal
  int16_t a,b;

  //Si la primera y última coordenada vertical coinciden...
  if(y0==y2) {
    //Sólo tendremos que dibujar una línea horizontal
    //Calcularemos sus coordenadas horizontales con el mínimo (a) y el máximo (b)
    a=b=x0;
    if(x1<a) a=x1; else if(x1>b) b=x1;
    if(x2<a) a=x2; else if(x2>b) b=x2;
    //Dibujamos una línea horizontal
    block(a,y1,b,y1,fillColor);
    //Hemos terminado
    return;
  }
  //El triángulo no forma una línea horizontal

  //Definición de variables
  int16_t dx01=x1-x0;
  int16_t dy01=y1-y0;
  int16_t dx02=x2-x0;
  int16_t dy02=y2-y0;
  int16_t dx12=x2-x1;
  int16_t dy12=y2-y1;
  int16_t sa=0;
  int16_t sb=0;
  int16_t last; //Última coordenada vertical a dibujar/rellenar
  int16_t y; //Coordenada vertical procesada

  //Calculamos la coordenada vertical hasta la que tenemos que dibujar/rellenar
  //Si la parte inferior del triángulo es horizontal...esa será la última
  if(y1==y2) last=y1;
  //Y si no...dibujaremos hasta una antes de llegar a la coordenada vertical intermedia
  else last=y1-1;
  
  //Dibujamos la parte superior del triángulo. Desde y0 a y1
  for (y=y0;y<=last;y++) {
    //Calculamos los extremos
    a=x0+sa/dy01;
    b=x0+sb/dy02;
    
    sa+=dx01;
    sb+=dx02;

    block(a,y,b,y,fillColor);
  }

  //Dibujamos la parte inferior del triángulo. Desde y1 a y2
  //Si y1==y2 no se dibuja nada
  sa=dx12*(y-y1);
  sb=dx02*(y-y0);
  for(;y<=y2;y++) {
    //Calculamos los extremos
    a=x1+sa/dy12;
    b=x0+sb/dy02;
    
    sa+=dx12;
    sb+=dx02;

    block(a,y,b,y,fillColor);
  }
}

//Dibuja una circunferencia
void RoJoGraph::circle(int16_t x0,int16_t y0,int16_t r,uint16_t color) {
  int16_t f=1-r,ddF_x=1,ddF_y=-2*r,x=0,y=r;
  drawPixel(x0,y0+r,color);
  drawPixel(x0,y0-r,color);
  drawPixel(x0+r,y0,color);
  drawPixel(x0-r,y0,color);
  while (x<y) {
    if(f>=0) {
      y--;
      ddF_y+=2;
      f+=ddF_y;
    }
    x++;
    ddF_x+=2;
    f+=ddF_x;
    drawPixel(x0+x,y0+y,color);
    drawPixel(x0-x,y0+y,color);
    drawPixel(x0+x,y0-y,color);
    drawPixel(x0-x,y0-y,color);
    drawPixel(x0+y,y0+x,color);
    drawPixel(x0-y,y0+x,color);
    drawPixel(x0+y,y0-x,color);
    drawPixel(x0-y,y0-x,color);
  }
}

//Dibuja un círculo
void RoJoGraph::disk(int16_t x0,int16_t y0,int16_t r,uint16_t color) {
  int16_t f=1-r,ddF_x=1,ddF_y=-2*r,x=0,y=r;
  block(x0,y0-r,x0,y0+r,color); //Línea vertical
  while(x<y) {
    if(f>=0) {
      y--;
      ddF_y+=2;
      f+=ddF_y;
    }
    x++;
    ddF_x+=2;
    f+=ddF_x;
    block(x0+x,y0-y,x0+x,y0+y,color);
    block(x0-x,y0-y,x0-x,y0+y,color);
    block(x0+y,y0-x,x0+y,y0+x,color);
    block(x0-y,y0-x,x0-y,y0+x,color);
  }
}

//Dibuja una elipse con o sin relleno
void RoJoGraph::_ellipse(int16_t x0,int16_t y0,uint16_t rx,uint16_t ry,uint16_t color,bool fill) {
  int32_t rx0=rx,ry0=ry; //Homogeneizamos tipos de variables
  //Si algún radio es inferior a 2...no dibujamos nada
  if(rx<2 || ry<2) return;
  int32_t x,y;
  int32_t rx2=rx0*rx0; //Cuadrado del radio horizontal
  int32_t ry2=ry0*ry0; //Cuadrado del radio vertical
  int32_t fx2=4*rx2;
  int32_t fy2=4*ry2;
  int32_t s;

  for(x=0,y=ry0,s=2*ry2+rx2*(1-2*ry0);ry2*x<=rx2*y;x++) {
    if(fill) {
      block(x0-x,y0+y,x0+x,y0+y,color);
      block(x0-x,y0-y,x0+x,y0-y,color);
    }
    else {
      drawPixel(x0+x,y0+y,color);
      drawPixel(x0-x,y0+y,color);
      drawPixel(x0-x,y0-y,color);
      drawPixel(x0+x,y0-y,color);
    }
    if(s>=0) {
      s+=fx2*(1-y);
      y--;
    }
    s+=ry2*(4*x+6);
  }

  for(x=rx0,y=0,s=2*rx2+ry2*(1-2*rx0);rx2*y<=ry2*x;y++) {
    if(fill) {
      block(x0-x,y0+y,x0+x,y0+y,color);
      block(x0-x,y0-y,x0+x,y0-y,color);
    }
    else {
      drawPixel(x0+x,y0+y,color);
      drawPixel(x0-x,y0+y,color);
      drawPixel(x0-x,y0-y,color);
      drawPixel(x0+x,y0-y,color);
    }
    if(s>=0) {
      s+=fy2*(1-x);
      x--;
    }
    s+=rx2*(4*y+6);
  }
}
void RoJoGraph::ellipse(int16_t x,int16_t y,uint16_t rx,uint16_t ry,uint16_t color) {
  _ellipse(x,y,rx,ry,color,false);
}
void RoJoGraph::ellipseFill(int16_t x,int16_t y,uint16_t rx,uint16_t ry,uint16_t color) {
  _ellipse(x,y,rx,ry,color,true);
}

//Información de archivo BMP
byte RoJoGraph::infoBMP(String filename,uint16_t *width,uint16_t *height,byte *colorDepth) {
  //Tabla de errores:
  //  0 : No hay errores. Todo correcto
  //  1 : No se puede abrir el archivo. Posiblemente no existe.
  //  2 : La firma del tipo de archivo no coincide
  //  3 : Anchura demasiado grande
  //  4 : Altura demasiado grande
  //  5 : El número de planos no es 1
  //  6 : La profundidad de color no está reconocida
  //  7 : Está comprimido

  //Formato BMP
  //CABECERA
  //offset # descripción
  //------ - -----------
  //   0   2 "BM" constante
  //   2   4 tamaño de archivo
  //   6   4 reservado
  //  10   4 offset de inicio de datos gráficos
  //  14   4 tamaño de cabecera
  //  18   4 anchura en pixels
  //  22   4 altura en pixels
  //  26   2 número de planos (habitualmente 1)
  //  28   2 bits por pixel (habitualmente 24 para imágenes a color)
  //  30   4 compresión (habitualmente 0=ninguna)
  //  34   4 tamaño de imágen
  //  38   4 resolución horizontal
  //  42   4 resolución vertical
  //  46   4 tamaño de la tabla de color
  //  50   4 contador de colores importantes

  //DATOS GRÁFICOS
  //Se guardan por filas
  //Las filas están invertidas. La primera que encontramos corresponde a la última de la imágen
  //El número de bytes de una fila debe ser múltiplo de 4. Si no lo es, se completarán con bytes vacíos
  //Por cada fila, las columnas van en orden: de izquierda a derecha
  //El primer dato gráfico contendrá información sobre el pixel de abajo a la izquierda

  //Declaración de variables
  byte errorCode=0; //Código de error a devolver

  #ifdef ROJO_PIN_CS_SD //Si se utiliza SD...
    SD.begin(ROJO_PIN_CS_SD);
    File f=SD.open(filename,FILE_READ); //Abrimos el archivo en la SD
  #else //Si utilizamos SPIFFS...
    //...ya se inicializó en el constructor
    File f=SPIFFS.open(filename,"r"); //Abrimos el archivo en SPIFFS
  #endif
  if(!f) return 1;
  //Si la firma de los dos primeros bytes no es "BM"...error
  if(f.read()!='B' || f.read()!='M') errorCode=2;
  else
  {
    //Comprobamos que la anchura está dentro de los límites
    f.seek(20);
    //Si la anchura es mayor de 16 bits...error
    f.read((byte *)width,2);
    if(*width) errorCode=3;
    else
    {
      //Comprobamos que la altura está dentro de los límites
      f.seek(24);
      //Si la altura es mayor de 16 bits...error
      f.read((byte *)width,2);
      if(*width) errorCode=4;
      else
      {
        //Si el número de planos no es 1...error
        f.read((byte *)width,2);
        if(*width!=1) errorCode=5;
        else
        {
          //Si no tiene una profundidad de color reconocida...error
          //Para sprites a color el bmp debe tener 24 bits por pixel
          //Para sprites monocromos el bmp debe tener 1 bit por pixel
          f.read((byte *)width,2);
          if(*width!=24 && *width!=1) errorCode=6;
          else
          {
            //Anotamos la profundidad de color
            *colorDepth=*width==24?16:1;
            //Si está comprimido...error
            f.read((byte *)width,2);
            f.read((byte *)height,2);
            if(*width!=0 || *height!=0) errorCode=7;
            else
            {
              //Leemos la anchura
              f.seek(18);
              f.read((byte *)width,2);
              //Leemos la altura
              f.seek(22);
              f.read((byte *)height,2);
            }
          }
        }
      }
    }
  }
  //hemos terminado de utilizar el archivo
  f.close();
  //Devolvemos código de error
  return errorCode;
}

//Dibuja un archivo bmp en unas coordenadas
//Sobreescribe la información existente
//Respuesta: código de error
byte RoJoGraph::drawBMP(String filename,int16_t x,int16_t y) {
  //Tabla de errores (los primeros son los de infoBMP):
  //  0 : No hay errores. Todo correcto
  //  1 : No se puede abrir el archivo. Posiblemente no existe.
  //  2 : La firma del tipo de archivo no coincide
  //  3 : Anchura demasiado grande
  //  4 : Altura demasiado grande
  //  5 : El número de planos no es 1
  //  6 : La profundidad de color no está reconocida
  //  7 : Está comprimido
  //  8 : La profundidad de color no coincide con la del display

  //Formato BMP
  //CABECERA
  //offset # descripción
  //------ - -----------
  //   0   2 "BM" constante
  //   2   4 tamaño de archivo
  //   6   4 reservado
  //  10   4 offset de inicio de datos gráficos
  //  14   4 tamaño de cabecera
  //  18   4 anchura en pixels
  //  22   4 altura en pixels
  //  26   2 número de planos (habitualmente 1)
  //  28   2 bits por pixel (habitualmente 24 para imágenes a color)
  //  30   4 compresión (habitualmente 0=ninguna)
  //  34   4 tamaño de imágen
  //  38   4 resolución horizontal
  //  42   4 resolución vertical
  //  46   4 tamaño de la tabla de color
  //  50   4 contador de colores importantes

  //DATOS GRÁFICOS
  //Se guardan por filas
  //Las filas están invertidas. La primera que encontramos corresponde a la última de la imágen
  //El número de bytes de una fila debe ser múltiplo de 4. Si no lo es, se completarán con bytes vacíos
  //Por cada fila, las columnas van en orden: de izquierda a derecha
  //El primer dato gráfico contendrá información sobre el pixel de abajo a la izquierda
  //En monocromo los volores están invertidos

  //Declaración de variables
  byte errorCode=0; //Código de error a devolver
  uint16_t width,height; //Anchura y altura
  byte colorDepth; //Profundidad de color
  
  //Leemos los valores del archivo bmp
  errorCode=infoBMP(filename,&width,&height,&colorDepth);
  //Si tenemos algún error...lo devolvemos
  if(errorCode) return errorCode;
  //No tenemos errores

  //Si la profundidad de color no coincide con la del display...terminamos con error
  if(_colorDepth!=colorDepth) return 8;

  //Calculamos el área visible
  displayRange r=visibleRange(x,y,width,height);
  //Si no hay área visible...hemos terminado ok
  if(!r.visible) return 0;

  //Calculamos el número de bytes que contiene una línea
  uint32_t rowLength;
  if(_colorDepth==16) rowLength=width*3; //color
  else { //monocromo
    rowLength=width/8;
    if(width%8) rowLength++;
  }
  //El número de bytes de una línea debe ser múltiplo de 4
  if(rowLength%4) rowLength+=4-(rowLength%4);

  //Leemos los datos gráficos
  #ifdef ROJO_PIN_CS_SD //Si se utiliza SD...
    SD.begin(ROJO_PIN_CS_SD);
    File f=SD.open(filename,FILE_READ); //Abrimos el archivo en la SD
  #else //Si utilizamos SPIFFS...
    //...ya se inicializó en el constructor
    File f=SPIFFS.open(filename,"r"); //Abrimos el archivo en SPIFFS
  #endif
  //Suponemos que no hay errores de apertura en el archivo, porque lo hemos
  //probado hace un momento con infoBMP

  //Leemos el offset de inicio de datos gráficos
  uint32_t offsetBase=0; //Offset de datos gráficos. Base (primer pixel)
  f.seek(10);
  f.read((byte *)&offsetBase,4);
  //El offset con el que trabajaremos será el del primer pixel de la primera línea
  offsetBase+=(uint32_t)rowLength*((uint32_t)height-1);

  #ifdef ESP8266
    ESP.wdtDisable(); //Desactivamos WatchDog mientras dibujamos
  #endif
  //Diferenciaremos la rutina de decodificación gráfica dependiendo de la profundidad de color
  if(_colorDepth==16) { //Color
    byte color[3]; //Componentes de color de un pixel
    uint32_t ry2=r.y2,y32=y;
    offsetBase+=(r.x1-x)*3; //Sumamos el offset de x
    //Recorremos las filas visibles del display
    for(uint32_t yy=(uint32_t)r.y1;yy<=ry2;yy++) {
      //Calculamos y posicionamos el offset en el archivo
      f.seek(offsetBase-rowLength*(yy-y32));
      //Recorremos las columnas visibles del display
      for(uint16_t xx=r.x1;xx<=r.x2;xx++) {
        //Leemos los componentes de color
        f.read(color,3);
        //Lo codificamos en un color 565
        //Escribimos el color en el pixel procesado
        //El orden de lectura también es little endian -> BGR
        drawPixel(xx,yy,getColor(color[2],color[1],color[0]));
      }
    }
  }
  else { //Monocromo
    byte readBits=0; //bits leidos
    byte pendingBits=0; //Bits pendientes de leer de readBits
    int32_t ry2=r.y2;
    byte x8r=(r.x1-x)%8; //Resto de 8 de la columna inicial
    offsetBase+=(r.x1-x)/8;
    //Recorremos las filas visibles del display
    for(int32_t yy=(int32_t)r.y1;yy<=ry2;yy++) {
      //Calculamos y posicionamos el offset en el archivo
      f.seek(offsetBase-rowLength*(yy-y));
      pendingBits=0;
      if(x8r) {
        readBits=f.read();
        readBits<<=x8r;
        pendingBits=8-x8r;
      }
      //Recorremos todas las columnas visibles del display
      for(uint16_t xx=r.x1;xx<=r.x2;xx++) {
        //Si no tenemos bits pendientes de leer...
        if(!pendingBits) {
          readBits=f.read(); //Leemos un nuevo byte
          pendingBits=8; //Ya tenemos 8 nuevos bits pendientes por leer
        }
        //Dibujamos el pixel con el color que corresponde
        drawPixel(xx,yy,readBits<128);
        //Ya tenemos un bit menos
        readBits<<=1;
        pendingBits--;
      }
    }
  }
  #ifdef ESP8266
    ESP.wdtEnable(1000); //Reactivamos las interrupciones de WatchDog
  #endif
  //hemos terminado de utilizar el archivo
  f.close();
  //Devolvemos código de error
  return errorCode;
}

//Información de archivo de sprite
byte RoJoGraph::infoSprite(String filename,uint16_t *width,uint16_t *height,byte *colorDepth) {
  //Tabla de errores:
  //  0 : No hay errores. Todo correcto
  //  1 : No se puede abrir el archivo. Posiblemente no existe
  //  2 : La profundidad de color no está reconocida

  //Formato sprite
  //CABECERA
  //offset # descripción
  //------ - -----------
  //   0   1 profundidad de color 1=monocromo, 16=color
  //   1   2 anchura en pixels
  //   3   2 altura en pixels
  //   5   ? datos gráficos

  //DATOS GRÁFICOS
  //Se guardan por filas (en color) o páginas (en monocromo)

  #ifdef ROJO_PIN_CS_SD //Si se utiliza SD...
    SD.begin(ROJO_PIN_CS_SD);
    File f=SD.open(filename,FILE_READ); //Abrimos el archivo en la SD
  #else //Si utilizamos SPIFFS...
    //...ya se inicializó en el constructor
    File f=SPIFFS.open(filename,"r"); //Abrimos el archivo en SPIFFS
  #endif
  if(!f) return 1;

  *colorDepth=f.read();
  if(*colorDepth!=1 && *colorDepth!=16) return 2;

  f.read((byte *)width,2);
  f.read((byte *)height,2);

  f.close();
  return 0; //Todo Ok
}

//Dibuja un archivo bmp en unas coordenadas
//Sobreescribe la información existente
//Respuesta: código de error
byte RoJoGraph::drawSprite(String filename,int16_t x,int16_t y) {
  //Tabla de errores (los primeros son los de infoSprite):
  //  0 : No hay errores. Todo correcto
  //  1 : No se puede abrir el archivo. Posiblemente no existe
  //  2 : La profundidad de color no está reconocida
  //  3 : La profundidad de color no coincide con la del display

  //Declaración de variables
  uint16_t width,height; //Anchura y altura
  byte colorDepth; //Profundidad de color
  //Leemos los valores del archivo bmp
  byte errorCode=infoSprite(filename,&width,&height,&colorDepth);
  //Si tenemos algún error...lo devolvemos
  if(errorCode) return errorCode;
  //No tenemos errores

  //Si la profundidad de color no coincide con la del display...terminamos con error
  if(_colorDepth!=colorDepth) return 3;

  //Calculamos el área visible
  displayRange r=visibleRange(x,y,width,height);
  //Si no hay área visible...hemos terminado ok
  if(!r.visible) return 0;

  //Leemos los datos gráficos
  #ifdef ROJO_PIN_CS_SD //Si se utiliza SD...
    SD.begin(ROJO_PIN_CS_SD);
    File f=SD.open(filename,FILE_READ); //Abrimos el archivo en la SD
  #else //Si utilizamos SPIFFS...
    //...ya se inicializó en el constructor
    File f=SPIFFS.open(filename,"r"); //Abrimos el archivo en SPIFFS
  #endif
  //Suponemos que no hay errores de apertura en el archivo, porque lo hemos
  //probado hace un momento con infoSprite

  //Offset de datos gráficos
  uint32_t offsetBase=5;

  #ifdef ESP8266
    ESP.wdtDisable(); //Desactivamos WatchDog mientras dibujamos
  #endif

  //Diferenciaremos la rutina de decodificación gráfica dependiendo de la profundidad de color
  if(_colorDepth==16) { //Color
    uint16_t color;
    uint32_t rowLength=width*2; //Número de bytes que contiene una línea
    uint32_t ry2=r.y2,rx2=r.x2,y32=y;
    //Incluimos el offset de x
    offsetBase+=2*(r.x1-x);
    //Recorremos las filas visibles del display
    for(uint32_t yy=(uint32_t)r.y1;yy<=ry2;yy++) {
      //Posicionamos offset en archivo
      f.seek(offsetBase+rowLength*(yy-y32));
      //Recorremos las columnas visibles del display
      for(uint32_t xx=r.x1;xx<=rx2;xx++) {
        //Leemos el color
        f.read((byte *)&color,2);
        //Dibujamos el pixel
        drawPixel(xx,yy,color);
      }
    }
  }
  else { //Monocromo
    uint32_t rowLength=width; //Número de bytes que contiene una línea
    uint32_t ry2=r.y2,rx2=r.x2;
    //Recorremos las filas visibles del display
    for(uint32_t yy=(uint32_t)r.y1;yy<=ry2;yy++) {
      //Recorremos las columnas visibles del display
      for(uint32_t xx=r.x1;xx<=rx2;xx++) {
        //Calculamos coordenadas del archivo
        uint32_t xxx=xx-x,yyy=yy-y;
        //Posicionamos offset en archivo
        f.seek(offsetBase+rowLength*(yyy/8)+xxx);
        //Leemos el dato
        byte valuePage=f.read();
        //Dibujamos el pixel
        drawPixel(xx,yy,(valuePage & (1<<(yyy%8)))>0);
      }
    }
  }
  #ifdef ESP8266
    ESP.wdtEnable(1000); //Reactivamos las interrupciones de WatchDog
  #endif
  //hemos terminado de utilizar el archivo
  f.close();
  //Todo Ok
  return 0;
}

#endif
