//Librerías de gestión de redes
import processing.net.*; 

//Estado actual. 0=apagado, 1=encendido
int currentStatus=1;
//Array de imágenes
PImage[] bulbs;
//Servidor
String host="192.168.1.115";
//Puerto
int port=5204;

void setup()
{
  //Dimensionamos el array de imágenes
  bulbs=new PImage[2];
  //Las cargamos
  bulbs[0]=loadImage("bulb-off-128.png");
  bulbs[1]=loadImage("bulb-on-128.png");

  //Preparamos el entorno gráfico
  size(128,128);
  //Cambiamos el estado, actualizamos la imagen y enviamos el estado actual al servidor 
  mousePressed();
}

void draw()
{
  //Nada especial que hacer aquí
}

void mousePressed()
{
  //Se ha presionado el botón del ratón
  
  //No nos importa en qué coordenadas se ha pulsado porque la imagen ocupa todo el área visible
  
  //Cambiamos el estado actual: 1 <-> 0
  currentStatus=1-currentStatus;
  //Dibujamos el estado actual
  image(bulbs[currentStatus],0,0);
  //Enviamos el estado actual al servidor
  
  //Creamos el objeto de gestión de una conexión cliente
  //Conectamos con el servidor por un puerto determinado
  //Aquí puede producirse un error si la dirección no existe o el puerto está cerrado
  Client myClient = new Client(this,host,port);
  //Enviamos el estado al servidor
  myClient.write(currentStatus);
  //Mostramos el valor enviado
  println(currentStatus==1?"encendido":"apagado");
}
