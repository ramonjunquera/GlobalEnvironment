/*
  Tema: Gestión de un SeekBar definiendo nuestra clase Listener
  Autor: Ramón Junquera
  Fecha: 20210512
  Funcionamiento:
    Tenemos un SeekBar que al manipularlo muestra su valor en un TextView.
  Notas:
  - Aquí se presenta una solución más límpia que la clásica de los ejemplos de Kotlin
  - En vez de redefinir la clase del Listener e instanciarla en el mismo punto, lo separamos.
    Primero se declara la clase, heredada del Listener y después se asigna una instancia de ésta.
  - La clase heredada del Listener debe sobreescribir todos los métodos de la clase original.
  - Gracias al parámetro de la clase, podemos tener acceso a todos los objetos gráficos de nuestra
    Activity.
  - El TextView sólo tiene las siguientes modificaciones:
    - Texto centrado
    - Tamaño de fuente a 20
*/

package com.rojo.app03

import android.os.Bundle
import android.widget.SeekBar
import androidx.appcompat.app.AppCompatActivity
import com.rojo.app03.databinding.ActivityMainBinding

//Definimos una clase heredada del Listener de SeekBar
//Tiene un sólo parámetro que se guardará como constante privada con el Binding, que nos permitirá
//acceder a los objetos gráficos de la Activity
class MySeekBarListener(val ui:ActivityMainBinding):SeekBar.OnSeekBarChangeListener {
  //Sobreescribimos todos los métiodos de la clase original
  override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
    ui.textView.text="$progress%" //Mostramos el texto en el TextView
  }
  override fun onStartTrackingTouch(seekBar: SeekBar?) {} //Nada especial que hacer aquí
  override fun onStopTrackingTouch(seekBar: SeekBar?) {} //Nada especial que hacer aquí
}

class MainActivity : AppCompatActivity() {
  private lateinit var ui:ActivityMainBinding
  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    ui=ActivityMainBinding.inflate(layoutInflater)
    setContentView(ui.root)

    //Asignamos una instancia de la clase Listener que gestionará los eventos de nuestro SeekBar
    ui.seekBar.setOnSeekBarChangeListener(MySeekBarListener(ui))
  }
}






