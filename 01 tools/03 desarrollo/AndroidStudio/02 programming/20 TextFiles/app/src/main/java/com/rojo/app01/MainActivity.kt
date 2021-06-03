/*
  Tema: Almacenamiento permanente con archivos de texto
  Autor: Ramón Junquera
  Fecha: 20210603
  - Objeto EditText multilínea
  - Botón guardar
  Funcionamiento:
  - Podemos escribir un texto en el EditText. Tras pulsar el botón guardar se almacena de manera
    permanente en un archivo de texto.
  - Al abrir la aplicación, recupera el valor almacenado y lo deja como texto por defecto en el
    EditText
  Notas:
  - Android permite almacenar información permanente por tres métodos distintos:
    - En las preferencias de la aplicación
    - En archivos de texto
    - En una base de datos local
  - Los archivos de texto permiten guardar gran cantidad de información, pero siempre de manera
    estática. El total de la información se guarda de una sola vez y se recupera también de una
    vez. Esta limitación viene dada por los archivos de texto.
  - El objeto EditText es multi línea. Se han cambiado sus propiedades en inputType/textMultiLine
  - El objeto EditText por defecto centra verticalmente el texto. Lo podemos cambiar en gravity/top
  - Definimos el nombre del archivo como constante privada de la clase de la Activity
  - Si no se especifica lo contrario, los archivos se guardarán en la carpeta por defecto de la
    aplicación. Siempre podemos obtenerla con println(filesDir). Para el ejemplo actual será:
    /data/user/0/com.rojo.app01/files. El volumen ocupado quedará reflejado en el apartado de datos
    del usuario cuando solicitamos información de la aplicación. Al elimnar la aplicación, también
    se borrarán estos archivos. Para poder acceder a estas ubicaciones debes tener permisos de root
    (teléfono rooteado).
*/

package com.rojo.app01

import android.app.Activity
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.Toast
import com.rojo.app01.databinding.ActivityMainBinding
import java.io.BufferedReader
import java.io.IOException
import java.io.InputStreamReader

class MainActivity : AppCompatActivity() {
  private lateinit var ui: ActivityMainBinding
  private val filename="borrame.txt" //Constante con el nombre del archivo utilizado
  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    ui=ActivityMainBinding.inflate(layoutInflater)
    setContentView(ui.root)

    //Si ya existe el archivo (si aparece en el listado de archivos)...
    if (fileList().contains(filename)) {
      //Para evitar excepciones en la apertura y lectura del archivo, utilizamos try/catch
      try {
        //Abrimos el archivo para lectura y lo asignamos a un Streamer para poder leerlo
        val myFile=openFileInput(filename) //Abrimos el archivo en modo lectura
        val myInputStreamer=InputStreamReader(myFile) //Le asignamos un Streamer
        val myBuffer=BufferedReader(myInputStreamer) //Le asignamos un buffer
        var txt="" //Variable en la que compondremos el texto leido del archivo. Inicialmente vacía
        var myLine=myBuffer.readLine() //Leemos la primera de las líneas del archivo texto
        while (myLine != null) { //Mientras se haya podido leer algo...
          txt+=myLine+"\n" //Añadimos la línea al texto total (con su fin de línea)
          myLine=myBuffer.readLine() //Leemos la siguiente línea
        }
        //Hemos terminado de leer todo el contenido del archivo
        myBuffer.close() //Cerramos el buffer de lectura
        myInputStreamer.close() //Cerramos el Streamer
        myFile.close() //Cerramos el archivo
        ui.editText.setText(txt) //Asignamos el texto leido al EditText
      } catch (e:IOException) {
        //Si tenemos algún problema para leer el archivo, no haremos nada especial.
        //El EditText se quedará vacío
      }
    }

    ui.buttonSave.setOnClickListener { //Se ha pulsado el botón de guardar
      //Utilizamos try/catch para evitar excepciones al guardar
      try {
        val myFile=openFileOutput(filename,Activity.MODE_PRIVATE) //Abrimos el archivo para escritura
        //Un archivo (sin Streamer asociado) sólo permite escribir arrays de bytes, no texto
        //Así que obtenemos el texto del EditText, lo convertimos en Sting y a continuación lo
        //volvemos a convertir en un Array de Bytes
        //De esta manera no necesitamos utilizar un Streamer
        myFile.write(ui.editText.text.toString().toByteArray())
        myFile.close() //Cerramos el archivo
        Toast.makeText(this,"Guardado correctamente",Toast.LENGTH_LONG).show()
      } catch (e:IOException) {
        //Si tenemos algún problema para guardar el archivo, simplemente notificamos
        Toast.makeText(this,"Error al guardar",Toast.LENGTH_LONG).show()
      }
    }
  }
}