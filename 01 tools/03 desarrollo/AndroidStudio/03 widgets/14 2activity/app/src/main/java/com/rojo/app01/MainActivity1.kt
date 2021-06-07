/*
  Nota:
  Como Activity secundaria su nombre ya no es ActivityMain.
  Por lo tanto, el binding ya no es ActivityMainBinding.
  Se debe actualizar el nombre de del binding tanto en las librerías importadas como en las
  referencias del programa.
*/

package com.rojo.app01

import android.content.Intent
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import com.rojo.app01.databinding.ActivityMain1Binding //Ojo! Referencia a ActivityMain1Binding

class MainActivity1 : AppCompatActivity() {
    private lateinit var ui: ActivityMain1Binding //
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        //ui=ActivityMain1Binding.inflate(layoutInflater)
        ui=ActivityMain1Binding.inflate(layoutInflater)
        setContentView(ui.root)

        ui.buttonEnd1.setOnClickListener { //Pulsado el botón de cerrar Activity
            finish() //Cierra la Activity
        }
    }
}


