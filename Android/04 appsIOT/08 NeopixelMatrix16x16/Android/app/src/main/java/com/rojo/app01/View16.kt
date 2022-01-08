/*
  Tema: Objeto View16
  Autor: Ramón Junquera
  Fecha: 20210827
  Herencias: Derivado de View.
  Objetivo:
    Muestra una rejilla de 16x16 círculos/pixels.
    Tiene métodos que nos permiten asignar un color a unas coordenadas o recuperar un color de unas
    coordenadas.
    También tiene un método para definir la función a la que de debe llamar cuando se detecte un
    click sobre un pixel. Como parámetros le pasa las coordenadas.
  Notas:
  - Clase heredada de View
  - No necesita compartir ui con la clase principal porque no accede a objetos gráficos que no
    sean propios.
  - Necesitaremos un objeto Paint que defina la manera de dibujar en el Canvas y como no es una
    buena idea instanciarlo en el propio método onDraw cada vez que se utilice, lo haremos como
    variable privada.
  - Guardamos el tamaño del Canvas en variables privadas globales
  - La función a la que debemos llamar cuando se haga click será nullable. No es obligatorio
    definir esta función. Por defecto es null. Gracias a esto podremos distinguir si la función
    ha sido definida o no.
  - Antes de dibujar nada con onDraw, siempre se inicializa con un cambio de tamaño. Sobreescribimos
    este método y aprovechamos para tomar nota de las nuevas dimensiones.
  - Sobreescribimos el método onDraw en el que dibujaremos la rejilla y los puntos en el Canvas
*/

package com.rojo.app01

import android.content.Context
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.util.AttributeSet
import android.view.View

class View16 @JvmOverloads constructor(context: Context, attrs: AttributeSet? = null, defStyleAttr: Int = 0) : View(context, attrs, defStyleAttr) {
    //Variables globales
    private var videoMem=arrayOf<Array<Int>>() //Memoria de vídeo
    private var canvasWidth=0.0f //Anchura del canvas
    private var canvasHeight=0.0f //Altura del canvas
    //Instanciamos la clase Paint para utilizar su variable color
    //Lo hacemos aquí para evitar hacerlo en la rutina onDraw en la que no se debería perder el
    //tiempo inicializando variables
    private var paint= Paint()
    //Función a llamar cuando se detecta pulsación. Inicialmente no hay ninguna
    private var clickFun: ((Int, Int) -> Unit)? =null

    //Constructor
    //Lo definimos aparte para poder inicializar los parámetros del objeto.
    init {
        val c=Color.BLACK //Inicialmente los pixels son negros
        //Creamos memoria de vídeo. Añadimos 16 filas de 16 elementos
        for(row in 0..15) videoMem+=arrayOf(c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c)

        setOnTouchListener { v,event-> //Gestor de pulsaciones
            v.performClick() //Informamos de un click a la clase padre
            //No filtraremos por tipo de evento, por lo tanto aceptaremos los eventos de pulsación,
            //soltado y movimiento.
            //Tomamos las coordenadas de la pulsación
            //Calculamos las coordenadas del Array
            val col=(event.x*16/canvasWidth).toInt()
            val row=(event.y*16/canvasHeight).toInt()
            //Si las coordenadas son válidas y tenemos definida la función a llamar...
            if(col<16 && row<16 && clickFun!=null) {
                //Nos aseguramos que la función no es nula y la llamamos (invocamos) con sus
                //parámetros (row,col)
                clickFun?.invoke(row,col)
            }
            true //Hemos terminado de procesar la pulsación correctamente
        }
    }

    //Gestión de cambio de tamaño
    override fun onSizeChanged(w: Int, h: Int, oldw: Int, oldh: Int) {
        super.onSizeChanged(w, h, oldw, oldh)
        //Guardamos el tamaño del canvas en variables globales
        canvasWidth=width.toFloat()
        canvasHeight=height.toFloat()
    }

    //Llamada cada vez que se deba redibujar el objeto gráfico
    override fun onDraw(canvas: Canvas?) {
        super.onDraw(canvas) //Llamamos al constructor de la clase padre
        //Dibujamos el fondo
        paint.color= Color.parseColor("#222222") //Gris oscuro
        canvas?.drawRect(0f,0f, canvasWidth, canvasHeight,paint) //Fondo

        //Dibujamos la rejilla
        paint.color=Color.WHITE
        var offsetX:Float
        var offsetY:Float
        for(i in 1..15) { //Dibujamos la rejilla
            offsetX=canvasWidth*i/16 //Coordenada x de la línea vertical
            canvas?.drawLine(offsetX,0f,offsetX,canvasHeight,paint) //
            offsetY=canvasHeight*i/16 //Coordenada y de la línea horizontal
            canvas?.drawLine(0f,offsetY,canvasWidth,offsetY,paint)
        }

        //Dibujamos los puntos
        val radius=canvasWidth/40
        offsetX=canvasWidth/32 //Offset X
        offsetY=canvasHeight/32 //Offset Y
        for(y in 0..15) {
            val rowY=canvasHeight*y/16 //Y de la fila
            for(x in 0..15) {
                paint.color=videoMem[y][x] //Tomamos el color de la memoria de vídeo
                canvas?.drawCircle(canvasWidth/16*x+offsetX,rowY+offsetY,radius,paint)
            }
        }
    }

    //Actualiza el color de un pixel y lo muestra
    //Devuelve true si lo consigue
    fun drawPixel(row:Int,col:Int,color:Int):Boolean {
        if(row>15 || col>15) return false //Si las coordenadas están fuera de rango...error
        videoMem[row][col]=color //Guardamos el color en la memoria de vídeo
        invalidate() //Fuerza a redibujar el Canvas (llama a onDraw)
        return true //Todo Ok
    }

    //Devuelve el color de un pixel
    //Si las coordenadas están fuera de rango devuelve 0
    fun getPixel(row:Int,col:Int):Int {
        if(row>15 || col>15) return 0 //Si las coordenadas están fuera de rango...error
        return videoMem[row][col]
    }

    //Definición de función que gestionará las pulsaciones de los pixels
    //La función a la que se llamará debe tener los siguientes parámetros:
    //- row:Int
    //- col:Int
    fun onClickListener(f:(Int,Int)->Unit) {
        clickFun=f //Guardamos función en variable privada
    }

}