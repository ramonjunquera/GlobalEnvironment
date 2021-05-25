package com.rojo.app02

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.rojo.app02.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {
  private lateinit var binding:ActivityMainBinding

  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    binding= ActivityMainBinding.inflate(layoutInflater)
    setContentView(binding.root)

    //Gestión de click del botón
    binding.button.setOnClickListener {
      binding.textView.text="Adiós" //Asignamos un nuevo texto al TextView
    }
  }
}