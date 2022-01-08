/*
  Tema: Almacenamiento permanente en archivos
  Autor: Ramón Junquera
  Fecha: 20210622
  - RadioGroup con 3 opciones para selecionar la ubicación de guardado
  - Objeto EditText multilínea
  - Botón guardar
  Funcionamiento:
  - El RadioGroup sólo tiene acticadas las opciones detectadas.
  - Podemos escribir un texto en el EditText. Tras pulsar el botón guardar se almacena de manera
    permanente en un archivo de texto en la ubicación seleccionada en RadioGroup.
  Detalles técnicos:
    Android permite almacenar información permanente por distintos métodos:
    - Como preferencias de la aplicación.
      Ya hemos desarrollado un ejemplo para este caso en concreto.
    - En la carpeta interna de la aplicación
      La carpeta interna de la aplicación se encuentra junto con la propia aplicación. Sólo la
      aplicación tiene acceso a esta carpeta. No se permite que una aplicación acceda a la carpeta
      interna de otra aplicación. Desde el exterior, a través de un explorador de archivos, no se
      puede llegar hasta aquí, a no ser que el dispositivos esté "rooteado".
      Para el ejemplo actual será: /data/user/0/com.rojo.app01/files
      El volumen ocupado quedará reflejado en el apartado de datos del usuario cuando solicitamos
      información de la aplicación.
    - En la memoria externa emulada
      Es una memoria de estructura similar a una SD, pero no extraible. No es obligatorio que todos
      los dispositivos la tengan.
      Para el ejemplo actual sería: /storage/emulated/0/Android/data/com.rojo.app01/files
      Por defecto no se permite que una aplicación lea o escriba en un path distinto al suyo.
    - En la memoria externa real
      Corresponde con la memoria de la tarjeta SD. Es una memoria externa real. No tiene porqué
      estar disponible. Al igual que con la memoria externa emulada, se permite guardar con
      libertad.
      Las versiones antiguas de sistema operativo utilizan el path:
        /storage/extSdCard/Android/data/com.rojo.app01/files
      Las nuevas componen el path con el id de la tarjeta SD:
        /storage/3DAE-104D/Android/data/com.rojo.app01/files
      Por defecto no se permite que una aplicación lea o escriba en un path distinto al suyo.
    - En una base de datos local
      Este caso lo comentaremos con un ejemplo propio.
  Notas:
  - Para acceso a memoria interna no se necesitan permisos especiales.
  - De la misma manera que se necesitan especificar permisos especiales para aplicaciones que
    utilizan comunicación con Internet, también debemos hacerlo con el acceso memoria externa.
    Esto se hace en el archivo app/manifest/AndroidManifest.xml
    Incluiremos al final del archivo, dentro de la sección manifest, y después de la sección
    application la siguiente línea:
      <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE" />
  - Si sólo necesitásemos leer podemos utilizar el siguiente permiso:
      <uses-permission android:name="android.permission.READ_EXTERNAL_STORAGE" />
  - En este ejemplo se utilizan archivos de texto, pero se podrían generar archivos binarios.
  - El objeto EditText es multi línea. Se han cambiado sus propiedades en inputType/textMultiLine
  - El objeto EditText por defecto centra verticalmente el texto. Lo podemos cambiar en gravity/top
  - Definimos el nombre del archivo como constante privada de la clase de la Activity
  - Cuando se elimina la aplicación, también se eliminan las distintas carpetas de la aplicación
    repartidas por todas las ubicaciones, con los archivos que contengan.
*/

package com.rojo.app01

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.Toast
import androidx.core.content.ContextCompat
import com.rojo.app01.databinding.ActivityMainBinding
import java.io.*

class MainActivity : AppCompatActivity() {
    private val filename="borrame.txt" //Nombre del archivo
    private var selectedIndex=-1 //Índice de opción seleccionada en el RadioGropup
    private lateinit var ui: ActivityMainBinding
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        ui=ActivityMainBinding.inflate(layoutInflater)
        setContentView(ui.root)

