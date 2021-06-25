/*
  Tema: Control de la reproducción de archivo de audio mp3
  Autor: Ramón Junquera
  Fecha: 20210611
  Funcionamiento:
    Se muestra una botonera de imágenes para:
    - Volver al principio
    - Reproducir
    - Pausar
    - Activar/desactivar la reproducción continua
  Estructura:
  - Dentro de la carpeta app/src/main/res/raw/ se guarda el archivo mp3
  - Puesto que a los archivos mp3 se les asignará un identificador, sus nombres no deberían tener
    caracteres especiales (espacios, guiones, etc).
  - Las imágenes de los botones se guardan en app/src/main/res/mipmap-mdpi/
  Notas:
  - Se crea un único objeto MediaPlayer con el mp3, del que se guarda la referencia de la instancia
    para poder interactuar con él.
  - Definimos como variable el tiempo transcurrido en milisegundos desde el inicio de la
    reproducción. Lo utilizamos para comenzar la reproducción desde ese punto.
  - El botón de reproducción cíclica lo convertimops en Toggle por código. Guardamos su estado
    y al ser pulsado, cambiamos la imágen del botón y el estado, al mismo tiempo que informamos al
    objeto MediaPlayer.
  - Se crea un Listener para que atienda el click de cada botón.
*/

package com.rojo.app01

import android.media.MediaPlayer
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.rojo.app01.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {
  private lateinit var ui: ActivityMainBinding

  private lateinit var mp:MediaPlayer //Objeto que contendrá el audio
  private var looping=false //Se debe hacer loop?. Inicialmente no.

  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    ui=ActivityMainBinding.inflate(layoutInflater)
    setContentView(ui.root)

    //Instanciamos el objeto MediaPlayer con el mp3 de los recursos
    mp=MediaPlayer.create(this,R.raw.supermariobros)

    //Inicialmente la reproducción comienza desde el principio
    //El valor representa los milisegundos transcurridos desde el principio
    var playingTime=0

    ui.imageButtonReset.setOnClickListener { //Pulsado el botón de reinicio...
      playingTime=0
      if(mp.isPlaying) { //Si se está reproduciendo...
        mp.pause() //Pausamos
        mp.seekTo(0) //Volvemos al principio
        mp.start() //Reproducimos
      }
    }

    ui.imageButtonPlay.setOnClickListener { //Pulsado el botón de play...
      if(!mp.isPlaying) { //Si no se está reproduciendo...
        mp.seekTo(playingTime) //Nos movemos al último tiempo anotado
        mp.start() //Comenzamos a reproducir
      }
    }

    ui.imageButtonPause.setOnClickListener { //Pulsado el botón de pausa...
      if(mp.isPlaying) { //Si se está reproduciendo...
        playingTime=mp.currentPosition //Anotamos el tiempo que se lleva reproducido
        mp.pause() //Pausamos la reproducción
      }
    }

    ui.imageButtonLoop.setOnClickListener { //Pulsado el botón de loop...
      //Este botón lo consideraremos un ToggleButton con imágen
      //Si el loop está activado...aplicamos la imágen gris
      if(looping) ui.imageButtonLoop.setImageResource(R.mipmap.loopgrey)
      //Si el loop está desactivado...aplicamos la imágen en rojo
      else ui.imageButtonLoop.setImageResource(R.mipmap.loop)
      looping=!looping //Cambiamos el estado del botón
      mp.isLooping=looping //Informamos al objeto MediaPlayer si se reproduce en bucle
    }
  }

  override fun onDestroy() { //La aplicación se va a cerrar
    super.onDestroy() //Llamamos al método de la clase padre
    mp.release() //Liberamos la memoria del objeto MediaPlayer para evitar memory leaks
  }
}