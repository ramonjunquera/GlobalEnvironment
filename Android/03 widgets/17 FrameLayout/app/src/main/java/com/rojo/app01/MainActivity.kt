/*
  Tema: FrameLoyout
  Autor: Ramón Junquera
  Fecha: 20210609
  Descripción del objeto:
    El Layout Frame dispone los objetos que contiene unos sobre otros.
    Cada uno de sus objetos podemos alinearlo a un extremo o al centro, en ambos sentidos, con la
    propiedad layout_gravity
    Alguno de sus usos son:
    - Layouts complejos con imágenes de fondo con texto sobreimpresionado
    - Varias disposiciones que podemos mostrar individialmente controlando si visibilidad
  Funcionamiento:
    Este ejemplo es la práctica del segundo de los usos.
    Tenemos una botonera superior con tre botones para controlar la visibilidad de las tres
    disposiciones que es capaz de mostrar el LayoutFrame.
    Las dos primeras opciones que muestra son objetos ImageView que ocupan todo el FrameLayout.
    La última es un ConstraintLayout que contiene un texto y un pequeño menú compuesto por dos
    ImageButton, que llevan a las mismas opciones que los dos primeros botones de la botonera
    superior.
  Notas:
  - Inicialmente, sólo tendremos visible la primera de las opciones. El primer ImageView.
  - Puesto que la principal tarea de la aplicación es cambiar entre distintas disposiciones del
    LayotFrame, creamos una función para ello para simplificar. Tiene un parámetro numérico con el
    índice de la disposición. La función desactiva todas las configuraciones y después activa sólo
    la opción que corresponda.
  - Cremos un Listener para el click de cada uno de los botones, que sólo hace la llamada a la
    función de cambio de opción.
  - Aunque la tercera opción es un Layout, cuando lo hacemos invisible, propaga la propiedad a
    todos sus componentes.
*/

package com.rojo.app01

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import androidx.core.view.isVisible
import com.rojo.app01.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {
    private lateinit var ui: ActivityMainBinding

    //Activar un Layout por índice
    private fun setLayout(index:Int) {
        //Primero hacemos invisibles todos los layouts
        ui.imageViewOff.isVisible=false
        ui.imageViewOn.isVisible=false
        ui.constraintLayoutMenu.isVisible=false
        when(index) {
            0-> { //Mostrar imageViewOff
                ui.imageViewOff.isVisible=true
            }
            1-> { //Mostrar imageViewOn
                ui.imageViewOn.isVisible=true
            }
            2 -> { //Mostrar constraintLayoutMenu
                ui.constraintLayoutMenu.isVisible=true
            }
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        ui=ActivityMainBinding.inflate(layoutInflater)
        setContentView(ui.root)

        ui.buttonOff.setOnClickListener { //Pulsado el botón Off
            setLayout(0)
        }

        ui.buttonOn.setOnClickListener { //Pulsado el botón On
            setLayout(1)
        }

        ui.buttonMenu.setOnClickListener { //Pulsado el botón Menú
            setLayout(2)
        }

        ui.imageButtonOff.setOnClickListener { //Pulsado el botón de imágen Off
            setLayout(0)
        }

        ui.imageButtonOn.setOnClickListener { //Pulsado el botón de imágen On
            setLayout(1)
        }
    }
}