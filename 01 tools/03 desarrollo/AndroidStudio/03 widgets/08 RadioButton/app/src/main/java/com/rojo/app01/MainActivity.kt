/*
  Tema: Gestión de Radio Button
  Autor: Ramón Junquera
  Fecha: 20210530
  Funcionamiento:
    Tenemos 2 EditText que sólo admiten números. Un grupo de RadioButton con 2 RadioButton
    etiquetados como sumar y restar. Un botón etiquetado como Calcular y un TextView con el texto
    resultado.
    El usuario asigna los valores a los EditText, selecciona el tipo de operación con el
    RadioButtonGroup, pulsa el botón y obtiene el resultado.
*/

package com.rojo.app01

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.rojo.app01.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {
  private lateinit var ui:ActivityMainBinding
  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    ui=ActivityMainBinding.inflate(layoutInflater)
    setContentView(ui.root)

    //Definimos Listener de pulsación de botón
    ui.button.setOnClickListener {
      val v1=ui.editTextNumber1.text.toString().toInt()
      val v2=ui.editTextNumber2.text.toString().toInt()
      val res = if(ui.radioButtonAdd.isChecked) v1+v2 else v1-v2
      ui.textViewRes.text=res.toString()
    }
  }
}