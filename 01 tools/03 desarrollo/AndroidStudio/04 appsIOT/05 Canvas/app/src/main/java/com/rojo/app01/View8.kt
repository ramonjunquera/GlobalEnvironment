/*
  Tema: Objeto View8
  Autor: Ramón Junquera
  Fecha: 20210527
  Herencias: Derivado de View.
  Objetivo:
    Muestra una rejilla de 8x8 círculos que permiten pulsaciones que los pintan del color
    seleccionado.
  Notas:
  - Clase heredada de View
  - Necesitaremos un objeto Paint que nos indiquen las características del dibujo en el Canvas y
    como no es una buena idea instanciarlo en el propio método onDraw cada vez que se utilice, lo
    haremos como variable privada.
  - Guardamos el tamaño del Canvas y el color seleccionado en variables privadas globales
  - En el constructor definimos la función de gestión de pulsaciones. Simplemente toma las
    coordenadas de la pulsación, la transforma a coordenadas del Array, valida si son correctas,
    actualiza el Array y solicita redibujar el Canvas.
  - Creamos un método para poder cambiar el color por defecto desde el exterior.
  - Antes de dibujar nada con onDraw, siempre se inicializa con un cambio de tamaño. Sobreescribimos
    este método y aprovechamos para tomar nota de las nuevas dimensiones.
  - Finalmente sobreescribimos el método onDraw en el que sólo dibujaremos el Bitmap en el Canvas
*/

package com.rojo.app01

import android.content.Context
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.util.AttributeSet
import android.view.View

//Objeto View customizado para que represente una matriz de 8x8
class View8 @JvmOverloads constructor(context: Context, attrs: AttributeSet? = null, defStyleAttr: Int = 0) : View(context, attrs, defStyleAttr){

    //Variables globales
    private var paint=Paint()
    private var videoMem=arrayOf<Array<Int>>() //Memoria de vídeo
    private var canvasWidth=0.0f //Anchura del canvas
    private var canvasHeight=0.0f //Altura del canvas
    private var ink:Int=Color.BLACK

    //Constructor
    init {
        //Creamos memoria de vídeo. Añadimos 8 filas de 8 elementos
        for(row in 0..7) videoMem+=arrayOf(ink,ink,ink,ink,ink,ink,ink,ink)
        setOnTouchListener { v, event -> //Gestor de pulsaciones
            v.performClick() //Informamos de un click a la clase padre
            //No filtraremos por tipo de evento, por lo tanto aceptaremos los eventos de pulsación,
            //soltado y movimiento.
            //Tomamos las coordenadas de la pulsación
            //Calculamos las coordenadas del Array
            val x=(event.x * 8/canvasWidth).toInt()
            val y=(event.y * 8/canvasHeight).toInt()
            if(x<8 && y<8) { //Si las coordenadas son válidas...
                if(ink!=videoMem[y][x]) { //Si el color es distinto...
                    videoMem[y][x]=ink //Copiamos el color seleccionado al Array
                    invalidate() //Fuerza a redibujar el Canvas (llama a onDraw)
                }
            }
            true //Hemos terminado de procesar la pulsación correctamente
        }
    }

    //Asigna nuevo color seleccionado
    fun setInkColor(color:Int) {
        ink=color //Guardamos parámetro en variable privada
    }

    override fun onSizeChanged(w: Int, h: Int, oldw: Int, oldh: Int) { //Si cambia de tamaño
        super.onSizeChanged(w, h, oldw, oldh)
        //Guardamos el tamaño del canvas en variables globales
        canvasWidth=width.toFloat()
        canvasHeight=height.toFloat()
    }

    //Llamada cada vez que se deba redibujar el objeto gráfico
    override fun onDraw(canvas: Canvas?) {
        super.onDraw(canvas) //Llamamos al constructor de la clase padre

        //Dibujamos el fondo
        paint.color=Color.parseColor("#222222") //Gris oscuro
        canvas?.drawRect(0f,0f, canvasWidth, canvasHeight,paint) //Fondo

        //Dibujamos la rejilla
        paint.color=Color.WHITE
        var offsetX:Float
        var offsetY:Float
        for(i in 1..7) { //Dibujamos la rejilla
            offsetX=canvasWidth*i/8 //Coordenada x de la línea vertical
            canvas?.drawLine(offsetX,0f,offsetX,canvasHeight,paint) //
            offsetY=canvasHeight*i/8 //Coordenada y de la línea horizontal
            canvas?.drawLine(0f,offsetY,canvasWidth,offsetY,paint)
        }

        //Dibujamos los puntos
        val radius=canvasWidth/18
        offsetX=canvasWidth/16 //Offset X
        offsetY=canvasHeight/16 //Offset Y
        for(y in 0..7) {
            val rowY=canvasHeight*y/8 //Y de la fila
            for(x in 0..7) {
                paint.color=videoMem[y][x] //Tomamos el color de la memoria de vídeo
                canvas?.drawCircle(canvasWidth/8*x+offsetX,rowY+offsetY,radius,paint)
            }
        }
    }
}