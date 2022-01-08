/*
  Tema: ScrollView
  Autor: Ramón Junquera
  Fecha: 20210609
  Descripción del objeto:
    El ScrollView es capaz de contener objetos más grandes que su propia vista.
    El scroll puede ser tanto horizontal como vertical.
    Por defecto, un ScrollView contiene un LinearLayout vertical, que después podemos adaptar.
  Funcionamiento:
    Este ejemplo no tiene código. Sólo es una demostración del funcionamiento de los objetos
    gráficos. Por eso ninguno de los objetos gráficos tiene id.
    Hemos incluido un ScrollView de un tamaño reducido (200dp x 200dp)
    En su LinearLayout vertical hemos incluido varias imágenes. El tamaño total del LinearLayout es
    muy superior al ScrollView.
    Aparece un barra de scroll vertical.
    El resultado es que podemos desplazar la columna de imágenes con el dedo.
*/

package com.rojo.app01

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.rojo.app01.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {
    private lateinit var ui: ActivityMainBinding
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        ui=ActivityMainBinding.inflate(layoutInflater)
        setContentView(ui.root)
    }
}