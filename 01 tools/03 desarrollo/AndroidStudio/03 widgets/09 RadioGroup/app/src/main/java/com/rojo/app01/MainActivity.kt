/*
  Tema: Gestión de RadioGroup
  Autor: Ramón Junquera
  Fecha: 20210607
  Funcionamiento:
    Tenemos 2 EditText que sólo admiten números. Un grupo de RadioButton con 3 RadioButton
    etiquetados como sumar, restar y multiplicar, y un TextView con el resultado.
    El usuario asigna los valores a los EditText, selecciona el tipo de operación con el
    RadioGroup y obtiene el resultado.
  Notas:
  - En el ejemplo de gestión de RadioButton se creaba un Listener que atendía la pulsación de un
    botón que posteriormente comprobaba el estado de un RadioButton para determinar la opción
    seleccionada. En el ejemplo actual crearemos un Listener para atender los cambios de selección
    del RadioGroup. Hasta que no se haga un cambio de opción en el RadioGroup, no se lanzará el
    cálculo.
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

    //Ha cambiado la selección del RadioGroup
    ui.radioGroup.setOnCheckedChangeListener { group, checkedId ->
      //Intentamos obtener los dos valores
      val v1=ui.editTextNumber1.text.toString().toInt()
      val v2=ui.editTextNumber2.text.toString().toInt()
      var res=0 //Resultado
      //Nececitamos conocer el índice de la opción seleccionada
      //Tenemos el id del RadioButton seleccionado en checkedId
      //Tenemos el RadioGroup en group
      //Localizamos la View del RadioButton seleccionado
      //Obtenemos el índice de la vista del RadioGroup
      //Ahora podemos decidir qué hacer en cada caso
      when(group.indexOfChild(group.findViewById(checkedId))) {
        0 -> { //Opción sumar
          res=v1+v2
        }
        1 -> { //Opción restar
          res=v1-v2
        }
        2 -> { //Opción multiplicar
          res=v1*v2
        }
      }
      ui.textViewRes.text=res.toString() //Mostramos resultado
    }

    //Ahora que tenemos Listener, activaremos manuelamente la primera opción y se llamará
    //automáticamente al Listener
    ui.radioButtonAdd.isChecked=true
  }
}