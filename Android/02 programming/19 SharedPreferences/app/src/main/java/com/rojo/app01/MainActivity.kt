/*
  Tema: Almacenamiento permanente con SharedPreferences
  Autor: Ramón Junquera
  Fecha: 20210606
  - Objeto EditText
  - Botón guardar
  Funcionamiento:
  - Podemos escribir un texto en el EditText. Tras pulsar el botón guardar se almacena de manera
    permanente en las preferencias de la aplicación.
  - Al abrir la aplicación, recupera el valor almacenado y lo deja como texto por defecto en el
    EditText
  Notas:
  - Android permite almacenar información permanente por distintos métodos:
    - En las preferencias de la aplicación
    - En archivos
    - En una base de datos local
  - Las preferencias de la aplicación permiten almacenar algunos datos. No es el lugar adecuado para
    almacenamiento masivo. Habitualmente utilizado para configuración de la aplicación.
  - Las preferencias se guardan en grupos a los que podemos etiquetar.
  - Un grupo de preferencias guardará cada elemento/dato con una etiqueta propia
  - Un grupo de preferencias necesita un editor para poder cambiar su contenido.
*/

package com.rojo.app01

import android.content.Context
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.rojo.app01.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {
  private lateinit var ui: ActivityMainBinding
  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    ui=ActivityMainBinding.inflate(layoutInflater)
    setContentView(ui.root)

    //Recuperamos el grupo de preferencias etiquetado como myPreferences
    val preferencesGroup=getSharedPreferences("myPreferences", Context.MODE_PRIVATE)
    //Recuperamos el elemento txt, que suponemos que es de tipo String
    //Si no existiese, queremos que nos devuelva como valor por defecto un String vacío
    //Asignamos el resultado al EditText
    ui.editText.setText(preferencesGroup.getString("txt",""))

    ui.buttonSave.setOnClickListener { //Se ha pulsado el botón de guardar
      //Aprovechamos que tenemos instanciado el grupo de preferencias y creamos un editor para poder
      //modificar su contenido
      val preferencesEditor=preferencesGroup.edit()
      //Recuperamos el texto del EditText
      //Lo añadimos (o sobreescribimos) a al elemento txt de nuestro grupo de preferencias mediante
      //el editor
      preferencesEditor.putString("txt",ui.editText.text.toString())
      //Guardamos las modificaciones que mantiene el editor de preferencias
      //El método .apply guarda los cambios en background
      preferencesEditor.apply()
      //Nota: Si justo a continuación de guardar los cambios finalizamos la aplicación con finish
      //deberíamos utilizar el método .commit que guarda los cambios inmediatamente.
    }
  }
}