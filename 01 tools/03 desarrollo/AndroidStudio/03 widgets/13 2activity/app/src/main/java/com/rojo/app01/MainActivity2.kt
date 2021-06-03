package com.rojo.app01

import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import com.rojo.app01.databinding.ActivityMain2Binding

class MainActivity2 : AppCompatActivity() {
    private lateinit var ui: ActivityMain2Binding
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        //ui=ActivityMain1Binding.inflate(layoutInflater)
        ui= ActivityMain2Binding.inflate(layoutInflater)
        setContentView(ui.root)

        ui.buttonEnd2.setOnClickListener { //Pulsado el botón de cerrar Activity
            finish() //Cierra la Activity
        }
    }
}