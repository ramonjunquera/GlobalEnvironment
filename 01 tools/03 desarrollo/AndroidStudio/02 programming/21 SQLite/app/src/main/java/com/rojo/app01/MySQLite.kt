/*
  Tema: Creación de clase personalizada de SQLite
  Autor: Ramón Junquera
  Fecha: 20210608
  Clase:
    Siempre que queremos utilizar una base de datos SQLite, debemos crear nuestra propia clase
    heredada de SQLiteOpenHelper.
    SQLiteOpenHelper tiene dos metodos que es obligatorio sobreescribir:
    - onCreate: inicializa la DB si no existe
    - onUpgrade: actualiza la estructura de la DB si es una versión antigua
    La ventaja de definir nuestra propia clase es que:
    - No tenemos que preocuparnos por si existe previamente la base de datos que estamso buscando.
      Si no existe, el método onCreate se debe encargar de generar e inicializar los objetos de base
      de datos que sean necesarios para su correcto funcionamiento.
    - Las base de datos siempre tienen un número de versión. Lo normal es que se comience por la
      versión 1 y se continúe aumentando secuencialmente. La estrutura interna de las distintas
      versiones no tiene porqué se la misma. El método onUpgrade se encarga de detectar la versión
      actual y la solicitada y hacerle los cambios necesarios.
    En el ejemplo actual sólo trabajaremos con una versión de base de datos, la 1. Por lo tanto no
    necesitamos especificar nada en el método onUpdate, porque nunca habrá un cambio de versión.
    El método onCreate sólo lanza la instrucción SQL para crear una tabla.
  Detalles:
    La base de datos sólo contiene una tabla llamada personas conos siguientes campos:
    - id (int): identificador único de la persona. Clave primaria.
    - name (text): nombre
    - age (int): edad
    La clase personalizada se llama MySQLite
  Creación:
  - Comenzamos localizando la carpeta que contiene el archivo .kt con código de gestión de la
    Activity principal en: /app/java/com.rojo.app01/
  - Hacemo click con el botón derecho sobre la carpeta y seleccionamos New/Kotlin Class/File
  - Dejamos seleccionado el objeto Class, escribimos el nombre de la clase: MySQLite y pulsamos
    Enter. Se crea el archivo MySQLite.kt.
  - El archivo está vacío. Indicamos que nuestra clase hereda de SQLiteOpenHelper completando la
    cabecera de clase:
      class MySQLite: SQLiteOpenHelper {
  - El primer error que nos aparece es porque no hemos implementado los miembros (métodos). Lo
    corregimos implementando los dos métodos obligatorios.
  - El segundo error indica que no hemos especificado los parámetros del constructor. Lo corregimos
    añadiendo el constructor estándar seleccionando la opción "Add constructor parameters from
    SQLiteOpenHelper(Context?, String?, SQLiteDatabase.CursorFactory?, Int)".
  - Ya sólo queda completar el método onCreate y vaciar onUpgrade
*/

package com.rojo.app01

import android.content.Context
import android.database.sqlite.SQLiteDatabase
import android.database.sqlite.SQLiteOpenHelper

class MySQLite(
    context: Context?,
    name: String?,
    factory: SQLiteDatabase.CursorFactory?,
    version: Int
) : SQLiteOpenHelper(context, name, factory, version) {
    override fun onCreate(db: SQLiteDatabase?) {
        //Lanzamos el comando SQL pra crera la única tabla de la base de datos
        db?.execSQL("create table personas(id int primary key,name text,age int)")
    }

    override fun onUpgrade(db: SQLiteDatabase?, oldVersion: Int, newVersion: Int) {
        //Nada especial que hacer aquí porque nunca hay un cambio de versión
    }

}