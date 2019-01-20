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

//Valor a enviar
byte value=0; 
 
void setup()
{
  //Nada especial que hacer aquí
} 
 
void draw()
{
  //Bucle principal
  
  //Creamos el objeto de gestión de una conexión cliente
  //Conectamos con la máquina local por un puerto determinado
  //Aquí puede producirse un error si la dirección no existe o el puerto está cerrado
  Client myClient = new Client(this, "127.0.0.1", 5204);
  //Enviamos el byte al servidor
  myClient.write(value);
  //Mostramos el valor enviado
  print(value);
  print(" ");
  //Incrementamos el valor para no enviar siempre el mismo
  value++;
  //Esperamos un segundo
  delay(1000);
} 
