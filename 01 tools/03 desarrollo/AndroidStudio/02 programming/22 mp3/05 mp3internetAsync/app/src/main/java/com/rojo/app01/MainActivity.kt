/*
  Tema: Reproducción de archivos mp3 desde Internet con descarga asíncrona
  Autor: Ramón Junquera
  Fecha: 20210614
  Componentes gráficos:
  - botón para reproducción y pausa.
  - texto con el estado actual de la reproducción
  Estructura:
  - La imágen del botón se guarda en app/src/main/res/mipmap-mdpi/
  Notas:
  - La novedad de este ejemplo es que la descarga del archivo desde Internet se realiza de manera
    asíncrona. Realmente la descarga se lanza en un proceso de fondo. El hilo principal queda
    liberado y no se bloquea interface de usuario.
  - Puesto que lo único que hacemos es lanzar la descarga, necesitaremos haber creado previamente
    las funciones/listener que atenderán la finalización de la descarga.
  - Desde el ejemplo anterior, cambiamos la variable playing:Boolean a playStatus:Int, puesto que
    ahora tenemos 3 estados y no dos.
  - Si se detecta una pulsación del botón mientras está descargando, no podemos hacer mucho más.
    No tiene acciones especiales.
  - Puesto que el proceso es asíncrono, la etiqueta del estado va cambiando y muestra siempre el
    estado actual.
  Requerimientos:
  - Es necesario indicar que la aplicación necesitará permisos para conectarte con el exterior.
    Esto se hace en el archivo app/manifest/AndroidManifest.xml
    Incluiremos al final del archivo, bajo la sección manifest, y después de la sección application
    la siguiente línea:
      <uses-permission android:name="android.permission.INTERNET" />
*/

package com.rojo.app01

import android.media.MediaPlayer
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.rojo.app01.databinding.ActivityMainBinding
import java.io.IOException

class MainActivity : AppCompatActivity() {
    private lateinit var ui: ActivityMainBinding

    private lateinit var mp:MediaPlayer

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        ui=ActivityMainBinding.inflate(layoutInflater)
        setContentView(ui.root)

        //Constante con la URL del archivo a reproducir
        val url="https://vgmsite.com/soundtracks/super-mario-bros/gwsxwxwl/02%20-%20Invincibility%20Star.mp3"

        //Estado actual de reproducción. Valores:
        //- 0 : Parado
        //- 1 : Descargando
        //- 2 : Reproduciendo
        var playStatus=0 //Inicialmente está parado

        ui.imageButtonPlay.setOnClickListener { //Se ha pulsado el botón
            when(playStatus) { //Dependiendo del estado de reproducción...
                0 -> { //Si está parado...
                    mp=MediaPlayer() //Instanciamos un nuevo objeto MediaPlayer
                    //Creamos el Listener que se llama cuando finaliza la descarga del archivo
                    mp.setOnPreparedListener {
                        it.start() //Comenzamos la reproducción
                        playStatus=2 //Ya se está reproduciendo
                        ui.textViewInfo.text="reproduciendo"
                    }
                    //Creamos el Listener que se llama cuando finaliza la reproducción
                    mp.setOnCompletionListener {
                        mp.release() //Borramos la instancia
                        playStatus=0 //Ya no se está reproduciendo
                        ui.textViewInfo.text="parado"
                    }
                    //Utilizamos un grupo try/catch por si no se encuentra el archivo
                    try {
                        mp.setDataSource(url) //Indicamos el origen del archivo
                        mp.prepareAsync() //Lo descargaremos de Internet de manera asíncrona
                        playStatus=1 //Descargando
                        ui.textViewInfo.text="descargando"
                    } catch (e:IOException) { //No se ha encontrado el archivo
                        ui.textViewInfo.text="sin acceso al archivo"
                        mp.release() //Borramos la instancia
                    }
                }
                1 -> { //Si está descargando...
                    //Se ha pulsado el botón de play, la descarga ha comenzado pero aun no ha
                    //terminado.
                    //En este caso no hacemos nada. Tendremos que esperar a que termine la descarga.
                }
                2-> { //Si está reproduciendo...
                    mp.stop() //Detenemos la reproducción
                    mp.release() //Borramos la instancia
                    playStatus=0 //Ya no se está reproduciendo
                    ui.textViewInfo.text="parado"
                }
            }
        }
    }
}