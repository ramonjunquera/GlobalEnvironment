//Librerías de gestión de redes
import processing.net.*;

//Puerto
int port=5204;
//Objeto servidor
Server myServer;
//Array de imágenes
PImage[] bulbs;

void setup()
{
  //Creamos el servidor en un puerto determinado
  myServer = new Server(this,port);
  //Mostramos los datos de conexión del servidor
  println("Server running at "+myServer.ip()+":"+str(port));
  
  //Dimensionamos el array de imágenes
  bulbs=new PImage[2];
  //Las cargamos
  bulbs[0]=loadImage("bulb-off-128.png");
  bulbs[1]=loadImage("bulb-on-128.png");
  
    //Preparamos el entorno gráfico
  size(128,128);
  //Comenzamos dibujando el estado apagado
  image(bulbs[0],0,0);
}

void draw()
{
  //Bucle principal
  
  //Anotamos la siguiente conexión cliente disponible
  Client myClient = myServer.available();
  //Si la tenemos algún cliente conectado...
  if (myClient!=null)
  {
    //Si el cliente actual tiene información pendiente de ser recibida...dibujamos el estado correspondiente
    if(myClient.available()>0) image(bulbs[myClient.read()==0?0:1],0,0);
    //Si el cliente no tiene información pendiente...cerramos su conexión
    else myServer.disconnect(myClient); 
  } 
}
