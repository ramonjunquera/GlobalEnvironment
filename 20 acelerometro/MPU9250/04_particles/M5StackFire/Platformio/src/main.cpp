/*
  Autor: Ramón Junquera
  Fecha: 20200218
  Tema: Acelerómetro 
  Objetivo: Mostrar lecturas gráficamente. Gestión de partículas
  Material: M5Stack Fire

  Descripción:
    Las partículas vienen definidas en un archivo gráfico.
    Cualquier pixel que no sea negro puro (color 0) se considerará una partícula.
    Se conservará el color original de cada una de ellas.
    Se reserva memoria dinámicamente, en función del número de partículas del
    archivo gráfico.
    Cada una tendrá su posición y velocidad.
    Las lecturas del acelerómetro les afectará.
    Las partículas pueden chocar emtre ellas.
    Existe el efecto rebote.
    Se varía ligeramente la aceleración de cada partícula para simular un plano irregular
    El primer botón activa la simulación y la resetea.
    Para mejorar el rendimiento se utilizan enteros con signo de 32 bits en vez de float.
    Consideramos que los 16 bits más altos indican las coordenadas de trabajo (antigua parte
    entera de valor float) y los 16 más bajos serán la posición segundaria dentro del mismo
    pixel (antigua parte decimal).
  
  Nota:
    - Es importante que inicialmente se mantenga el dispositivo horizontal para que toma las
      referencias de inclinación.
*/

#include <Arduino.h>
#include <RoJoILI9341.h> //Librería de gestión del display ILI9341
#include <RoJoSprite.h> //Librería de gestión de sprites monocromos
#include <RoJoMPU9250.h> //Librería de gestión de sensores de MPU9250
#include <RoJoSwitch.h> //Librería de gestión de interruptores

//Definición de constantes globales
const uint16_t grain=2; //Tamaño de pixel
const byte elastic=40; //Coeficiente de elasticidad. Porcentaje de la energía resultante. 0=no rebota. 100=siempre rebota

//Definición de objetos globales
RoJoILI9341 display;
RoJoSprite m(1); //Sprite monocromo para máscara (colisiones)
RoJoMPU9250 GAM; //Sensores
RoJoSwitch button1(39); //Primer botón

//Definición de variables globales
uint16_t particles; //Número de partículas
int32_t *coorX,*coorY,*speedX,*speedY; //Arrays de coordenadas y velocidad de cada partícula
RoJoColor* color; //Array de color de cada partícula
int16_t dataA[3]; //Datos del acelerómetro
int16_t refXY[2]; //Valores de referencia del Acelerómetro
uint16_t xRes,yRes; //Resolución de trabajo
int32_t accelX,accelY; //Aceleración del ciclo

//Libera la memoria de todos los arrays
void clearParticles() {
  free(coorX); coorX=0;
  free(coorY); coorY=0;
  free(speedX); speedX=0;
  free(speedY); speedY=0;
  free(color); color=0;
}

//Dibuja un pixel en pantalla y máscara teniendo en cuenta el tamaño de grano
void drawPixel(uint16_t pixelIndex,RoJoColor color) {
  uint16_t x=coorX[pixelIndex]>>16;
  uint16_t y=coorY[pixelIndex]>>16;
  m.drawPixel(x,y,{0,0,color.get24()>0}); //Anotamos pixel en sprite de máscara
  uint16_t xx=x*grain,yy=y*grain; //Cálculo de coordenadas de display
  display.block(xx,yy,xx+grain-1,yy+grain-1,color); //Dibujamos bloque en display
}

//Inicialización
void beginParticles() {
  xRes=display.xMax()/grain; //Cálculo de resolución X
  yRes=display.yMax()/grain; //Cálculo de resolución Y
  m.setSize(xRes,yRes); //Dimensionamos sprite de máscara con la resolución de trabajo
  clearParticles(); //Liberamos memoria = borramos arrays
  display.clear(); //Borra pantalla
  m.clear(); //Borra máscara
  RoJoSprite sprite; 
  sprite.loadBMP("/HOLA.bmp"); //Cargamos imágen a sprite desde archivo

  //Calculamos el offset para que el sprite quede centrado
  uint16_t xOffset=(xRes-sprite.xMax())/2;
  uint16_t yOffset=(yRes-sprite.yMax())/2;
  //Contamos el número de particulas
  particles=0;
  for(uint16_t y=0;y<sprite.yMax();y++)
    for(uint16_t x=0;x<sprite.xMax();x++)
      if(sprite.getPixel(x,y).get24()>0) particles++;
  //Reservamos memoria para los array de propiedades de partículas
  coorX=(int32_t*)malloc(particles*4);
  coorY=(int32_t*)malloc(particles*4);
  speedX=(int32_t*)malloc(particles*4);
  speedY=(int32_t*)malloc(particles*4);
  color=(RoJoColor*)malloc(particles*3);
  //Si nos ha faltado memoria para algún array...no trataremos ninguna partícula
  if(coorX==0 || coorY==0 || speedX==0 || speedY==0 || color==0) particles=0;
  else { //Hemos podido dimensionar todos los arrays
    //Revisamos todos los pixels del sprite y anotamos sus propiedades
    uint16_t pixelIndex=0;
    for(uint16_t y=0;y<sprite.yMax();y++)
      for(uint16_t x=0;x<sprite.xMax();x++) {
        RoJoColor c=sprite.getPixel(x,y); //Obtenemos el color del pixel procesado
        if(c.get24()>0) { //Si tiene color...
          coorX[pixelIndex]=((int32_t)(xOffset+x))<<16;; //Coordenada x
          coorY[pixelIndex]=((int32_t)(yOffset+y))<<16; //Coordenada y
          speedX[pixelIndex]=0; //Velocidad X inicial nula
          speedY[pixelIndex]=0; //Velocidad Y inicial nula
          color[pixelIndex]=c; //Anotamos color de partícula
          drawPixel(pixelIndex,c); //Dibujamos pixel
          pixelIndex++;
        }
      }
  }
  sprite.end();
  //Recogemos los valores actuales del acelerómetro que utilizaremos como referencia
  GAM.readA(refXY);
  //Esperamos a que se puse el botón
  while(!button1.pressed());
}

