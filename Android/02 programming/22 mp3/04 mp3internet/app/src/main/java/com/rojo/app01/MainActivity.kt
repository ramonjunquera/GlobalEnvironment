/*
  Tema: Reproducción de archivos mp3 desde Internet
  Autor: Ramón Junquera
  Fecha: 20210614
  Componentes gráficos:
  - botón para reproducción y pausa.
  - texto con el estado actual de la reproducción
  Estructura:
  - La imágen del botón se guarda en app/src/main/res/mipmap-mdpi/
  Notas:
  - El gran problema de reproducir un archivo que no tenemos en local es su descarga. En el ejemplo
    actual, cada vez que reproducimos el audio, comenzamos desde cero, instanciando el objeto y
    descargándolo de Internet. Esto significa que desde la solicitud de reproduccion hasta su inicio
    necesitaremos añadir el tiempo de descarga.
  - Mientras se realiza la descarga el interface de usuario se congela. La razón es porque estamos
    llamando al método "prepare" desde el hilo principal. Si antes de la descarga cambiamos el texto
    de pantalla a "descargando", el usuario no podría ver cambio, porque no tiene el hilo principal
    no tiene tiempo para hacer la actualización.
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

        var playing=false //Inicialmente no se está reproduciendo

        ui.imageButtonPlay.setOnClickListener { //Se ha pulsado el botón
            if(!playing) { //Si no se está reproduciendo...
                mp=MediaPlayer() //Instanciamos un nuevo objeto MediaPlayer
                try {
                    mp.setDataSource(url) //Indicamos el origen del archivo
                    mp.prepare() //Lo descargamos de Internet
                    mp.start() //Comenzamos su reproducción
                    ui.textViewInfo.text="reproduciendo"
                    playing=true //Ya se está reproduciendo
                    mp.setOnCompletionListener { //Se ha terminado la reproducción
                        mp.release() //Borramos la instancia
                        playing=false //Ya no se está reproduciendo
                        ui.textViewInfo.text="parado"
                    }
                } catch (e:IOException) { //No se ha encontrado el archivo
                    ui.textViewInfo.text="sin acceso al archivo"
                    mp.release() //Borramos la instancia
                }
            } else { //Si se está reproduciendo...
                mp.stop() //Detenemos la reproducción
                mp.release() //Borramos la instancia
                playing=false //Ya no se está reproduciendo
                ui.textViewInfo.text="parado"
            }
        }
    }
}