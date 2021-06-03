/*
  Tema: Aplicación con varias Activity
  Autor: Ramón Junquera
  Fecha: 20210603
  Objetivo:
    Crear una aplicación con más de una Activity
  Funcionamiento:
  - Tenemos una Activity principal llamada MainActivity que es la que se crea por defecto. Es la
    que se abre con la aplicación. Desde ella podemos abrir otras Activity a las que llamaremos
    secundarias.
  - Cuando una Activity finaliza se recupera la última Activity "viva".
  - Si cerramos la última Activity "viva" de la aplicación, finaliza la aplicación
  - El ejemplo tiene dos Activity secundarias que son llamadas desde la Activity principal mediante
    botones. Las Activity secundarias se tienen como nombre MainActivity1 y MainActivity2.
  Notas:
  - Para añadir una nueva Activity al proyecto hacemos click con el botón derecho sobre app (el
    elemento raíz del árbol de archivos) y seleccionamos New/Activity/Empty Activity. Además del
    archivo .kt, también se crea el archivo .xml con el interface gráfico.
  - Los identificadores de los objetos gráficos deben ser únicos en la aplicación. No se pueden
    repetir identificadores entre distintas Activity.
  - No se puede añadir una segunda Activity en modo offine. Necesita conexión para crear el .xml.
  - Una Activity secundaria no se llama MainActivity y su binding tampoco es ActivityMainBinding.
    El binding debe ser renombrado como corresponda, tanto en la importación de librerías como en
    el código.
  - Puesto que las Activity secundarias no llaman a ninguna otra, al cerrarse devolverán el control
    a la Activity principal.
  - Cuando se abre otra Activity no se detiene el flujo de ejecución. Simplemente se abre y se
    continúa al mismo tiempo que se inicializa y ejecuta la nueva Activity.
  - El lugar en el que se decide cuál de todas las Activity definidas para una aplicación es la
    que se abre al inicio es en el archivo AndroidManifest.xml. Podemos ver como todas las Activity
    están definidas, pero sólo una de ellas (la que llamamos Activity principal) contiene sección
    <intent-filter>
*/

package com.rojo.app01

import android.content.Intent
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import com.rojo.app01.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {
    private lateinit var ui: ActivityMainBinding
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        ui=ActivityMainBinding.inflate(layoutInflater)
        setContentView(ui.root)

        ui.button1.setOnClickListener { //Pulsado el botón de abrir Activity 1
            startActivity(Intent(this,MainActivity1::class.java))
        }

        ui.button2.setOnClickListener { //Pulsado el botón de abrir Activity 2
            startActivity(Intent(this,MainActivity2::class.java))
        }

        ui.buttonEnd0.setOnClickListener { //Pulsado el botón de cerrar aplicación
            finish() //Cierra la Activity
        }
    }
}