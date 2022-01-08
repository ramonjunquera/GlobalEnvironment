/*
  Tema: Pasar parámetros a otras Activity
  Autor: Ramón Junquera
  Fecha: 20210603
  Funcionamiento:
  - Tenemos una aplicación con dos Activity. Una principal y una secundaria.
  - La Activity principal recoge algunos datos del usuario y los incluye en la llamada de
    apertura de la Activity secundaria.
  - La Activity secundario recupera los parámetros y los muestra
  Notas:
  - Se han eliminado los identificadores de los objetos gráficos a los que no se accede desde
    programa.
*/

package com.rojo.app01

import android.content.Intent
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import com.rojo.app01.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {
    private lateinit var ui: ActivityMainBinding
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        ui=ActivityMainBinding.inflate(layoutInflater)
        setContentView(ui.root)

        ui.buttonActivity1.setOnClickListener { //Pulsado el botón de abrir Activity 1
            //Instanciamos el Intent de la Activity
            val intentActivity1=Intent(this,MainActivity1::class.java)
            //Añadimos el valor extra "name" como String
            intentActivity1.putExtra("name",ui.editTextName0.text.toString())
            //Añadimos el valor extra "age" como Int
            intentActivity1.putExtra("age",ui.editTextNumber0.text.toString().toInt())
            startActivity(intentActivity1) //Abrimos la Activity1
        }
    }
}