package com.rojo.app02

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.rojo.app02.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {
  private lateinit var binding:ActivityMainBinding

  //Función que gestiona el click sobre myButton2
  private fun button2Click() {
    println("Click2")
  }

  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    binding= ActivityMainBinding.inflate(layoutInflater)
    setContentView(binding.root)

    //Creamos el código que se debe ejecutar cuando se haga click sobre myButton1
    binding.myButton1.setOnClickListener {
      println("Click1")
    }
    //También podemos llamar a una función externa para no tener que escribir la función aquí
    binding.myButton2.setOnClickListener{ button2Click() }
  }
}