/*
  Autor: Ramón Junquera
  Tema: Processing. Comunicación serie
  Fecha: 20180715
  Objetivo: Interactuar desde Arduino con Processing
  Material adicional: cualquier placa

  Descripción:
  Tomamos como base un circuito simple que sólo contiene un interruptor.
  Si la placa tiene un interruptor incluido, como 'DOIT ESP32 DevKit V1' o 'ESP32 OLED TTGO 16Mb'
  lo utilizaremos. Sino, incluiremos un interruptor externo asociado al pin 0 en placas ESP32.

  El objetivo del desarrollo es que cada vez que se detecte una pulsación en el interruptor, se
  envíe un carácter/byte determinado a Processing.
  En Processing se recibirá el carácter clave y se cambiará el estado booleano que guarda que es
  representado por una imagen de una bombilla (encendida o apagada) que se muestra en tiempo real.

  En Processing, cargaremos inicialmente todas las imágenes utilizadas para no tener que cargarlas
  cada vez que se requiere.
  Las imágenes se encuentran en la misma carpeta del desarrollo.

  Como carácter clave tomaremos 'X'.

  Utilizaremos la librería de gestión de interruptores para simplificar el desarrollo.

  Hay otra parte muy importante en la inicialización: la detección del puerto serie usado para la
  comunicación.

  En el PC podemos tener varios puertos serie que pueden ser creados de manera dinámica.
  Podríamos definir una constante global al principio del desarrollo, pero no tendría porqué se
  correcta en todos los casos. Además se agrava teniendo en cuenta que los puertos serie no tienen
  la misma nomenclatura en Windows que en Linux.

  Para evitar hacer modificaciones, hemos creado un detector automático de puerto serie.

  El funcionamiento en simple.
  Desde Arduino nos dedicamos a enviar periódicamente un carácter de control que servirá para informar
  a quien escuche que pretendemos utilizar ese puerto.
  Al mismo tiempo estamos escuchando por si se recibe respuesta. Si recibimos el carácter adecuado
  entenderemos que alguien nos ha escuchado y que además entiende y acepta el puerto de comunicación.
  En ese momento dejamos de enviar caracteres de control y pasamos a funcionamiento real del
  desarrollo.
  Desde Processing recorreremos todos los puertos existentes secuencialmente.
  Utilizamos un control de excepciones por si el puerto no está realmente disponible o no es accesible.
  Si conseguimos abrir el puerto, damo un tiempo prudencial para poder recibir algún carácter.
  Si recibimos el carácter de control, automáticamente respondemos por el mismo puerto y lo dejamos,
  Si no, lo intentamos con el siguiente puerto.
  
  Tanto el carácter que envía Arduino como el que responde Processing para confirmar se han definido como '0'.
  
  Resultado:
  Una vez detectado el puerto de comunicación, cuando se pulsa el interruptor en la placa, cambia el estado
  de la imagen en Processing
*/

//Gestión de puerto serie
import processing.serial.*; 

//Declaramos clase de gestión de puerto serie
Serial mySerial;
//Estado actual. 0=apagado, 1=encendido
int currentStatus=0;
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
  while(recChar!='0')
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
  mySerial.write('0');
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
  //Dibujamos el estado actual
  image(bulbs[currentStatus],0,0);
}

void draw()
{
  //Si tenemos caracteres pendientes por recibir...
  if(mySerial.available()>0)
  {
    //Si el carácter recibido es el esperado...
    if(mySerial.read()=='X')
    {
      //Recibido el caracter que indica que se ha pulsado el interruptor!
      //Cambiamos el estado actual: 1 <-> 0
      currentStatus=1-currentStatus;
      //Dibujamos el estado actual
      image(bulbs[currentStatus],0,0);
    }
  }
}
