package com.rojo.app02

import android.annotation.SuppressLint
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.rojo.app02.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {
  private lateinit var binding:ActivityMainBinding

  @SuppressLint("SetTextI18n")
  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    binding=ActivityMainBinding.inflate(layoutInflater)
    setContentView(binding.root)

    binding.buttonSaludar.setOnClickListener { //Gestión de click del botón
      //Creamos un String que incluye el texto de EditText
      binding.textViewSaludo.text="Hola ${binding.editTextNombre.text}!"
    }
  }
}