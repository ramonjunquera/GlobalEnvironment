/*
  Tema: Reproducción de archivos mp3 desde la tarjeta SD
  Autor: Ramón Junquera
  Fecha: 20210612
  Funcionamiento:
    Si se encuentra una tarjeta SD con el archivo mp3 correcto en la ubicacióin correcta...
    Se muestra una botonera de imágenes para:
    - Reproducir/Pausar
    - Volver al principio
    - Activar/desactivar la reproducción continua
  Estructura:
  - Dentro de la carpeta app/src/main/res/raw/ se guarda el archivo mp3
  - Puesto que a los archivos mp3 se les asignará un identificador, sus nombres no deberían tener
    caracteres especiales (espacios, guiones, etc).
  - Las imágenes de los botones se guardan en app/src/main/res/mipmap-mdpi/
  Notas:
  - Se comprueba que existe una tarjeta SD y que tiene el archivo mp3 que estramos buscando en la
    ubicación por defecto de archivos de la aplicación. Si se cumple, se activan (se hacen visibles)
    los controles y se instancia el objeto MediaPlayer.
  - Definimos como variable el tiempo transcurrido en milisegundos desde el inicio de la
    reproducción. Lo utilizamos para comenzar la reproducción desde ese punto.
  - Se crea un Listener para que atienda el click de cada botón.
  - Si no se ha instanciado el objeto MediaPlayer, no se pueden crear Listeners para él.
  - Si la reproducción cíclica está desactivada y llegamos al final de la reproducción del archivo,ç
    pasaremos a modo pausa y reiniciaremos el tiempo de reproducción.
  - En diseño, todos los objetos son invisibles por defecto-
  - Si no se detecta el archivo mp3 en la SD no se visualizarán los controles y la aplicación
    aparecerá vacía.
  - Para el ejemplo actual, hay que copiar el archivo supermariobros.mp3 en la carpeta de la SD:
    /Android/data/com.rojo.app01/files/. Este archivo se puede encontrar en la carpeta raíz del
    proyecto.
*/

package com.rojo.app01

import android.media.MediaPlayer
import android.net.Uri
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import androidx.core.content.ContextCompat
import androidx.core.view.isVisible
import com.rojo.app01.databinding.ActivityMainBinding
import java.io.File

class MainActivity : AppCompatActivity() {
  private lateinit var ui: ActivityMainBinding

  private var fileExists=false //Inicialmente el archivo no existe
  private lateinit var mp:MediaPlayer //Objeto que contendrá el audio

  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    ui=ActivityMainBinding.inflate(layoutInflater)
    setContentView(ui.root)

    val filename="supermariobros.mp3" //Nombre del archivo a reproducir

    //Comprobaremos si existe tarjeta SD y si en ella existe el archivo
    //Preguntamos por las ubicaciones para guardar archivos externos
    val dirs= ContextCompat.getExternalFilesDirs(this, null)
    for(dir in dirs) { //Recorremos todas las ubicaciones detectadas...
      if(dir != null) { //Si es un path válido...
        if(!dir.toString().contains("emulated")) { //Y es memoria externa real (SD)...
          val pathSD=dir.toString()+"/"+filename //Componemos el path completo
          if(File(pathSD).exists()) { //Si el archivo existe...
            fileExists=true //Anotamos que el archivo existe
            //Hacemos visibles los controles
            ui.imageButtonPlay.isVisible=true
            ui.imageButtonRewind.isVisible=true
            ui.switchLoop.isVisible=true
            //Instanciamos el objeto MediaPlayer con el mp3 de los recursos
            //Para la creación de un objeto MediaPlayer no podemos utilizar directamente un String
            //como path. Debemos convertirlo a formato Uri
            mp=MediaPlayer.create(this,Uri.parse(pathSD))
          }
        }
      }
    }

    var playing=false //Está reproduciendo?. Inicialmente no.
    var looping=false //Se debe hacer loop?. Inicialmente no.
    var playingTime=0 //Tiempo de reproducción en ms. Inicialmente desde el principio.

    ui.imageButtonPlay.setOnClickListener { //Pulsado botón play/pause
      if(playing) { //Si se estaba reproduciendo...
        playingTime=mp.currentPosition //Anotamos el tiempo que se lleva reproducido
        mp.pause() //Pausamos la reproducción
      } else { //Si no se estaba reproduciendo...
        mp.seekTo(playingTime) //Nos movemos al último tiempo anotado
        mp.start() //Comenzamos a reproducir
      }
      playing=!playing //Cambiamos el estado de la reproducción
    }

    ui.imageButtonRewind.setOnClickListener { //Pulsado botón rewind
      if (playing) { //Si se estaba reproduciendo...
        mp.pause() //Pausamos
        mp.seekTo(0) //Volvemos al principio
        mp.start() //Reproducimos
      }
      playingTime = 0
    }

    ui.switchLoop.setOnClickListener { //El Switch ha cambiado de estado
      looping = !looping //Cambiamos el estado del looping
      mp.isLooping = looping //Informamos al objeto MediaPlayer
    }

    if(fileExists) { //Si hemos inicializado el objeto MediaPlayer...podemos crear su Listener
      mp.setOnCompletionListener { //Si ha terminado la reproducción del audio...
        if(fileExists) {
          if (!looping) { //Y no hay que repetir...
            playing = false //Entraremos en pausa
            playingTime = 0 //La próxima vez comenzaremos desde el principio
          }
        }
      }
    }
  }

  override fun onDestroy() { //La aplicación se va a cerrar
    super.onDestroy() //Llamamos al método de la clase padre
    //Si habíamos creado el objeto MediaPlayer...liberamos su memoria para evitar memory leaks
    if(fileExists) mp.release()
  }
}