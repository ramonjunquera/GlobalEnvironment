/*
  Tema: Creación y gestión de objeto gráfico
  Autor: Ramón Junquera
  Fecha: 20210528
  Controles:
  - Un EditText para la dirección ip.
  - Un EditText para el puerto.
  - Un Switch para poder conectar/desconectar
  - 3 SeekBar, una por canal de color, que nos permiten seleccionar el color.
  - View con el color seleccionado
  - Objeto View8 que representa una matriz de 8x8
  Funcionamiento:
  - Con el Switch podemos conectar con el servidor teniendo en cuenta los parámetros de ip & puerto
  - Las 3 SeekBar permiten componer el color asignado al botón.
  - Cuando se pulsa en el objeto View8 se pueden pintar los círculos con el color seleccionado.
  - La pulsación puede arrastrarse
  - Al pulsar en el View de color seleccionado se puede tomar alguno de los que ya se han utilizado
  Notas:
  - Dentro de la clase de la Activity, creamos una clase heredada del Listener de SeekBar que
    permite cambiar el color seleccionado.
  - Esta clase la instanciaremos y la aplicaremos como Listener a todas las SeekBar, porque todas
    tienen el mismo funcionamiento. Cuando hay una variación en cualquiera de ellas, se debe
    recalcular el color y aplicar.
  - Ante cualquier cambio de color, el Listener calcula el nuevo color, lo aplica al View del color
    seleccionado y notifica a la clase ViewDrawable para que lo guarde como nuevo color seleccionado
  Requerimientos:
  - Es necesario indicar que la aplicación necesitará permisos para conectarte con el exterior.
    Esto se hace en el archivo app/manifest/AndroidManifest.xml
    Incluiremos al final del archivo, bajo la sección manifest, y después de la sección application
    la siguiente línea:
      <uses-permission android:name="android.permission.INTERNET" />
*/

package com.rojo.app01

import android.graphics.Color
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.support.v7.app.AppCompatActivity
import android.widget.SeekBar
import com.rojo.app01.databinding.ActivityMainBinding
import java.net.Socket

//Gestión principal de Activity
class MainActivity : AppCompatActivity() {
    //Variable global de clase para acceso a objetos gráficos
    private lateinit var ui: ActivityMainBinding

    //Creamos clase heredada del Listener de SeekBar
    //Puede acceder a los objetos gráficos a través de la variable global ui
    inner class MySeekBarListener : SeekBar.OnSeekBarChangeListener {
        override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
            //Obtenemos los valores de los canales y componemos el color
            val color=Color.rgb(ui.seekBarR.progress,ui.seekBarG.progress,ui.seekBarB.progress)
            //Asignamos el color al objeto gráfico
            ui.viewColor.setBackgroundColor(color)
            //Informamos a ViewDrawable del nuevo color seleccionado
            ui.view.setInkColor(color)
        }
        //El resto de métodos no los utilizaremos, pero su declaración es obligatoria
        override fun onStartTrackingTouch(seekBar: SeekBar?) {}
        override fun onStopTrackingTouch(seekBar: SeekBar?) {}
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        ui=ActivityMainBinding.inflate(layoutInflater)
        setContentView(ui.root)

        //Definimos objeto para poder añadir tareas al Looper
        val mainHandler= Handler(Looper.getMainLooper()) //Handle de la Activity

        //Inicializamos objeto View8
        //El parámetro es la función a la que se llama cuando hemos activado el flag de recuperar
        //el color utilizado en la siguiente pulsación
        ui.view.begin(fun(color:Int) {
            //Recibimos el color de la pulsación cuando hemos activado el flag
            //Descomponemos el color y lo aplicamos al progreso de las SeekBar
            ui.seekBarR.progress=Color.red(color)
            ui.seekBarG.progress=Color.green(color)
            ui.seekBarB.progress=Color.blue(color)
            //No asignamos el color al botón, porque con cada cambio en las Seekbar se actualiza
        })

        //Instanciamos un único Listener para todas las SeekBar
        val mySeekBarListener=MySeekBarListener()
        ui.seekBarR.setOnSeekBarChangeListener(mySeekBarListener)
        ui.seekBarG.setOnSeekBarChangeListener(mySeekBarListener)
        ui.seekBarB.setOnSeekBarChangeListener(mySeekBarListener)

        //Definimos función de gestión de cambio del objeto Switch
        ui.switchConectar.setOnClickListener {
            Thread { //Creamos un Thread para gestionar la conexión/desconexión
                if(ui.switchConectar.isChecked) { //Si quiere conectar...
                    //Solicitamos la desactivación de los controles de ip & puerto
                    mainHandler.post { //Añadimos la rutina a ejecutar por el Handler
                        ui.editTextIP.isEnabled=false //Control ip desactivado
                        ui.editTextPort.isEnabled=false //Control puerto desactivado
                    }
                    try {
                        //Intentamos conectar
                        ui.view.client=Socket(ui.editTextIP.text.toString(),ui.editTextPort.text.toString().toInt())
                        //No ha dado error. Hemos conectado!
                        ui.view.send() //Enviamos imágen actual
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
                        ui.view.client?.close() //Cerramos la conexión con el Socket
                    } catch (e: Exception) { //Si ocurre cualquier problema...
                        //No hacemos nada especial
                        //Esto puede ocurrir si queremos cerrar una conexión que ya se ha perdido
                    }
                    ui.view.client=null //Estamos desconectados
                    //Solicitamos la activación de los controles de ip & puerto
                    mainHandler.post { //Añadimos la rutina a ejecutar por el Handler
                        ui.editTextIP.isEnabled=true
                        ui.editTextPort.isEnabled=true
                    }
                }
            }.start() //Lanzamos el Thread que conecta/desconecta
        }

        //Definimos función de gestión de click en el view de selección de color
        ui.viewColor.setOnClickListener {
            //Queremos recuperar el color de la siguiente pulsación. Activamos el flag
            ui.view.getColorNextTouch()
        }
    }
}
