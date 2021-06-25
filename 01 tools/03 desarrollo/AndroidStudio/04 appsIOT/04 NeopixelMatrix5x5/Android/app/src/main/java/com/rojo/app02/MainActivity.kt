/*
  Tema: Control remoto de una matriz de leds RGN Neopixel 5x5
  Autor: Ramón Junquera
  Fecha: 20210517
  Controles:
  - Dirección ip y puerto para que el usuario defina el servidor.
  - Switch para establecer conexión
  - 3 SeekBar, una por canal de color, que nos permiten seleccionar el color.
  - Botón con el color seleccionado
  - Matriz de 5x5 botones para poder asignar el color seleccionado y dibujar
  Protocolo:
  - El valor 255 se considera un comando. Servirá para indicar que se envía un color nuevo.
  - Una imágen consta de 5 filas, 5 columnas y 3 canales de color.
  - La imágen se envía por filas, columnas y canales de color.
  - Los canales de color tienen como valor máximo 254, para no ser confundidos con el comando reset.
  - Cada vez que se vaya a enviar un nuevo color, se enviará primero el código reset. Esto
    asegurará la correcta sincronización de canales, aunque se produzcan errores de comunicación.
  Funcionamiento:
  - La conexión es algo que se debe poder utilizar en cualquiera de las clases que definamos,
    por eso se declara como variable global.
  - Puesto que el color debe recalcularse con la modificación de cualquiera de las SeekBar, creamos
    un Listener único para todas las SeekBar.
  - El Listener admite como parámetro un Binding que nos permite acceder a los objetos gráficos.
  - Tenemos la obligación de sobreescribir todos los métodos de la clase original del Listener,
    aunque no lo vayamos a utilizar.
  - Cuando se modifica cualquiera de las SeekBar, tomamos el progreso de cada una, pintamos el
    fondo del botón de selección.
  - Para evitar que las SeekBar generen el código reset, se limitan a 254.
  - Cuando solicita conexión con el control Switch, lanzamos un Thread para que gestione las
    operaciones de conexión y desconexión sin que afecte al hilo principal.
  - Si queremos conectar, desactivamos los controles de ip y puerto, tomamos sus valores e
    intentamos la conexión con el socket.
    Si no lo conseguimos, volveremos adejar los controles como al principio.
    Si conectamos llámamos al método de nuestro Listener que gestiona los cambios en las SeekBar.
    Así forzamos a que se envíe una actualización del color actual al servidor.
  Requerimientos:
  - Es necesario indicar que la aplicación necesitará permisos para conectarte con el exterior.
    Esto se hace en el archivo app/manifest/AndroidManifest.xml
    Incluiremos al final del archivo, bajo la sección manifest, y después de la sección application
    la siguiente línea:
      <uses-permission android:name="android.permission.INTERNET" />
  Notas:
  - Cuando enviamos un comando, no se cierra la comunicación a continuación. Por lo tanto no
    es necesario forzar la descarga del buffer de salida (flush).
  - Las actualizaciones de objetos gráficos desde Thread secundarios no está permitida. Siempre
    de debe hacer desde el Thread principal. Por eso se utiliza el Looper. Se añaden tareas mediante
    un Handler
*/

package com.rojo.app02

import android.graphics.Color
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.view.View
import android.widget.Button
import android.widget.SeekBar
import androidx.appcompat.app.AppCompatActivity
import com.rojo.app02.databinding.ActivityMainBinding
import java.net.Socket


//Variable globales
private lateinit var client: Socket //Declaramos el objeto de gestión del Socket
private var selectedColor:Int=0 //Color del botón de selección
private var buttonsColor=arrayOf<Array<Int>>() //Array bidimensional de colores
private var pickColor=false //Se quiere tomar un color?

//Definimos una clase heredada del Listener de SeekBar
//Tiene un parámetro con el Binding que nos permite el acceso a los objetos gráficos
//Se considerará una constate privada
//No la definimos como constante global porque las variables de estáticas de contexto (como esta)
//no se deben declarar como públicas (se pierde memoria en ello según indica el warning)
class MySeekBarListener(private val ui:ActivityMainBinding): SeekBar.OnSeekBarChangeListener {
  override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
    //Obtenemos los valores de los canales
    val r=ui.seekBarR.progress
    val g=ui.seekBarG.progress
    val b=ui.seekBarB.progress
    //Componemos el color de botón de selección de color y lo guardamos
    selectedColor=Color.rgb(r,g,b)
    //Asignamos este el color al botón de selección
    ui.buttonColor.setBackgroundColor(selectedColor)
  }
  //El resto de métodos no nos utilizaremos, pero su declaración es obligatoria
  override fun onStartTrackingTouch(seekBar: SeekBar?) {}
  override fun onStopTrackingTouch(seekBar: SeekBar?) {}
}

