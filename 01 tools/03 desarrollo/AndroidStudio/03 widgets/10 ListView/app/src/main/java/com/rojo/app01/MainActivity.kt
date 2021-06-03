/*
  Tema: Gestión de ListView
  Autor: Ramón Junquera
  Fecha: 20210601
  Funcionamiento:
    Tenemos un TextView en la parte superior en el que se mostrará la información
    Por debajo y ocupando todo el área de la Activity tenemos un ListView
    Cuando se selecciona un item del ListView se muestra la información que corresponde a ese item.
  Notas.
  - El objeto ListView se encuentra en la sección Legacy
  - ListView muestra un listado de opciones siempre visibles. Si hay tantas que no se pueden
    visualizar todas, aparecerá una barra de scroll.
  - Cuando se hece click sobre un item, no se queda seleccionada. Simplente saltará el Listener si
    es que ha sido configurado.
  - Al igual que con el Spinner, creamos un Array de Strings con las distintas opciones, que
    convertiremos en un adaptador. Es en tiempo de ejecución cuando se llenan las opciones de la
    lista.
  - La información de cada item se guarda en otro Array distinto, pero con el mismo número de
    elementos.
  - Debido al tamaño de pantalla y número de opciones, podemos ver todos los items a la vez. Si
    aumentamos el número de opciones, aparecería una barra de scroll.
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

        //El objeto gráfico ListView no tiene contenido
        //Crearemos un Array de Strings con las distintas opciones
        val planetsList = arrayOf("Mercurio","Venus","Tierra","Marte","Júpiter","Saturno","Urano","Neptuno")
        val diameterList= arrayOf(4878,12100,12756,6787,142984,120536,51108,49538)
        //Crearemos el objeto con la lista de opciones basado en el Array y específico para un ListView
        //El opjeto será un "Adapter"
        val listViewAdapter = ArrayAdapter(this,android.R.layout.simple_list_item_1,planetsList)
        //Asignamos el Adapter a nuestro ListView
        ui.listView.adapter=listViewAdapter

        //Creamos el Listener que gestiona la pulsación de un item del ListView
        //Tenemos como parámetros por defecto: parent, view, position y id
        //El formato correcto sería:
        //ui.listView.setOnItemClickListener { parent, view, position, id ->
        //Pero como tenemos parámetros que no usamos, los sustituimos por _
        ui.listView.setOnItemClickListener { _, _, position, _ ->
            //Componemos el texto para el TextView con el diámetro
            //Aprovechamos que uno de los parámetros es la posición del item seleccionado
            //que comienza desde 0
            //Ponemos la cadena entre paréntesis para forzar a que complete las operaciones de
            //concatenación previamente a entregar el resultado final.
            ui.textView.text=("Diametro ecuatorial: ${diameterList[position]}Km")
        }
    }
}