void setup() {
  //Serial.begin(115200); delay(4000); //DEBUG
  //Inicialización de display para M5Stack Fire
  display.begin(33,27,14,32);
  //Rotamos el display. Los botones quedan debajo
  display.rotation(1);
  //Inicialización de sensores
  GAM.begin();
  //Inicialización de partículas
  beginParticles();
}

void loop() {
  //Si se ha pulsado el botón...inicializamos de nuevo las partículas
  if(button1.pressed()) beginParticles();
  //Leemos los datos actuales del acelerómetro
  GAM.readA(dataA);
  //Calculamos aceleración para cada eje
  //Teniendo en cuenta la rotación de pantalla, cambiamos el signo al eje Y
  accelX=refXY[0]-dataA[0];
  accelY=dataA[1]-refXY[1];

  //Recorremos todas las partículas
  for(uint16_t p=0;p<particles;p++) {
    int32_t destX32,destY32; //Coordenadas de destino en resolución completa
    int16_t destX16,destY16; //Coordenadas de destino en resolución de pantalla

    //Calculamos la velocidad en base a la aceleración
    //Añadimos un sumando aleatorio
    speedX[p]+=accelX+random(-100,100);
    speedY[p]+=accelY+random(-100,100);
    //Calculamos las coordenadas destino
    destX32=coorX[p]+speedX[p];
    destY32=coorY[p]+speedY[p];
    destX16=destX32>>16;
    destY16=destY32>>16;

    //Si la coordenada destino horizontal está fuera de pantalla...
    if(destX16<0 || destX16>=xRes) {
      //...Rebota = Cambiamos la velocidad horizontal
      speedX[p]=-speedX[p]*elastic/100;
    } else { //La coordenada destino horizontal está en pantalla
      //Si el pixel origen y destino es el mismo...
      if((coorX[p]>>16)==destX16) {
        //Sólo actualizamos las coordenadas
        coorX[p]=destX32;
      } else { //El pixel origen y destino son distintos
        //Si el pixel destino está ocupado...
        if(m.getPixel(destX16,coorY[p]>>16).get24()>0) {
          //...ha chocado. Cambiamos la velocidad horizontal y no la movemos
          speedX[p]=-speedX[p]*elastic/100;
        } else { //Si el pixel destino está libre...
          //Borramos la partícula en origen
          drawPixel(p,{0,0,0});
          //Movemos la coordenada horizontal
          coorX[p]=destX32;
          //Dibujamos el pixel de la nueva posición
          drawPixel(p,color[p]);
        }
      }
    }

    //Si la coordenada vertical está fuera de pantalla...
    if(destY16<0 || destY16>=yRes) {
      //...Rebota = Cambiamos la velocidad vertical
      speedY[p]=-speedY[p]*elastic/100;
    } else { //La coordenada vertical está en pantalla
      //Si el pixel origen y destino es el mismo...
      if((coorY[p]>>16)==destY16) {
        //Sólo actualizamos las coordenadas
        coorY[p]=destY32;
      } else { //El pixel origen y destino son distintos
        //Si el pixel destino está ocupado...
        if(m.getPixel(coorX[p]>>16,destY16).get24()>0) {
          //...ha chocado. Cambiamos la velocidad vertical y no la movemos
          speedY[p]=-speedY[p]*elastic/100;
        } else { //Si el pixel destino está libre...
          //Borramos la partícula en origen
          drawPixel(p,{0,0,0});
          //Movemos la coordenada vertical
          coorY[p]=destY32;
          //Dibujamos el pixel de la nueva posición
          drawPixel(p,color[p]);
        }
      }
    }
  }
}
