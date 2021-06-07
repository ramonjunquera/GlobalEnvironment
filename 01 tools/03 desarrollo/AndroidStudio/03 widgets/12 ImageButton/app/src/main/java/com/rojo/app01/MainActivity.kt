/*
  Tema: Gestión de ImageButton
  Autor: Ramón Junquera
  Fecha: 20210601
  Funcionamiento:
    Comenzamos con un ImageButton con la imágen de una bombilla apagada.
    Si lo pulsamos, cambiará la imágen del bótón.
  Notas.
  - El objeto Button y ImageButton, son distintos.
  - Antes de insertar el ImageButton, debemos haber preparado los archivos de imágen.
  - El nombre de los archivos debería estar escritos en minúsculas, sin espacios ni caracteres
    especiales. Esto es obligatorio porque los archivos parsean generando un identificador que
    coincide con el nombre. Por eso en nuestro ejemplo los archivos se llaman on y off.
  - Por defecto dejamos los archivos en la carpeta /app/src/res/mipmap-mdpi que corresponde con
    los recursos con menor resolución.
  - Los archivos aparecen automáticamente en el IDE bajo app/res/mipmap/
  - El resto de carpetas contienen recursos con mayor resolución para que las aplicaciones puedan
    ser ejecutadas en pantallas más grandes (tablets) sin que pierdan calidad.
    La resolución utilizada por cada una de las carpetas es:
    - res/mipmap-mdpi/ : tamaño original
    - res/mipmap-hdpi/ : 150% del tamaño original
    - res/mipmap-xhdpi/ : 200% del tamaño original
    - res/mipmap-xxhdpi/ : 300% del tamaño original
    - res/mipmap-xxxhdpi/ : 400% del tamaño original
  - Cuando insertamos un ImageButton se nos pide seleccionar la imágen a mostar
  - Por defecto se ajusta al tamaño de la imágen que contiene
*/

package com.rojo.app01

import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import com.rojo.app01.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {
    private lateinit var ui:ActivityMainBinding
    private var status=false //Estado de la bombilla
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        ui=ActivityMainBinding.inflate(layoutInflater)
        setContentView(ui.root)

        ui.imageButton.setOnClickListener { //Se ha pulsado el botón...
            //Dependiendo del estado, seleccionamos el identificador de una imágen u otra
            //Asignamos el identificador de la imágen a la imágen del botón
            ui.imageButton.setImageResource(if(status) R.mipmap.off else R.mipmap.on)
            status=!status //Cambiamos el estado para la próxima pulsación
        }
    }
}