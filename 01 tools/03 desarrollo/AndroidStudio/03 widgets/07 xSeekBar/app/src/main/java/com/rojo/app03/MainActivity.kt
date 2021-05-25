/*
  Tema: Gestión de varias SeekBars que comparten Listener
  Autor: Ramón Junquera
  Fecha: 20210512
  Funcionamiento:
    Tenemos 3 SeekBar, una por cada canal de color. Cuando interactuamos con ellas, se aplica el
    color resultante al fondo de un objeto View.
  Notas:
  - Si seguimos la recomendación de Kotlin, tenemos que definir un Listener por cada SeekBar.
    En este ejemplo no lo haremos. Definiremos una clase heredada del Listener de SeekBar, crearemos
    una instancia y la compartiremos para todos los SeekBar.
    La razón es que en con tal de que cambie un sólo SeekBar, se tiene que recalcular el color y
    aplicarlo al fondo del objeto View.
  - Para que cada barra tenga su propio color, hemos cambiado las propiedades de
    - thumbTint : para el color de la bolita
    - progressTint : para col color de la barra por la parte izquierda
  - Puesto que los canales de color deben tener un valor de tipo Byte, se ha limitado el máximo de
    las barras a 255.
  - No se puede asignar un color directamente al fondo de un objeto View como un entero, porque la
    codificación no coincide. La fórmula lógica de color=r<<10 | g<<8 | b no es correcta.
    Los valores de los colores son todos negativos. Es mucho más cómodo utilizar los métodos de la
    clase Color.
  Otras posibilidades:
    Si la clase de la Activity (MainActivity), además de heredarla como siempre de
    AppCompatActivity(), también lo hace del Listener (SeekBar.OnSeekBarChangeListener), tendríamos
    sus métodos dentro de MainActivity y se podría comportar también como un Listener.
    Tendríamos que definir los métodos sobreescribiendo.
    Después a la hora de asignar Listener a cada SeekBar, incidaríamos que es this.
    Ventajas:
    - No necesitamos clases externas ni trucos para que puedan acceder los objetos gráficos.
    - La asignación de Listener a las SeekBar es sencilla. No necesitamos instanciar nada.
      Utilizamos la instancia actual.
    Inconvenientes:
    - Los métodos de ambas clases (Lístener y Activity) se mezclan en una misma clase.
      Podrían solaparse. Más sucio y confuso.
*/

package com.rojo.app03

import android.graphics.Color
import android.os.Bundle
import android.widget.SeekBar
import androidx.appcompat.app.AppCompatActivity
import com.rojo.app03.databinding.ActivityMainBinding

//Definimos una clase heredada del Listener de SeekBar
//Tiene un sólo parámetro que se guardará como constante privada con el Binding, que nos permitirá
//acceder a los objetos gráficos de la Activity
class MySeekBarListener(val ui:ActivityMainBinding):SeekBar.OnSeekBarChangeListener {
  override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
    //Localizamos la vista y asignamos un color para su fondo.
    //El color estará compuesto por los canales de color RGB.
    //Cada canal toma el valor del prograso de cada una de las barras
    ui.view.setBackgroundColor(Color.rgb(ui.seekBarR.progress,ui.seekBarG.progress,ui.seekBarB.progress))
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

    //Instanciamos un único Listener
    val myListener=MySeekBarListener(ui)
    //Aplicamos la instancia para que gestione los eventos de todos los SeekBar
    ui.seekBarR.setOnSeekBarChangeListener(myListener)
    ui.seekBarG.setOnSeekBarChangeListener(myListener)
    ui.seekBarB.setOnSeekBarChangeListener(myListener)
  }
}






