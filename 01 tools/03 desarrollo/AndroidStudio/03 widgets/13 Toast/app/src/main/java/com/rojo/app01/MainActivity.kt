/*
  Tema: Ejemplo de uso de Toast
  Autor: Ramón Junquera
  Fecha: 20210602
  Descripción:
    Es un mensaje corto que sobreimprime en pantalla durante unos segundos.
  Funcionamiento:
    Es el juego de mayor-menor.
    El programa peinsa un número del 1 al 1000. El usuario debe averiguarlo.
    El programa responderá si el número introducido es mayor o menor que el pensado.
    Cuando se descubre el número se indica el número de intentos.
  Notas.
  - Puesto que el objeto Toast tiene una sintaxis bastante larga, se crea una función sólo para su
    llamada.
  - Todos los mensajes haca el usuario son de tipo Toast
*/

package com.rojo.app01

import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.widget.Toast
import com.rojo.app01.databinding.ActivityMainBinding
import kotlin.random.Random

class MainActivity : AppCompatActivity() {
    private lateinit var ui:ActivityMainBinding

    private var hideNumber=0 //Número a adivinar
    private var tryCounter=0 //Contador de intentos

    private fun reset() { //Inicialización
        tryCounter=0 //Reseteamos el contador de intentos
        hideNumber= Random.nextInt(1,1000) //Pensamos un nuevo número
        ui.editTextNumber.setText("") //Borramos texto de intento previo
        ui.textViewTry.text = "intento 0"
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        ui=ActivityMainBinding.inflate(layoutInflater)
        setContentView(ui.root)

        ui.buttonReset.setOnClickListener { //Si se ha pulsado el botón de reset...
            reset()
        }

        fun show(txt:String) { //Muestra mensaje con Toast
            Toast.makeText(this,txt,Toast.LENGTH_LONG).show()
        }

        ui.buttonTry.setOnClickListener { //Si se ha pulsado el botón de probar...
            val tryValue=ui.editTextNumber.text.toString().toIntOrNull() //Intentamos parsear el número
            if(tryValue==null) show("Valor incorrecto!")
            else {
                if(tryValue==hideNumber) {
                    show("Correcto en $tryCounter intentos")
                    reset()
                } else {
                    if(tryValue<hideNumber) show("Mayor")
                    else show("Menor")
                    tryCounter++
                    ui.textViewTry.text = ("intento $tryCounter")
                    ui.editTextNumber.setText("")
                }
            }
        }

        reset() //Inicialización
    }
}