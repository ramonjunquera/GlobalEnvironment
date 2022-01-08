/*
  Tema: Gestión de base de datos SQLite
  Autor: Ramón Junquera
  Fecha: 20210608
  Estructura:
  - La base de datos se llamará "amigos" y será de tipo "MySQLite"
  - Sólo contiene una tabla llamada "personas" con los siguientes campos:
    - id (int): identificador único de la persona. Clave primaria.
    - name (text): nombre
    - age (int): edad
  - La variable que la instancia es myDB
  Funcionamiento:
  - Se muestran los 3 campos que componen un registro (id, nombre y edad) y 5 botones de acción
    (crear, buscar id, buscar nombre, modificar por id y borrar por id)
  Notas:
  - A lo largo de la aplicación utilizaremos mensajes Toast. Creamos el método show para simplificar
    los mensajes.
*/

package com.rojo.app01

import android.content.ContentValues
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.Toast
import com.rojo.app01.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {
    private lateinit var ui: ActivityMainBinding

    //Muestra un mensaje por Toast
    private fun show(txt:String) {
        Toast.makeText(this,txt, Toast.LENGTH_SHORT).show()
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        ui=ActivityMainBinding.inflate(layoutInflater)
        setContentView(ui.root)

        //Instanciamos la base de datos "amigos" del tipo MySQLite e indicamos que es de
        //lectura/escritura. Todos los Listener que atienden a las pulsaciones de los botones que
        //definiremos a continuación tendrán acceso.
        val myDB=MySQLite(this,"amigos",null,1).writableDatabase

        ui.buttonNew.setOnClickListener { //Pulsado el botón crear
            val reg=ContentValues() //Instanciamos un registro
            //Añadimos los distintos campos tomando los datos de los objetos gráficos
            reg.put("id",ui.editTextId.text.toString())
            reg.put("name",ui.editTextName.text.toString())
            reg.put("age",ui.editTextAge.text.toString())
            //Guardamos el registro en la tabla personas
            //insert devuelve -1 si no ha podido insertarlo, seguramente porque ya existe el id
            //Si no puede completar la inserción, muestra mensaje de error
            if(myDB.insert("personas",null,reg)<0) show("Error. Id existente")
            else show("Ok. Registro guardado")
        }

        ui.buttonFindId.setOnClickListener { //Pulsado el botón buscar por id
            //Hacemos una consulta simple que devuelve un cursor
            val res=myDB.rawQuery("select name,age from personas where id=${ui.editTextId.text}",null)
            //Si podemos movernos al primero de los registros...es que al menos hay uno
            if(res.moveToFirst()) {
                //Hemos solicitado que los campos devueltos sean name & age
                //Recogemos sus valores y los mostramos en los correspondientes EditText
                ui.editTextName.setText(res.getString(0))
                ui.editTextAge.setText(res.getString(1))
                show("Ok")
            } else show("Error. Id desconocido")
            res.close() //Cerramos el cursor
        }

        ui.buttonFindName.setOnClickListener { //Pulsado botón buscar por nombre
            //Hacemos una consulta simple que devuelve un cursor
            //Esta vez buscamos por un texto. Es obligatorio que vaya entre apóstrofes
            val res=myDB.rawQuery("select id,age from personas where name='${ui.editTextName.text}'",null)
            //Si podemos movernos al primero de los registros...es que al menos hay uno
            if(res.moveToFirst()) {
                //Estamos en el primero de los registros
                //Es el único que tendremos en cuenta
                //Podría ser que haya más registros con el mismo valor del campo name, puesto que no
                //es clave, pero sólo mostraremos la primera coincidencia
                ui.editTextId.setText(res.getString(0))
                ui.editTextAge.setText(res.getString(1))
                show("Ok")
            } else show("Error. Nombre inexistente")
            res.close() //Cerramos el cursor
        }

        ui.buttonUpdate.setOnClickListener { //Pulsado botón modificar por id
            val reg=ContentValues() //Instanciamos un registro
            //Añadimos los distintos campos tomando los datos de los objetos gráficos
            reg.put("name",ui.editTextName.text.toString())
            reg.put("age",ui.editTextAge.text.toString())
            //update devuelve el número de registros afectados
            //Si se ha podido actualizar alguno...mostramos mensaje Ok
            if(myDB.update("personas",reg,"id=${ui.editTextId.text}",null)>0) show("Ok")
            else show("Error. Id inexistente")
        }

        ui.buttonDelete.setOnClickListener { //Pulsado botón borrar por id
            //delete devuelve el número de registros afectados
            //Si se ha podido borrar alguno...mostramos mensaje Ok
            if(myDB.delete("personas","id=${ui.editTextId.text}",null)>0) show("Ok")
            else show("Error. Id inexistente")
        }
    }
}
