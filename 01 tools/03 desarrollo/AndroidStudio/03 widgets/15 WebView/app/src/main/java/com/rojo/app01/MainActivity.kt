/*
  Tema: Widget WebView
  Autor: Ramón Junquera
  Fecha: 20210603
  Funcionamiento:
    Crearemos nuestro propio navegador web (con limitaciones).
    En la parte superior tenermos un EdiText que recogerá la URL.
    Se sigue un pequeño botón llamado GO para solicitar la conexión a la URL.
    Todo el área restante la ocupa un objeto WebView para mostrar el contenido de la URL.
  Requerimientos:
  - Es necesario indicar que la aplicación necesitará permisos para conectarte con el exterior.
    Esto se hace en el archivo app/manifest/AndroidManifest.xml
    Incluiremos al final del archivo, dentro de la sección manifest, y después de la sección
    application la siguiente línea:
      <uses-permission android:name="android.permission.INTERNET" />
*/

package com.rojo.app01

import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.webkit.WebViewClient
import com.rojo.app01.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {
    private lateinit var ui: ActivityMainBinding
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        ui=ActivityMainBinding.inflate(layoutInflater)
        setContentView(ui.root)

        //Indicamos que utilizaremos como cliente para mostrar el resultado el propio objeto
        //WebView. Si no lo indicamos explicitamente, por defecto se abrirá la aplicación Chrome
        //para mostrar cualquier llamada a una URL.
        ui.webView.webViewClient=WebViewClient()

        ui.buttonGo.setOnClickListener { //Pulsado el botón GO
            //Solicitamos la carga de una nueva URL.
            //Si no tiene prefijo de protocolo (http:// o https://), se tomará por defecto http://
            //El problema es que actualmente hay muy pocas porque no están pormitidas por Google.
            //Por eso, lo mejor es añadir manualmente siempre el prefijo https://
            ui.webView.loadUrl(ui.editTextUrl.text.toString())
        }
    }
}