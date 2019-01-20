/*
  Autor: Ramón Junquera
  Tema: Processing. Comunicación web socket
  Fecha: 20180803
  Objetivo: Prueba de concepto de client y servidor socket con Processing
  Material adicional: ninguno

  Descripción:
  Crearemos dos aplicaciones en Processing, un cliente y un servidor.
  El cliente se conectará al servidor (una determinada ip y puerto) y enviará un byte
  cada segundo. El valor se incrementará en cada envío y volverá a cero después de 
  alcanzar el desbordamiento.
  El cliente mostrará en su consola los valores enviados.
  
  El servidor recibirá datos de cualquier conexión cliente y mostrará en su consola
  la información recibida.
  Cuando el servidor arranca, muestra su dirección ip para facilitar su localización.
  Tras procesar la información recibida, fuerza la desconexión del cliente.
  Esta acción facilita que se puedan procesar varios clientes simultáneamente porque no
  se alcanza nunca el límite de conexiones simultáneas máximas.
*/

//Librerías de gestión de redes
import processing.net.*;

//Objeto servidor
Server myServer;

void setup()
{
  //Creamos el servidor en un puerto determinado
  myServer = new Server(this, 5204); 
  println("Server running at "+myServer.ip());
}

void draw()
{
  //Bucle principal
  
  //Anotamos la siguiente conexión cliente disponible
  Client myClient = myServer.available();
  //Si la tenemos algún cliente conectado...
  if (myClient!=null)
  {
    //Si el cliente actual tiene información pendiente de ser recibida...mostramos el valor del siguiente byte recibido
    if(myClient.available()>0) print(myClient.read()+" ");
    //Si el cliente no tiene información pendiente...cerramos su conexión
    else myServer.disconnect(myClient); 
  } 
}
