/*
  Tema: Gestión clásica de un SeekBar
  Autor: Ramón Junquera
  Fecha: 20210512
  Funcionamiento:
    Tenemos un SeekBar que al manipularlo muestra su valor en un TextView.
  Notas:
  - En este ejemplo se utiliza el sistema clásico propuesto en los ejemplos de Kotlin.
  - Tenemos que asignar al SeekBar una clase que gestione los eventos que genera el objeto gráfico.
    Para ello instanciamos en el mismo punto en el que hacemos la llamada un objeto heredado de
    la clase del Listener (SeekBar.OnSeekBarChangeListener) que además también definimos.
  - Sobreescribimos los métodos de la clase original con la funcionalidad que nos interesa.
  - Es obligatorio sobreescribir todos los métodos de la clase original.
  - El TextView sólo tiene las siguientes modificaciones:
    - Texto centrado
    - Tamaño de fuente a 20
*/

package com.rojo.app03

import android.os.Bundle
import android.widget.SeekBar
import androidx.appcompat.app.AppCompatActivity
import com.rojo.app03.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {
  private lateinit var ui:ActivityMainBinding
  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    ui=ActivityMainBinding.inflate(layoutInflater)
    setContentView(ui.root)

    //Indicamos cuál será el Listener que gestionará los eventos de nuestro SeekBar.
    //Instanciamos un objeto heredado de la clase SeekBar.OnSeekBarChangeListener
    //Definimos nuestra clase aquí mismo
    ui.seekBar.setOnSeekBarChangeListener(object:SeekBar.OnSeekBarChangeListener {
      //Sobreescribimos los métodos de la clase original
      override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
        ui.textView.text="$progress%" //Mostramos el texto en el TextView
      }
      override fun onStartTrackingTouch(seekBar: SeekBar?) {} //Nada especial que hacer aquí
      override fun onStopTrackingTouch(seekBar: SeekBar?) {} //Nada especial que hacer aquí
    })
  }
}






