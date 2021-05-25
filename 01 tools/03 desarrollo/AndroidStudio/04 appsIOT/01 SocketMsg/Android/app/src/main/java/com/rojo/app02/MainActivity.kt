/*
  Tema: Envío de texto a un socket tcp
  Autor: Ramón Junquera
  Fecha: 20210511
  Funcionamiento:
    Conecta con un socket tcp a una dirección y puerto seleccionable y envía un String
  Requerimientos:
  - Es necesario indicar que la aplicación necesitará permisos para conectarte con el exterior.
    Esto se hace en el archivo app/manifest/AndroidManifest.xml
    Incluiremos al final del archivo, bajo la sección manifest, y después de la sección application
    la siguiente línea:
      <uses-permission android:name="android.permission.INTERNET" />
  Política de procesos de comunicaciones
    La política de Threads de Android no permite que se lancen procesos de comunicaciones en el
    hilo principal. Si queremos hacerlo, utilizaremos un Thread exclusivo para ese proceso.
    En nuestro caso queremos que se abra una comunicación con un Socket, se envíe un mensaje y
    se cierre. Todas estas acciones deben incluirse en un Thread que se lance desde el principal.
    Si no lo hacemos obtendremos una excepción generado por la política de Threads.
    Podríamos modificar la política y aplicarla con las siguientes líneas:
      val policy=ThreadPolicy.Builder().permitAll().build()
      StrictMode.setThreadPolicy(policy)
    Pero esto no es la solución al problema real, que es una mala práctica de programación.
  Envío de información
    El servidor deja de comprobar si hay información pendiente de ser recibida en cuanto detecta
    que la conexión se ha cerrado.
    Para evitar que se pierda información hacemos dos cosas:
    - Tras enviar el mensaje, nos aseguramos que se vacía el buffer de salida con flush()
    - Esperamos un momento para que la información llegue al servidor y pueda ser leida y procesada.
*/


package com.rojo.app02

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import com.rojo.app02.databinding.ActivityMainBinding
import java.net.Socket

class MainActivity : AppCompatActivity() {
  private lateinit var ui:ActivityMainBinding
  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    ui=ActivityMainBinding.inflate(layoutInflater)
    setContentView(ui.root)

    ui.buttonSend.setOnClickListener { //Pulsado el botón Enviar
      val sendMsg=Thread { //Creamos un Thread para que envíe el mensaje
        try {
          //Creamos un cliente para un Socket.
          //Necesitamos la ip en String y el puerto en Int
          //Recordemos que la variable pública text de un EditText no es un String, sino un Editable!
          //Siempre tenemos que convertirlo a String con .toString()
          //Además en el puerto lo convertimos a entero con .toInt()
          val client=Socket(ui.editTextIP.text.toString(),ui.editTextPort.text.toString().toInt())
          //El mensaje debemos escribir en OutputStream del cliente
          //Tomamos nota de él, porque lo utilizaremos para varias cosas
          val out=client.getOutputStream()
          //Escribimos el mensaje en formato ByteArray
          out.write(ui.editTextMsg.text.toString().toByteArray())
          out.flush() //Antes de terminar, nos aseguramos que se envía la información pendiente
          //Esperamos un momento antes de cerrar la conexión para que puedan recibir el
          //mensaje en el servidor
          Thread.sleep(3)
          out.close() //Cerramos el OutputStream
          client.close() //Cerramos la conexión
        } catch (e: Exception) { //Si ocurre cualquier problema...
          e.printStackTrace() //...lo mostramos
        }
      }
      sendMsg.start() //Lanzamos el Thread que envía el mensaje
    }
  }
}