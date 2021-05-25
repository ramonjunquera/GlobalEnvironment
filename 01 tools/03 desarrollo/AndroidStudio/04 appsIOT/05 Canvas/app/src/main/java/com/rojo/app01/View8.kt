/*
  Clase: View8
  Herencias: Derivado de View.
  Objetivo:
    Muestra una rejilla de 8x8 círculos que permiten pulsaciones que los pintan del color
    seleccionado.
  Notas:
  - La clase hereda las propiedades tanto del objeto View como de lu Listener de pulsaciones
    OnTouchListener.
  - Se autoconfigura como su propio Listener de pulsaciones.
  - Actualiza la memoria de vídeo, el Canvas y el color del botón de selección de color.
  - No necesita variables públicas
  - La notificación del botón de selección de color y la actualización de color se hacen mediante
    métodos.
*/

package com.rojo.app01

import android.content.Context
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.util.AttributeSet
import android.view.MotionEvent
import android.view.View
import android.widget.Button

//Objeto View customizado para que represente una matriz de 8x8
class View8 @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null, defStyleAttr: Int = 0
) : View(context, attrs, defStyleAttr),View.OnTouchListener{

    //Variables globales
    private var paint=Paint()
    private var videoMem=arrayOf<Array<Int>>() //Memoria de vídeo
    private var canvasWidth=0.0f //Anchura del canvas
    private var canvasHeight=0.0f //Altura del canvas
    private var _selectedColor:Int=0
    private var _colorButton:Button?=null

    //Constructor
    init {
        //Creamos memoria de vídeo
        val b=Color.BLACK //Se llenará del color negro
        for(row in 0..7) videoMem+=arrayOf(b,b,b,b,b,b,b,b) //Añadimos 8 filas de 8 elementos
        setOnTouchListener(this) //El listener de clicks del objeto será esta misma clase
    }

    //Asigna el botón de selección de color
    fun setColorButton(colorButton:Button) {
        _colorButton=colorButton //Guardamos parámetro en variable privada
    }

    //Asigna nuevo color seleccionado
    fun setSelectedColor(selectedColor:Int) {
        _selectedColor=selectedColor //Guardamos parámetro en variable privada
        _colorButton!!.background.setTint(_selectedColor) //Asignamos el color al botón de selección
    }

    //Llamada cada vez que se deba redibujar el objeto gráfico
    override fun onDraw(canvas: Canvas?) {
        super.onDraw(canvas) //Llamamos al constructor de la clase padre

        //Guardamos el tamaño del canvas en variables globales
        canvasWidth=width.toFloat()
        canvasHeight=height.toFloat()

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

    //Método de clase View.OnTouchListener
    override fun onTouch(v: View?, event: MotionEvent?): Boolean {
        //Se ha pulsado en algún punto del Canvas del objeto
        if (event?.actionMasked == MotionEvent.ACTION_DOWN) { //Si es un evento de pulsación..
            //En base a las coordenadas del click, calculamos las coordenadas del Array
            val x=(event.x * 8/canvasWidth).toInt()
            val y=(event.y * 8/canvasHeight).toInt()
            videoMem[y][x]= _selectedColor //Copiamos el color seleccionado al Array
            invalidate() //Refrescamos la vista (obligamos a llamar a onDraw)
        }
        return false // let the touch event pass on to whoever needs it
    }
}