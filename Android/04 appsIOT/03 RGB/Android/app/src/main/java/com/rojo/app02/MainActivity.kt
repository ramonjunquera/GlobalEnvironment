/*
  Tema: Control remoto de un led RGB
  Autor: Ramón Junquera
  Fecha: 20210513
  Controles:
  - Dirección ip y puerto para que el usuario defina el servidor.
  - Switch para establecer conexión
  - 3 SeekBar, una por canal de color, que nos permiten seleccionar el color.
  - View que toma el color resultante de los los canales
  Protocolo:
  - El valor 255 se considera un comando. Servirá para indicar que se envía un color nuevo.
  - Un color consta de 3 bytes con cada uno de los canales de color.
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
    fondo del View con el color resultante y enviamos un comando de reset (valor 255) y 3 bytes
    en con los valores de canal de color en orden RGB al servidor.
    Por seguridad (aunque no obligatorio), el envío se hace desde un Thread en segundo plano.
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
import android.widget.SeekBar
import androidx.appcompat.app.AppCompatActivity
import com.rojo.app02.databinding.ActivityMainBinding
import java.net.Socket

//Variable globales
private lateinit var client: Socket //Declaramos el objeto de gestión del Socket

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
    //Localizamos la vista y asignamos un color para su fondo.
    //El color estará compuesto por los canales de color RGB.
    ui.view.setBackgroundColor(Color.rgb(r,g,b))
    if(ui.switchConectar.isChecked) { //Si tenemos conexión...
      val out=client.getOutputStream() //Anotamos el OutputStream para enviar por el cliente
      //No es obligatorio, pero sí conveniente lanzar la operación de envío en segundo plano
      //Las posibles esperas no afectarán a la fluidez de la aplicación
      Thread {
        try {
          out.write(255) //Index channel reset (nuevo color)
          out.write(r) //Enviamos canal R
          out.write(g) //Enviamos canal G
          out.write(b) //Enviamos canal B
        } catch (e: Exception) { //Si tenemos algún un problema...
          //...el envío se pierde
        }
      }.start() //Y lo lanzamos inmediatamente
    }
  }
  //El resto de métodos no nos utilizaremos, pero su declaración es obligatoria
  override fun onStartTrackingTouch(seekBar: SeekBar?) {}
  override fun onStopTrackingTouch(seekBar: SeekBar?) {}
}

class MainActivity : AppCompatActivity() {
  private lateinit var ui:ActivityMainBinding //Acceso a los objetos gráficos
  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    ui=ActivityMainBinding.inflate(layoutInflater)
    setContentView(ui.root)

    //Instanciamos un único Listener para todas las SeekBar
    val myListener=MySeekBarListener(ui)
    //Lo utilizamos como gestor de eventos de todas las SeekBar
    ui.seekBarR.setOnSeekBarChangeListener(myListener)
    ui.seekBarG.setOnSeekBarChangeListener(myListener)
    ui.seekBarB.setOnSeekBarChangeListener(myListener)

    ui.switchConectar.setOnClickListener { //El switch ha cambiado su estado
      //Creamos un Handler para poder añadir tareas al Looper
      val mainHandler=Handler(Looper.getMainLooper())
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
            //Simulamos que se ha producido un evento en alguna de las SeekBar llamando al
            //método adecuado del Listener que las gestiona.
            //De esta manera forzamos una actualización del color del View (que no nos importa)
            //y el envío del color actual (que es realmente lo que queremos)
            //Los parámetros son obligatorios, pero sabemos que no se tienen en cuenta
            //así que enviaremos cualquier cosa
            myListener.onProgressChanged(null,0,false)
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