/*
  Tema: Reproducción de archivo de audio mp3
  Autor: Ramón Junquera
  Fecha: 20210610
  Funcionamiento:
    Se muestra una botonera con distintas etiquetas. Al pulsar un botón se reproduce un sonido
    procedente de un archivo mp3
  Estructura:
  - Dentro de la carpeta app/src/main/res/raw se guardan los archivos mp3
  - Puesto que a los archivos mp3 se les asignará un identificador, sus nombres no deberían tener
    caracteres especiales (espacios, guiones, etc).
  Notas:
  - Cuando se solicita la reproducción de un objeto MediaPlayer, se lanza en segundo plano. No
    se detiene el flujo del programa principal.
  - Cuando un objeto MediaPlayer finaliza su reproducción, no se destruye automáticamente, sino que
    se queda a la espera de una nueva instrucción, como: reproduce de nuevo desde un determinado
    momento..
  - Se crea un Listener para que atienda el click de cada botón. Los Listener de todos los botones
    tienen la misma estructura.
  - Si cuando se pulsa un botón, simplemente instanciamos un objeto MediaPlayer y comenzamos su
    reproducción, estaremos dejando la instancia en memoria. OCupando espacio y recursos. Si
    repetimos esto muchas veces, conseguiremos que el sistema nos devuelva un mensaje de error
    indicando que no se pueden crear más instancias de ese objeto.
    Por esta razón no preocupamos de crear un Listener específico para cada instancia que sea
    llamado al finaliozar la reproducción del audio y que se encargue de borrar el objeto de memoria
  - La pulsación y reproducción de un audio no afecta para nada a cualquier otro audio que se
    encuentre en reproducción.
*/

package com.rojo.app01

import android.media.MediaPlayer
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.rojo.app01.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {
  private lateinit var ui: ActivityMainBinding
  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    ui=ActivityMainBinding.inflate(layoutInflater)
    setContentView(ui.root)

    ui.buttonCoin.setOnClickListener { //Se ha pulsado el botón de Coin
      //Instanciamos un objeto MediaPlayer en el contexto actual y con el archivo que corresponda
      val mp=MediaPlayer.create(this,R.raw.coin)
      //Iniciamos la reproducción del archivo
      mp.start()
      //Creamos un Listener para cuando finalice la reproducción del audio
      mp.setOnCompletionListener {
        //Nos preocupamos de liberar el objeto MediaPlayer
        mp.release()
      }
    }

    ui.buttonHmm.setOnClickListener {
      val mp=MediaPlayer.create(this,R.raw.hmm)
      mp.start()
      mp.setOnCompletionListener {
        mp.release()
      }
    }

    ui.buttonLife.setOnClickListener {
      val mp=MediaPlayer.create(this,R.raw.life)
      mp.start()
      mp.setOnCompletionListener {
        mp.release()
      }
    }

    ui.buttonMammamia.setOnClickListener {
      val mp=MediaPlayer.create(this,R.raw.mammamia)
      mp.start()
      mp.setOnCompletionListener {
        mp.release()
      }
    }

    ui.buttonPipeline.setOnClickListener {
      val mp=MediaPlayer.create(this,R.raw.pipeline)
      mp.start()
      mp.setOnCompletionListener {
        mp.release()
      }
    }

    ui.buttonWoohoo.setOnClickListener {
      val mp=MediaPlayer.create(this,R.raw.woohoo)
      mp.start()
      mp.setOnCompletionListener {
        mp.release()
      }
    }
  }
}