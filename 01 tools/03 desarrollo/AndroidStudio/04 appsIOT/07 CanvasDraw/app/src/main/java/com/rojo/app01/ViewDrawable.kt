/*
  Tema: Objeto View para dibujar
  Autor: Ramón Junquera
  Fecha: 20210526
  Funcionamiento:
  - Muestra un punto en las coordenadas de la pulsación.
  Notas:
  - Clase derivada de View
  - Sabemos que el Canvas se redibuja completamente cada vez que se llama al método onDraw, por lo
    tanto, necesitamos almacenar nuestro dibujo en algún sitio. Lo más sencillo es hacerlo en un
    Bitmap, puesto que después es fácil asignarlo al Canvas. Lo declaramos como variable privada.
  - Necesitaremos un objeto Paint que nos indiquen las características del dibujo en el Canvas y
    como no es una buena idea instanciarlo en el propio método onDraw cada vez que se utilice, lo
    haremos como variable privada.
  - Necesitamos guardar el color seleccionado. También lo hacemos en una variable privada.
  - En el constructor definimos la función de gestión de pulsaciones. Simplemente toma las
    coordenadas de la pulsación y dibuja un cuadrado con el color seleccionado.
  - Creamos un método para poder cambiar el color por defecto desde el exterior.
  - Antes de dibujar nada con onDraw, siempre se inicializa con un cambio de tamaño. Sobreescribimos
    este método y aprovechamos para tomar nota de las nuevas dimensiones, crear un nuevo Bitmap del
    mismo tamaño y guardarlo en la variable privada.
  - Finalmente sobreescribimos el método onDraw en el que sólo dibujaremos el Bitmap en el Canvas
*/

package com.rojo.app01

import android.content.Context
import android.graphics.Bitmap
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.util.AttributeSet
import android.view.View

class ViewDrawable @JvmOverloads constructor(context: Context, attrs: AttributeSet? = null, defStyleAttr: Int = 0) : View(context, attrs, defStyleAttr) {

    private lateinit var myCanvasBitmap:Bitmap
    private var myCanvasPaint=Paint()
    private var myInkColor=Color.BLACK //Color de tinta inicialmente negro

    init { //Constructor
        setOnTouchListener { v, event -> //Gestor de pulsaciones
            v.performClick() //Informamos de un click a la clase padre
            //Tomamos las coordenadas de la pulsación y las redondeamos a entero, porque las
            //pulsaciones o el Canvas trabajan con Float, pero Bitmap lo hace con Int
            val x=event.x.toInt()
            val y=event.y.toInt()
            //println("($x,$y)")
            //Un pixel es demasiado pequeño para que podamos distinguirlo
            //Dibujaremos un cuadrado de 5x5 en las coordenadas de pulsación
            for(xRange in x-2..x+2) {
                for(yRange in y-2..y+2) {
                    myCanvasBitmap.setPixel(xRange,yRange,myInkColor)
                }
            }
            invalidate() //Fuerza a redibujar el Canvas (llama a onDraw)
            true //Hemos terminado de procesar la pulsación correctamente
        }
    }

    //Fija un nuevo color de dibujo
    fun setInkColor(color:Int) {
        myInkColor=color
    }

    override fun onSizeChanged(w: Int, h: Int, oldw: Int, oldh: Int) { //Si cambia de tamaño
        super.onSizeChanged(w, h, oldw, oldh)
        //println("Nuevo tamaño ($w,$h)")
        //Creamos un nuevo Bitmap del tamaño del nuevo Canvas
        //Todos sus pixels serán transparentes
        myCanvasBitmap=Bitmap.createBitmap(w,h,Bitmap.Config.ARGB_8888)
        //Pintamos todos los pixels de blanco opaco
        //Si no lo hacemos se conservará la transparencia original y los pixels dibujados no se verán
        myCanvasBitmap.eraseColor(Color.WHITE)
    }

    override fun onDraw(canvas: Canvas?) { //Redibujar Canvas
        //Se nos entrega un Canvas nuevo y vacío
        super.onDraw(canvas) //Constructor de clase padre
        //Dibujamos Bitmap en Canvas desde el origen (0,0)
        canvas?.drawBitmap(myCanvasBitmap,0.0f,0.0f,myCanvasPaint)
    }
}