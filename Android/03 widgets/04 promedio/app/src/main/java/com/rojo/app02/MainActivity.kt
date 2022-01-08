/*
  Tema: Interactuar con Button, TextView y EditText
  Autor: Ramón Junquera
  Fecha: 20210510
  Funcionamiento:
    Calculadora de promedio de notas de 3 asignaturas
  A tener en cuenta:
  - Formateo de valores de coma flotante con cierto número de decimales
  - EditText que sólo admiten valores numéricos decimales
  - Obtención del texto de un EditText
  - Conversión de String a numérico
  Vista:
  - Se ha desactivado el EditText del total para que no se pueda editar
*/


package com.rojo.app02

import android.annotation.SuppressLint
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.text.Editable
import com.rojo.app02.databinding.ActivityMainBinding
import kotlin.math.roundToInt

class MainActivity : AppCompatActivity() {
  private lateinit var ui:ActivityMainBinding

  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    ui=ActivityMainBinding.inflate(layoutInflater)
    setContentView(ui.root)

    ui.buttonCalcular.setOnClickListener { //Pulsado el botón Calcular
      //Obtenemos el texto del EditText de Mates
      //El resultado no es un String sino un String editable
      //Lo convertimos a String
      //La función toFloatOrNull lo convierta a Float si puede. Sino devuelve null
      //Lo guardamos en una variable Float anulable (Float?)
      var mates:Float?=ui.editTextMates.text.toString().toFloatOrNull()
      //Si no se ha conseguido parsear el texto a Float, valdrá 0
      if(mates==null) mates=0f
      var fisica:Float?=ui.editTextFisica.text.toString().toFloatOrNull()
      if(fisica==null) fisica=0f
      var quimica:Float?=ui.editTextQuimica.text.toString().toFloatOrNull()
      if(quimica==null) quimica=0f
      //Calculamos la media de las 3 notas
      var total:Float=(mates+fisica+quimica)/3
      //Recalculamos el valor del total forzando a que sólo tenga un decimal
      total=(total*10).roundToInt()/10f
      //Componemos un String con un valor numérico de tipo Float formateado con 1 decimal
      //Asignamos el String al EditText del total
      ui.editTextTotal.setText("%.1f".format(total))
      //Decidimos si está aprobado
      ui.textViewNota.text=if(total>=5) "APROBADO" else "SUSPENDIDO"
      //Nota1
      //Cuando de crea un String con formato, hay que recordar que se utiliza como
      //separador decimal la coma, al menos en España
      //Si intentamos convertir este String de nuevo en FLoat con toFloat() dará siempre
      //un error, porque toma como separador decimal el punto.
    }
  }
}