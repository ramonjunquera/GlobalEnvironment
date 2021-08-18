/*
  Tema: Mostrar información interactiva en la reproducción de audio mp3
  Autor: Ramón Junquera
  Fecha: 20210815
  Funcionamiento:
    Sólo tenemos un botón para comenzar/pausar la reproducción de audio.
    Los elementos que se actualizan en tiempo real son:
    - Un contador con los segundos transcurridos
    - Un contador con los segundos pendientes
    - Una barra de progreso
    La barra de progreso es interactiva. Permite posicionar la reproducción de la canción en
    cualquier punto.
  Estructura:
  - Dentro de la carpeta app/src/main/res/raw/ se guarda el archivo mp3
  - Puesto que a los archivos mp3 se les asignará un identificador, sus nombres no deberían tener
    caracteres especiales (espacios, guiones, etc).
  - Las imágenes se guardan en app/src/main/res/mipmap-mdpi/
  Notas:
  - Se crea un único objeto MediaPlayer con el mp3, del que se guarda la referencia de la instancia
    para poder interactuar con él.
  - Mantenemos un Thread mientras la canción está en reproducción para que refresque los elementos
    gráficos.
  - Este Thread sólo existirá mientras la canción se esté reproduciendo, para no gastar recursos
    innecesarios.
  - Puesto que la barra de progreso se actualiza tanto por programa como por interacción del
    usuario, y ambas generan eventos en su Listener, necesitamos distinguirlo. Si no lo hacemos,
    los eventos generados por programa afectan a la reproducción de la canción, porque cada segundo
    se vuelve a reposicionar la reproducción del audio. Esto se solución con el parámetro fromUser
    del Listener, que nos indica el origen del evento.
*/

package com.rojo.app01

import android.media.MediaPlayer
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.widget.SeekBar
import com.rojo.app01.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {
  private lateinit var ui: ActivityMainBinding
  private lateinit var mp:MediaPlayer //Objeto que contendrá el audio
  private var duration:Int=0 //Duración de la canción en segundos

  private fun to00(v:Int):String { //Convierte valor a doble dígito
    var res=v.toString()
    if(res.length<2) res="0$res"
    return res
  }

  private fun toMMSS(s:Int):String { //Convierte segundos a MM:SS
    return to00(s/60)+":"+to00(s%60)
  }

  private fun refresh() { //Actualiza la barra y el tiempo
    Thread {
      var currentPosition: Int
      val mainHandler = Handler(Looper.getMainLooper())
      do {
        currentPosition = mp.currentPosition / 1000 //Posición actual en segundos
        mainHandler.post { //Rutina para enviar al Looper
          ui.seekBar.progress = currentPosition
          ui.textView1.text = toMMSS(currentPosition)
          ui.textView2.text = toMMSS(duration - currentPosition)
        }
        if (mp.isPlaying) Thread.sleep(1000)
      } while (mp.isPlaying)
    }.start()
  }

  //Activar/desactivar reproducción
  private fun play(status:Boolean) {
    if(status) { //Si se debe reproducir...
      mp.start() //Comenzamos la reproducción
      ui.play.setImageResource(R.mipmap.play) //Icono de play
      refresh()
    } else { //Si se debe pausar...
      mp.pause() //Pausamos la reproducción
      ui.play.setImageResource(R.mipmap.pause) //Icono de pause
    }
  }

  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    ui=ActivityMainBinding.inflate(layoutInflater)
    setContentView(ui.root)

    //Instanciamos el objeto MediaPlayer con el mp3 de los recursos
    mp=MediaPlayer.create(this,R.raw.supermariobros)
    duration=mp.duration/1000 //Anotamos cuanto dura la canción en segundos
    ui.seekBar.max=duration //Fijamos el máximo de la barra

    ui.play.setOnClickListener { //Se ha pulsado la imágen de play...
      play(!mp.isPlaying) //Reproducimos o paramos en función del estado actual
    }

    mp.setOnCompletionListener { //Si finaliza la canción...
      //Ya se ha pausado automáticamente
      mp.seekTo(0) //Volvemos al principio
      refresh() //Refrescamos objetos gráficos
    }

    //Definición del Lixtener de SeekBar
    ui.seekBar.setOnSeekBarChangeListener(object: SeekBar.OnSeekBarChangeListener {
      //Sobreescribimos los métodos de la clase original
      override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
        //Si el cambio de progreso está producido por la interacción del usuario...
        if(fromUser) mp.seekTo(progress*1000) //...cambiamos la posición de la canción
      }
      override fun onStartTrackingTouch(seekBar: SeekBar?) {} //Nada especial que hacer aquí
      override fun onStopTrackingTouch(seekBar: SeekBar?) {} //Nada especial que hacer aquí
    })
  }

  override fun onDestroy() { //La aplicación se va a cerrar
    super.onDestroy() //Llamamos al método de la clase padre
    mp.release() //Liberamos la memoria del objeto MediaPlayer para evitar memory leaks
  }
}