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

        //Recuperamos los valores extras del Intent
        val name= intent.extras?.getString("name")
        val age= intent.extras?.getInt("age")
        //Los mostramos
        ui.textViewName1.text=name
        ui.textViewNumber1.text=age.toString()

        ui.buttonEnd.setOnClickListener { //Pulsado el botón de cerrar
            finish() //Cierra la Activity
        }
    }
}


