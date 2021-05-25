/*
  Tema: Envío de valor binario a un socket tcp
  Autor: Ramón Junquera
  Fecha: 20210512
  Funcionamiento:
    Permite conectar con un socket tcp a una dirección y puerto seleccionable.
    Mantiene la conexión. Envía un byte para indicar los comandos.
    Interactua con los elementos gráficos, desactivando los que no son necesarios.
  Requerimientos:
  - Es necesario indicar que la aplicación necesitará permisos para conectarte con el exterior.
    Esto se hace en el archivo app/manifest/AndroidManifest.xml
    Incluiremos al final del archivo, bajo la sección manifest, y después de la sección application
    la siguiente línea:
      <uses-permission android:name="android.permission.INTERNET" />
  Notas:
  - Cuando enviamos un comando, no se cierra la comunicación a continuación. Por lo tanto no
    es necesario forzar la descarga del buffer de salida (flush).
  - Los comandos de comunicaciones (conectar, desconectar, enviar) deben ser ejecutado en un Thread
    secundario, por política de Android.
  - Las actualizaciones de objetos gráficos desde Thread secundarios no está permitida. Siempre
    de debe hacer desde el Thread principal. Por eso se utiliza el Looper. Se añaden tareas mediante
    un Handler
  - Se reduce al máximo el volumen de datos transmitidos. Sólo envían comandos de un solo byte.
    0 para apagar y 1 para encender.
  - Puesto que la comunicación se mantiene abierta, es necesario que el objeto Socket se guarde
    en una variable de clase privada para tenerla disponible desde cualquier método.
*/

package com.rojo.app02

import android.os.Bundle
import android.os.Handler
import android.os.Looper
import androidx.appcompat.app.AppCompatActivity
import com.rojo.app02.databinding.ActivityMainBinding
import java.net.Socket

class MainActivity : AppCompatActivity() {
  private lateinit var ui:ActivityMainBinding

  private lateinit var client: Socket //Declaramos el objeto de gestión del Socket

  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    ui=ActivityMainBinding.inflate(layoutInflater)
    setContentView(ui.root)

    ui.switchConectar.setOnClickListener { //El switch ha cambiado su estado
      //Creamos un Handler para poder añadir tareas al Looper
      val mainHandler = Handler(Looper.getMainLooper())
      val connDisconn=Thread { //Creamos un Thread para gestionar la conexión/desconexión
        if(ui.switchConectar.isChecked) { //Si quiere conectar...
          //Solicitamos la desactivación de los controles de ip & puerto
          mainHandler.post { //Añadimos la rutina a ejecutar por el Handler
            ui.editTextIP.isEnabled=false
            ui.editTextPort.isEnabled=false
          }
          try {
            //Intentamos conectar
            client=Socket(ui.editTextIP.text.toString(),ui.editTextPort.text.toString().toInt())
            //No ha dado error. Hemos podido conectar
            //Solicitamos la activación de los botones On & Off
            mainHandler.post { //Añadimos la rutina a ejecutar por el Handler
              ui.buttonOn.isEnabled=true
              ui.buttonOff.isEnabled=true
            }
          } catch (e: Exception) { //Si ocurre cualquier problema (no hemos podido conectar)...
            //Solicitamos dejar los controles como al principio
            mainHandler.post { //Añadimos la rutina a ejecutar por el Handler
              ui.switchConectar.isChecked=false //No estamos conectados
              ui.editTextIP.isEnabled=true //Control IP activado
              ui.editTextPort.isEnabled=true //Control puerto activado
            }
          }
        } else { //Si se quiere desconectar...
          //Solicitamos desactivar los botones On & Off
          mainHandler.post { //Añadimos la rutina a ejecutar por el Handler
            ui.buttonOn.isEnabled=false
            ui.buttonOff.isEnabled=false
          }
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
      }
      connDisconn.start() //Lanzamos el Thread que conecta/desconecta
    }

    ui.buttonOn.setOnClickListener { //Se ha pulsado el botón On
      //Enviamos comando de encender por el OutputStreamer del cliente
      client.getOutputStream().write(1)
    }

    ui.buttonOff.setOnClickListener { //Se ha pulsado el botón Off
      //Enviamos comando de apagar por el OutputStreamer del cliente
      client.getOutputStream().write(0)
    }
  }
}