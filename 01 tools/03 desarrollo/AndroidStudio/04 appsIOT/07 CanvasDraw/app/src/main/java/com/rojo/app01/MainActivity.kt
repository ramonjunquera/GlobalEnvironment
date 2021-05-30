/*
  Tema: Dibujo en Canvas
  Autor: Ramón Junquera
  Fecha: 20210526
  Controles:
  - 3 SeekBar, una por canal de color, que nos permiten seleccionar el color.
  - View con el color seleccionado
  - Objeto ViewDrawable que contiene el dibujo
  Funcionamiento:
  - Las 3 SeekBar permiten componer el color asignado al botón.
  - Se puede dibujar en el panel inferior con el color seleccionado
  Notas:
  - Dentro de la clase de la Activity, creamos una clase heredada del Listener de SeekBar que
    permite cambiar el color seleccionado.
  - Esta clase la instanciaremos y la aplicaremos como Listener a todas las SeekBar, porque todas
    tienen el mismo funcionamiento. Cuando hay una variación en cualquiera de ellas, se debe
    recalcular el color y aplicar.
  - Ante cualquier cambio de color, el Listener calcula el nuevo color, lo aplica al View del color
    seleccionado y notifica a la clase ViewDrawable para que lo guarde como nuevo color seleccionado
  - El objeto ViewDrawable es autogestionado (pulsaciones, Canvas, ...), por eso no necesitamos
    código específico en la Activity
*/

package com.rojo.app01

import android.graphics.Color
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.widget.SeekBar
import com.rojo.app01.databinding.ActivityMainBinding

//Gestión principal de Activity
class MainActivity : AppCompatActivity() {
    //Variable global de clase para acceso a objetos gráficos
    private lateinit var ui: ActivityMainBinding

    //Creamos clase heredada del Listener de SeekBar
    //Puede acceder a los objetos gráficos a través de la variable global ui
    inner class MySeekBarListener(): SeekBar.OnSeekBarChangeListener {
        override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
            //Obtenemos los valores de los canales y componemos el color
            val color=Color.rgb(ui.seekBarR.progress,ui.seekBarG.progress,ui.seekBarB.progress)
            //Asignamos el color al objeto gráfico
            ui.viewColor.setBackgroundColor(color)
            //Informamos a ViewDrawable del nuevo color seleccionado
            ui.view.setInkColor(color)
        }
        //El resto de métodos no los utilizaremos, pero su declaración es obligatoria
        override fun onStartTrackingTouch(seekBar: SeekBar?) {}
        override fun onStopTrackingTouch(seekBar: SeekBar?) {}
    }

    //Inicialización de la Activity
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        ui=ActivityMainBinding.inflate(layoutInflater)
        setContentView(ui.root)

        //Instanciamos un único Listener para todas las SeekBar
        val mySeekBarListener=MySeekBarListener()
        ui.seekBarR.setOnSeekBarChangeListener(mySeekBarListener)
        ui.seekBarG.setOnSeekBarChangeListener(mySeekBarListener)
        ui.seekBarB.setOnSeekBarChangeListener(mySeekBarListener)
    }
}
