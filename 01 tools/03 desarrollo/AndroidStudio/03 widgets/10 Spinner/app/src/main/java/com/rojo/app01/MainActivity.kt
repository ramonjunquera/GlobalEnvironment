/*
  Tema: Gestión de Spinner
  Autor: Ramón Junquera
  Fecha: 20210531
  Funcionamiento:
    Tenemos 2 EditText que sólo admiten números. Un Spinner con varias operaciones. Un botón
    etiquetado como Calcular y un TextView con el texto
    El usuario asigna los valores a los EditText, selecciona el tipo de operación con el Spinner,
    pulsa el botón y obtiene el resultado.
  Notas.
  - El Spinner corresponde a una lista desplegable. Al hacer click sobre el objeto, se despliega
    permite desplazarse por la lista de opciones y al seleccionar una de ella se vuelve a recoger.
  - En este ejemplo el objeto gráfico Spinner lo dejamos vacío. No hacemos referencia a las opciones
    que pueda contener. Es después en el momento de la creación de la Activity cuando creamos un
    Array de String con las opciones, lo transformamos en un Adapter para Spinner y lo asignamos.
  - Identificamos la opción seleccionada del Spinner con la palabra completa. Podríamos haber
    utilizado el identificador de la opción (ui.spinner.selectedItemId) que es un Long que comienza
    en 0. Al igual que la posición de la opción seleccionada (ui.spinner.selectedItemPosition).
  - No ponemos ningún tipo de control para recoger los dos valores de entrada. Si intentamos hacer
    un cálculo sin fijar lo valores, dará un error.
*/

package com.rojo.app01

import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.widget.ArrayAdapter
import com.rojo.app01.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {
    private lateinit var ui:ActivityMainBinding
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        ui=ActivityMainBinding.inflate(layoutInflater)
        setContentView(ui.root)

        //El objeto gráfico Spinner no tiene contenido
        //Crearemos un Array de Strings con las distintas opciones
        val operationList = arrayOf("sumar","restar","multiplicar",)
        //Crearemos el objeto con la lista de opciones basado en el Array y específico para un Spinner
        //El opjeto será un "Adapter"
        val spinnerAdapter = ArrayAdapter(this,android.R.layout.simple_spinner_item,operationList)
        //Asignamos el Adapter a nuestro Spinner
        ui.spinner.adapter=spinnerAdapter

        //Creamos el Listener que gestiona la pulsación del botón
        ui.button.setOnClickListener {
            //Obtenemos los valores del los EditText
            val v1=ui.editTextNumber1.text.toString().toInt()
            val v2=ui.editTextNumber2.text.toString().toInt()
            var res=0 //Variable para guardar el resultado
            when(ui.spinner.selectedItem.toString()) { //Dependiendo de la opción seleccionada...
                "sumar" -> res=v1+v2
                "restar" -> res=v1-v2
                "multiplicar" -> res=v1*v2
            }
            ui.textViewRes.text=res.toString()
        }
    }
}