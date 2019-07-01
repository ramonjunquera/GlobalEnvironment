/*
  Nombre de la librería: RoJoGraph.h
  Versión: 20190626
  Autor: Ramón Junquera
  Descripción:
    Funciones gráficas avanzadas
*/

#ifndef RoJoGraph_cpp
#define RoJoGraph_cpp

#include <RoJoGraph.h>

//Inicialización
void RoJoGraph::begin()
{
  //Creamos este método porque en ESP32 no se puede inicializar SPIFFS
  //dentro de un constructor. Da un error en tiempo de ejecución
  //Este método debe se llamada  siempre por las clases hijas

  //Si no utilizamos SD (si utilizamos SPIFFS)...
  #ifndef ROJO_PIN_CS_SD
    //...lo inicializamos
    SPIFFS.begin();
  #endif
}

//Anchura de display (dummy)
uint16_t RoJoGraph::xMax()
{
  return 0;
}

//Altura de display (dummy)
uint16_t RoJoGraph::yMax()
{
  return 0;
}

//Calcula el rango visible de un objeto
//Parámetros:
//  x,y : coordenadas del objeto
//  xx,yy : anchura y altura del objeto
//Respuesta: estructura con las coordenadasde display ocupadas por objeto
displayRange RoJoGraph::visibleRange(int16_t x,int16_t y,uint16_t xx,uint16_t yy)
{
  //Anotamos la anchura y altura del display
  uint16_t dxx=xMax();
  uint16_t dyy=yMax();
  //Creamos estructura de respuesta
  displayRange answer;
  //Si el objeto está fuera de pantalla...no será visible
  if(x>=(int16_t)dxx || y>=(int16_t)dyy || x+xx-1<0 || y+yy-1<0) answer.visible=false;
  else //El área es total o parcialmente visible
  {
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
uint16_t RoJoGraph::getColor(byte r,byte g,byte b)
{
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
void RoJoGraph::getComponents(uint16_t color,byte *r,byte *g,byte *b)
{
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
//Devuelve true si lo consigue si el pixel es visible
bool RoJoGraph::drawPixel(int16_t x,int16_t y,uint16_t color)
{
  return false;
}

//Dibuja una línea utilizando el algoritmo de Bresenham
void RoJoGraph::line(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t color)
{
  int16_t tmp; //Para swap
  const bool steep=abs(y2-y1)>abs(x2-x1);
  
  if(steep)
  {
    tmp=x1;x1=y1;y1=tmp;
    tmp=x2;x2=y2;y2=tmp;
  }
 
  if(x1>x2)
  {
    tmp=x1;x1=x2;x2=tmp;
    tmp=y1;y1=y2;y2=tmp;
  }
 
  int16_t dx=x2-x1;
  int16_t dy=abs(y2-y1);
 
  int16_t err=dx/2;
  const int16_t ystep=y1<y2?1:-1;
 
  for(;x1<=x2;x1++)
  {
    if(steep) drawPixel(y1,x1,color);
    else drawPixel(x1,y1,color);
 
    err-=dy;
    if(err<0)
    {
      y1+=ystep;
      err+=dx;
    }
  }
}

//Imprime el texto indicado basado en una fuente
//Se indica el color del texto
//Respuesta: true si lo consigue
bool RoJoGraph::print(int16_t x,int16_t y,String fileNameFon,String text,uint16_t textColor)
{
  #ifdef ROJO_PIN_CS_SD //Si se utiliza SD...
    SD.begin(ROJO_PIN_CS_SD);
    File f=SD.open(fileNameFon,FILE_READ); //Abrimos el archivo en la SD
  #else //Si utilizamos SPIFFS...
    //...ya se inicializó en el constructor
    File f=SPIFFS.open(fileNameFon,"r"); //Abrimos el archivo en SPIFFS
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
  for(byte i=0;i<text.length();i++)
  {
    //Extraemos el carácter procesado
    charCurrent=text[i];
    //Si el carácter está dentro del rango de la fuente...
    //(si no pertenece a la fuente, no lo tenemos en cuenta)
    if(charCurrent>=charMin && charCurrent<=charMax)
    {
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
      for(byte p=0;p<charPages;p++)
      {
        //Recorremos las columnas que forman el carácter
        for(byte c=0;c<charWidth;c++)
        {
          //Leemos el byte de la página
          f.read((byte *)&pageByte,1);
          //Recorremos todos sus bits
          for(byte b=0;b<8;b++)
          {
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

//Dibuja un rectángulo relleno (dummy)
bool RoJoGraph::block(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t color)
{
  return false;
}

//Borra el área de dibujo
void RoJoGraph::clear(uint16_t color)
{
  block(0,0,xMax()-1,yMax()-1,color);
}

//Dibuja un rectángulo sin relleno
//Devuelve si tiene parte visible
bool RoJoGraph::rect(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t borderColor)
{
  //Dibujamos el borde superior
  bool visible=block(x1,y1,x2,y1,borderColor);
  //Si tiene borde inferior...
  if(y1!=y2)
  {
    //...la dibujamos
    visible|=block(x1,y2,x2,y2,borderColor);
    //Si tiene bordes laterales...
    if(y2-y1>1)
    {
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
void RoJoGraph::triangle(int16_t x1,int16_t y1,int16_t x2,int16_t y2,int16_t x3,int16_t y3,uint16_t borderColor)
{
  line(x1,y1,x2,y2,borderColor);
  line(x1,y1,x3,y3,borderColor);
  line(x2,y2,x3,y3,borderColor);
}

//Dibuja un triángulo relleno
void RoJoGraph::triangleFill(int16_t x0,int16_t y0,int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t fillColor)
{
  int16_t tmp; //Para swap

  //Ordenamos en vertical: y0 <= y1 <= y2
  if(y0>y1)
  {
    tmp=x0;x0=x1;x1=tmp;
    tmp=y0;y0=y1;y1=tmp;
  }
  if(y1>y2)
  {
    tmp=x1;x1=x2;x2=tmp;
    tmp=y1;y1=y2;y2=tmp;
  }
  if(y0>y1)
  {
    tmp=x0;x0=x1;x1=tmp;
    tmp=y0;y0=y1;y1=tmp;
  }

  //Inicio y final de segmento horizontal
  int16_t a,b;

  //Si la primera y última coordenada vertical coinciden...
  if(y0==y2)
  {
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
  for (y=y0;y<=last;y++)
  {
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
  for(;y<=y2;y++)
  {
    //Calculamos los extremos
    a=x1+sa/dy12;
    b=x0+sb/dy02;
    
    sa+=dx12;
    sb+=dx02;

    block(a,y,b,y,fillColor);
  }
}

//Dibuja una circunferencia
void RoJoGraph::circle(int16_t x0,int16_t y0,int16_t r,uint16_t color)
{
  int16_t f=1-r,ddF_x=1,ddF_y=-2*r,x=0,y=r;
  drawPixel(x0,y0+r,color);
  drawPixel(x0,y0-r,color);
  drawPixel(x0+r,y0,color);
  drawPixel(x0-r,y0,color);
  while (x<y)
  {
    if(f>=0)
    {
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
void RoJoGraph::disk(int16_t x0,int16_t y0,int16_t r,uint16_t color)
{
  int16_t f=1-r,ddF_x=1,ddF_y=-2*r,x=0,y=r;
  block(x0,y0-r,x0,y0+r,color); //Línea vertical
  while(x<y)
  {
    if(f>=0)
    {
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

//Dibuja una elipse con a sin relleno
void RoJoGraph::_ellipse(int16_t x0,int16_t y0,uint16_t rx,uint16_t ry,uint16_t color,bool fill)
{
  int32_t rx0=rx,ry0=ry; //Homogeneizamos tipos de variables
  //Si algún radio es inferior a 2...no dibujamos nada
  if(rx<2 || ry<2) return;
  int32_t x,y;
  int32_t rx2=rx0*rx0; //Cuadrado del radio horizontal
  int32_t ry2=ry0*ry0; //Cuadrado del radio vertical
  int32_t fx2=4*rx2;
  int32_t fy2=4*ry2;
  int32_t s;

  for(x=0,y=ry0,s=2*ry2+rx2*(1-2*ry0);ry2*x<=rx2*y;x++)
  {
    if(fill)
    {
      block(x0-x,y0+y,x0+x,y0+y,color);
      block(x0-x,y0-y,x0+x,y0-y,color);
    }
    else
    {
      drawPixel(x0+x,y0+y,color);
      drawPixel(x0-x,y0+y,color);
      drawPixel(x0-x,y0-y,color);
      drawPixel(x0+x,y0-y,color);
    }
    if(s>=0)
    {
      s+=fx2*(1-y);
      y--;
    }
    s+=ry2*(4*x+6);
  }

  for(x=rx0,y=0,s=2*rx2+ry2*(1-2*rx0);rx2*y<=ry2*x;y++)
  {
    if(fill)
    {
      block(x0-x,y0+y,x0+x,y0+y,color);
      block(x0-x,y0-y,x0+x,y0-y,color);
    }
    else
    {
      drawPixel(x0+x,y0+y,color);
      drawPixel(x0-x,y0+y,color);
      drawPixel(x0-x,y0-y,color);
      drawPixel(x0+x,y0-y,color);
    }
    if(s>=0)
    {
      s+=fy2*(1-x);
      x--;
    }
    s+=rx2*(4*y+6);
  }
}
void RoJoGraph::ellipse(int16_t x,int16_t y,uint16_t rx,uint16_t ry,uint16_t color)
{
  _ellipse(x,y,rx,ry,color,false);
}
void RoJoGraph::ellipseFill(int16_t x,int16_t y,uint16_t rx,uint16_t ry,uint16_t color)
{
  _ellipse(x,y,rx,ry,color,true);
}

#endif
