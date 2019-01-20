/*
  Autor: Ramón Junquera
  Tema: Processing. Comunicación serie
  Fecha: 20180721
  Objetivo: Enviar información desde Processing a Arduino
  Material adicional: cualquier placa

  Descripción:
  Toda el espacio de visualización estará ocupado por una imagen de una bombilla.
  Su estado (encendido/apagado) cambiará al detectarse una pulsación del ratón en
  cualquier punto.
  Además de actualizar la imagen, se enviará un código a Arduino por el puerto serie
  que para que se le aplique el mismo estado al led integrado en placa.
  Los códigos serán:
    0 : apagar
    1 : encender
  Estos códigos corresponden con el estado en Processing.
  
  Al igual que todos los ejercicios en los que se utilice el puerto serie, se implementa
  la rutina de autoidentificación de puerto.
  
  Resultado:
  Cuando cambiamos el estado de la imagen en processing con un click de ratón, también se
  cambia en el led integrado en placa.
*/

//Gestión de puerto serie
import processing.serial.*; 

//Declaramos clase de gestión de puerto serie
Serial mySerial;
//Estado actual. 0=apagado, 1=encendido
int currentStatus=1;
//Array de imágenes
PImage[] bulbs;

void setup()
{
  print("Detecting serial port.");
  //Carácter recibido
  int recChar='a'; //Cualquier carácter distinto al esperado para que funcione la primera vez
  //Puerto serie procesado
  int serialCurrent=0;
  //Mientras no recibamos el caracter esperado...
  while(recChar!='R')
  {
    print(".");
    //Pasamos al siguiente puerto serie. Nunca debe superar el número de puertos serie del sistema
    //Recorremos los puertos serie en orden descendente porque los puertos USB quedan al final
    //Si pasamos a una posición negativa...asignamos la última posición actual
    if(--serialCurrent<0) serialCurrent=Serial.list().length-1;
    //Incluimos la asignación del puerto dentro de un try porque si el puerto no es válido dará error
    try
    {
      //Asignamos a nuestro puerto serie el puerto que se está procesando
      mySerial=new Serial(this,Serial.list()[serialCurrent],115200);
      //Damos tiempo suficiente para que se pueda recibir un nuevo carácter
      //Nota:
      //Las placas Arduino se reinician cuando se abre su puerto serie asociado. Las ESP no.
      //Puesto que aquí vamos probando todos los puertos serie disponibles, abrimos la conexión.
      //La placa Arduino se reinicia automáticamante y debemos darle tiempo para ello y para que 
      //comience a enviar su carácter clave.
      //Si reducimos este tiempo, la placa Arduino no podrá completar su reinicio. Enviará su 
      //carácter clave, será respondido por Processing y no le dara tiempo a recibirlo y reconocerlo.
      //Y continuará enviando el carácter clave de manera indefinida.
      //En placas ESP, que reinician más rápido y que no son afectadas por la apertura del puerto,
      //se podría utilizar una demora de 600ms.
      //Lo dejaremos con 2s para guardar compatibilidad con cualquier placa
      delay(2000);
      //Si tiene caracteres pendientes por recibir...obtenemos el siguiente
      if(mySerial.available()>0) recChar=mySerial.read();
    }
    //Importante utilizar variable de tipo Exception y no IOException. Con la segunda da error
    catch(Exception ex)
    {
      //La asignación del puerto ha fallado
      //Anotamos como carácter cualquiera que sea distinto al esperado
      recChar='a';
      delay(100);
    }
  }
  //Hemos recibido el carácter clave!
  //Enviamos el código de parada
  mySerial.write('J');
  //Esperamos un momento
  delay(100);
  //Vaciamos el buffer recibido
  mySerial.clear();
  //Informamos del puerto serie indentificado
  println(Serial.list()[serialCurrent]);
  
  //Dimensionamos el array de imágenes
  bulbs=new PImage[2];
  //Las cargamos
  bulbs[0]=loadImage("bulb-off-128.png");
  bulbs[1]=loadImage("bulb-on-128.png");

  //Preparamos el entorno gráfico
  size(128,128);
  //Cambiamos el estado, actualizamos la imagen y enviamos el comando a Arduino 
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
  //Enviamos el estado actual por el puerto serie
  //El carácter ASCII 48 es el '0'
  mySerial.write(48+currentStatus);
}