//Definimos una clase heredada del Listener de una vista para aplicarla los botones
//Tenemos dos parámetros:
//- ui para acceder a todos los objetos gráficos
//- mainHandler para gestionar el Handler de la Activity
class MyClickListener(private val ui:ActivityMainBinding,private val mainHandler:Handler):View.OnClickListener {
  //Obligatorio sobreescribir todos sus métodos
  override fun onClick(v: View?) {
    val buttonName=v!!.resources.getResourceName(v.id) //Obtenemos el nombre del botón
    val buttonNameLength=buttonName.length //Su longitud
    val buttonRow=buttonName.substring(buttonNameLength-2,buttonNameLength-1).toInt() //Su fila
    val buttonCol=buttonName.substring(buttonNameLength-1).toInt() //Su columna
    if(pickColor) { //Si se quiere tomar el color del botón pulsado...
      val color= buttonsColor[buttonRow][buttonCol] //Obtenemos color de botón pulsado
      ui.buttonColor.setBackgroundColor(color) //Lo asignamos a botón de selección
      mainHandler.post { //Añadimos la rutina a ejecutar por el Handler de la Activity
        //Aplicamos los valores de los canales a las SeekBar
        ui.seekBarR.progress=Color.red(color)
        ui.seekBarG.progress=Color.green(color)
        ui.seekBarB.progress=Color.blue(color)
      }
      pickColor=false //Ya se ha tomado el color
    } else { //Se quiere asignar el color seleccionado
      v.setBackgroundColor(selectedColor) //Copiamos el color del botón selector al actual
      buttonsColor[buttonRow][buttonCol]= selectedColor //Lo anotamos en el Array
      if(ui.switchConectar.isChecked) { //Si tenemos conexión...
        send() //...enviamos la imágen actual
      }
    }
  }
  fun send() { //Envía la imágen actual
    val out=client.getOutputStream() //Anotamos el OutputStream para enviar por el cliente
    //No es obligatorio, pero sí conveniente lanzar la operación de envío en segundo plano
    //Las posibles esperas no afectarán a la fluidez de la aplicación
    Thread {
      try {
        out.write(255) //Index channel reset (nuevo color)
        for(row in 0..4) { //Recorremos todas las filas
          for(col in 0..4) { //Recorremos todas las columnas
            val color=buttonsColor[row][col] //Anotamos el color
            out.write(Color.red(color)) //Enviamos canal R
            out.write(Color.green(color)) //Enviamos canal G
            out.write(Color.blue(color)) //Enviamos canal B
          }
        }
      } catch (e: Exception) { //Si tenemos algún un problema...
        //...el envío se pierde
      }
    }.start() //Y lo lanzamos inmediatamente
  }
}

class MainActivity : AppCompatActivity() {
  private lateinit var ui: ActivityMainBinding //Acceso a los objetos gráficos
  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    ui=ActivityMainBinding.inflate(layoutInflater)
    setContentView(ui.root)
    val mainHandler=Handler(Looper.getMainLooper()) //Handle de la Activity

    //Rellenamos los colores de todos los botones a negro
    for(row in 0..4) { //Recorremos todas las filas
      buttonsColor+=arrayOf(0,0,0,0,0) //Añadimos una fila más
    }

    //Instanciamos un único Listener para todas las SeekBar
    val mySeekBarListener=MySeekBarListener(ui)
    //Lo utilizamos como gestor de eventos de todas las SeekBar
    ui.seekBarR.setOnSeekBarChangeListener(mySeekBarListener)
    ui.seekBarG.setOnSeekBarChangeListener(mySeekBarListener)
    ui.seekBarB.setOnSeekBarChangeListener(mySeekBarListener)

    //Instanciamos un único Listener para todos los Button
    val myClickListener=MyClickListener(ui,mainHandler)
    //Recorremos todos los botones por filas y columnas
    for(row in 0..4) {
      for (col in 0..4) {
        //Obtenemos el id del Button por el nombre
        val idButton = resources.getIdentifier("button$row$col", "id", packageName)
        //Con el id obtenemos el objeto y le asignamos el Listener
        findViewById<Button>(idButton).setOnClickListener(myClickListener)
      }
    }

    ui.buttonColor.setOnClickListener { //Se ha pulsado el botón de selección de color
      pickColor=true //Activamos el flag
    }

    ui.switchConectar.setOnClickListener { //El switch ha cambiado su estado
      Thread { //Creamos un Thread para gestionar la conexión/desconexión
        if(ui.switchConectar.isChecked) { //Si quiere conectar...
          //Solicitamos la desactivación de los controles de ip & puerto
          mainHandler.post { //Añadimos la rutina a ejecutar por el Handler
            ui.editTextIP.isEnabled=false //Control ip desactivado
            ui.editTextPort.isEnabled=false //Control puerto desactivado
          }
          try {
            //Intentamos conectar
            client=Socket(ui.editTextIP.text.toString(),ui.editTextPort.text.toString().toInt())
            //No ha dado error. Hemos conectado!
            //Pedimos al Listener de los botones que envíe la imágen actual
            myClickListener.send()
          } catch (e: Exception) { //Si ocurre cualquier problema (no hemos podido conectar)...
            //Solicitamos dejar los controles como al principio
            mainHandler.post { //Añadimos la rutina a ejecutar por el Handler
              ui.switchConectar.isChecked=false //No estamos conectados
              ui.editTextIP.isEnabled=true //Control IP activado
              ui.editTextPort.isEnabled=true //Control puerto activado
            }
          }
        } else { //Si se quiere desconectar...
          try {
            client.close() //Cerramos la conexión con el Socket
          } catch (e: Exception) { //Si ocurre cualquier problema...
            //No hacemos nada especial
            //Esto puede ocurrir si queremos cerrar una conexión que ya se ha perdido
          }
          //Solicitamos la activación de los controles de ip & puerto
          mainHandler.post { //Añadimos la rutina a ejecutar por el Handler
            ui.editTextIP.isEnabled=true
            ui.editTextPort.isEnabled=true
          }
        }
      }.start() //Lanzamos el Thread que conecta/desconecta
    }
  }
}