        //El RadioGroup muestra las siguientes opciones:
        //- Archivo en carpeta interna de aplicación
        //- Archivo en memoria externa emulada
        //- Archivo en memoria externa real (SD)
        //Inicialmente las dos últimas estás desactivadas porque no siempre están disponibles
        //Crearemos un Array de String donde guardaremos el path de cada una de las opciones:
        // 0 = carpeta interna de la aplicación
        // 1 = memoria externa emulada
        // 2 = memoria externa real (SD)
        //La primera opción siempre está disponible y guardamos su path y dejamos el resto vacíos.
        val paths=arrayOf(filesDir.toString(),"","")
        //Preguntamos por las ubicaciones para guardar archivos externos
        val dirs=ContextCompat.getExternalFilesDirs(this, null)
        for(dir in dirs) { //Recorremos todas las ubicaciones detectadas...
            if(dir != null) { //Si es un path válido...
                //Si el path contiene la palabra emulated...
                if(dir.toString().contains("emulated")) { //Es memoria externa emulada...
                    ui.radioButton1.isEnabled=true //Activamos el RadioButton
                    paths[1]=dir.toString() //Guardamos el path
                } else { //Es memoria externa real (SD)...
                    ui.radioButton2.isEnabled=true //Activamos el RadioButton
                    paths[2]=dir.toString() //Guardamos el path
                }
            }
        }

        //Listener de cambio de opción del RadioGroup
        ui.radioGroup.setOnCheckedChangeListener { group, checkedId ->
            //Nececitamos conocer el índice de la opción seleccionada
            //Tenemos el id del RadioButton seleccionado en checkedId
            //Tenemos el RadioGroup en group
            //Localizamos la View del RadioButton seleccionado
            //Obtenemos el índice de la vista del RadioGroup
            selectedIndex=group.indexOfChild(group.findViewById(checkedId))

            //Para evitar excepciones en la apertura y lectura del archivo, utilizamos try/catch
            //Podrían ocurrir no si existe el archivo que queremos abrir. Esto ocurre la primera vez
            try {
                val myFile= File(paths[selectedIndex],filename) //Ubicación del archivo
                val myFileInputStream= FileInputStream(myFile) //Abrimos archivo para lectura
                val myInputStreamReader= InputStreamReader(myFileInputStream)
                val myBufferReader= BufferedReader(myInputStreamReader)
                var txt="" //Variable en la que compondremos todo el texto leido
                var myLine=myBufferReader.readLine() //Leemos la primera línea
                while (myLine != null) { //Mientras se haya podido leer algo...
                    txt+=myLine+"\n" //Añadimos la línea al texto total (con su fin de línea)
                    myLine=myBufferReader.readLine() //Leemos la siguiente línea
                }
                myBufferReader.close()
                myInputStreamReader.close()
                myFileInputStream.close()
                ui.editText.setText(txt) //Asignamos texto leido a EditText
            } catch (e: IOException) { //Si tenemos algún problema para leer el archivo
                ui.editText.setText("") //El EditText se quedará vacío
            }
        }

        ui.buttonSave.setOnClickListener { //Se ha pulsado el botón de guardar
            //Utilizamos try/catch para evitar excepciones al guardar
            //Podría darse el caso de que no haya espacio suficiente
            try {
                val myFile=File(paths[selectedIndex],filename) //Ubicación del archivo
                val myFileOutputStream=FileOutputStream(myFile) //Abrimos archivo para escritura
                //FileOutputStream permite escribir con el método write, pero sólo en formato
                //ByteArray, así que obtenemos el texto del EditText, lo convertimos en String y
                //a continuación lo convertimos en ByteArray. Esto nos permite no tener que pasar
                //por la creación de un OutputStreamWriter
                myFileOutputStream.write(ui.editText.text.toString().toByteArray())
                myFileOutputStream.close()
                Toast.makeText(this,"Guardado correctamente",Toast.LENGTH_LONG).show()
            } catch (e:IOException) { //Tenemos algún problema para guardar el archivo...
                //...simplemente notificamos
                Toast.makeText(this,"Error al guardar",Toast.LENGTH_LONG).show()
            }
        }

        //En el diseño gráfico no hay ninguna opción seleccionada en el RadioGroup
        //Ahora que hemos definido el Listener, seleccionaremos la opción por defecto que llamará al
        //Listener y cargará el texto correspondiente
        ui.radioButton0.isChecked=true
    }
}