/*
  Tema: Creación y gestión de objeto gráfico
  Autor: Ramón Junquera
  Fecha: 20210523
  Controles:
  - 3 SeekBar, una por canal de color, que nos permiten seleccionar el color.
  - Botón con el color seleccionado
  - Objeto View8
  Funcionamiento:
  - Las 3 SeekBar permiten componer el color asignado al botón.
  - Cuando se pulsa en el objeto View8 se pueden pintar los círculos con el color seleccionado.
  Notas:
  - Creamos una clase heredada del Listener de SeekBar que permite cambiar el color del botón.
  - Esta clase la instanciaremos y la aplicaremos como Listener a todas las SeekBar, porque todas
    tienen el mismo funcionamiento. Cuando hay una variación en cualquiera de ellas, se debe
    recalcular el color y aplicar.
  - Pâra que el Listener pueda acceder a los objetos gráficos, le añadimos el parámetro de Binding.
    (Se podría entregar solamente el objeto View8, porque no necestia ningún otro).
  - Ante cualquier cambio de color, el Listener calcula el nuevo color y notifica a la clase View8
    para que guarde el unevo color seleccionado y actualice el color del botón.
  - El objeto View8 es autogestionado. Aparte de declararlo en la sección gráfica no necesita
    código.
*/

package com.rojo.app01

import android.graphics.Color
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.widget.SeekBar
import com.rojo.app01.databinding.ActivityMainBinding


//Definimos una clase heredada del Listener de SeekBar
//Tiene un parámetro con el Binding que nos permite el acceso a los objetos gráficos
//(SeekBar, Button y View8).
class MySeekBarListener(private val ui:ActivityMainBinding): SeekBar.OnSeekBarChangeListener {
    override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
        //Obtenemos los valores de los canales
        val r=ui.seekBarR.progress
        val g=ui.seekBarG.progress
        val b=ui.seekBarB.progress
        //Componemos el color seleccionado y notificamos al objeto View8
        ui.view.setSelectedColor(Color.rgb(r,g,b))
    }
    //El resto de métodos no nos utilizaremos, pero su declaración es obligatoria
    override fun onStartTrackingTouch(seekBar: SeekBar?) {}
    override fun onStopTrackingTouch(seekBar: SeekBar?) {}
}

//Gestión principal de Activity
class MainActivity : AppCompatActivity() {
    private lateinit var ui: ActivityMainBinding
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        ui=ActivityMainBinding.inflate(layoutInflater)
        setContentView(ui.root)

        //Instanciamos un único Listener para todas las SeekBar
        val mySeekBarListener=MySeekBarListener(ui)
        //Lo utilizamos como gestor de eventos de todas las SeekBar
        ui.seekBarR.setOnSeekBarChangeListener(mySeekBarListener)
        ui.seekBarG.setOnSeekBarChangeListener(mySeekBarListener)
        ui.seekBarB.setOnSeekBarChangeListener(mySeekBarListener)

        ui.view.setColorButton(ui.buttonColor) //Informamos a View8 del botón de selección de color
    }
}
