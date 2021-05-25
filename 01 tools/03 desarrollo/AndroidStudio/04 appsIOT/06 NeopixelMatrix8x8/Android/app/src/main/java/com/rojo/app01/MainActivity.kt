/*
  Tema: Control remoto de una matriz de leds RGN Neopixel 8x8
  Autor: Ramón Junquera
  Fecha: 20210524
  Controles:
  - Dirección ip y puerto para que el usuario defina el servidor.
  - Switch para establecer conexión
  - 3 SeekBar, una por canal de color, que nos permiten seleccionar el color.
  - View con el color seleccionado
  - View modificado para que muestre en su Canvas una matriz de 8x8 que además admite pulsaciones
  Protocolo:
  - El valor 255 se considera un comando. Servirá para indicar que se envía un color nuevo.
  - Una imágen consta de 8 filas, 8 columnas y 3 canales de color.
  - La imágen se envía por filas, columnas y canales de color.
  - Los canales de color tienen como valor máximo 254, para no ser confundidos con el comando reset.
  - Cada vez que se vaya a enviar un nuevo color, se enviará primero el código reset. Esto
    asegurará la correcta sincronización de canales, aunque se produzcan errores de comunicación.
  Estructura:
  - Todo el código se encuentra en un único archivo.
  - Se utilizan algunas variables globales que permiten compartir su contenido entre clases
  - El contexto que permite acceder a todos los objetos gráficos no se declara como variable porque
    podría provocar pérdidas de memoria. Si alguna clase lo necesita, se le pasa como parámetro.
  Funcionamiento:
  - Puesto que el color debe recalcularse con la modificación de cualquiera de las SeekBar, creamos
    un Listener único para todas las SeekBar.
  - Tenemos la obligación de sobreescribir todos los métodos de la clase original del Listener,
    aunque no lo vayamos a utilizar.
  - Cuando se modifica cualquiera de las SeekBar, tomamos el progreso de cada una, pintamos el
    fondo del botón de selección.
  - Para evitar que las SeekBar generen el código reset, se limitan a 254.
  - Cuando solicita conexión con el control Switch, lanzamos un Thread para que gestione las
    operaciones de conexión y desconexión sin que afecte al hilo principal.
  - Si queremos conectar, desactivamos los controles de ip y puerto, tomamos sus valores e
    intentamos la conexión con el socket.
    Si no lo conseguimos, volveremos adejar los controles como al principio.
    Si conectamos llámamos al método de nuestro Listener que gestiona los cambios en las SeekBar.
    Así forzamos a que se envíe una actualización del color actual al servidor.
  Requerimientos:
  - Es necesario indicar que la aplicación necesitará permisos para conectarte con el exterior.
    Esto se hace en el archivo app/manifest/AndroidManifest.xml
    Incluiremos al final del archivo, bajo la sección manifest, y después de la sección application
    la siguiente línea:
      <uses-permission android:name="android.permission.INTERNET" />
  Notas:
  - Cuando enviamos un comando, no se cierra la comunicación a continuación. Por lo tanto no
    es necesario forzar la descarga del buffer de salida (flush).
  - Las actualizaciones de objetos gráficos desde Thread secundarios no está permitida. Siempre
    de debe hacer desde el Thread principal. Por eso se utiliza el Looper. Se añaden tareas mediante
    un Handler
*/

package com.rojo.app01

import android.content.Context
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.support.v7.app.AppCompatActivity
import android.util.AttributeSet
import android.view.MotionEvent
import android.view.View
import android.widget.SeekBar
import com.rojo.app01.databinding.ActivityMainBinding
import java.net.Socket

//Variables globales
var videoMem=arrayOf<Array<Int>>() //Memoria de vídeo
var canvasWidth=0.0f //Anchura del canvas
var canvasHeight=0.0f //Altura del canvas
var selectedColor:Int=Color.BLACK //El color seleccionado por defecto es negro
var pickColor:Boolean=false //Se ha pulsado el botón de selección de color?
lateinit var client:Socket //Declaramos el objeto de gestión del Socket
lateinit var mainHandler: Handler //Handle de la Activity

//Obtención de coordenadas de pulsación para View8
class MyOnTouchListener(private var ui:ActivityMainBinding):View.OnTouchListener {
    override fun onTouch(v: View?, event: MotionEvent?): Boolean {
        v?.performClick() //Informamos que se ha detectado un click a la clase padre
        //Se ha pulsado en algún punto del Canvas del objeto
        if (event?.actionMasked == MotionEvent.ACTION_DOWN) { //Si es un evento de pulsación..
            //En base a las coordenadas del click, calculamos las coordenadas del Array
            val x=(event.x * 8/canvasWidth).toInt()
            val y=(event.y * 8/canvasHeight).toInt()
            if(pickColor) { //Si se debe recuperar el color al botón de selección...
                val myColor=videoMem[y][x] //Recuperamos el color
                //Descomponemos el color y lo aplicamos al progreso de las SeekBar
                ui.seekBarR.progress=Color.red(myColor)
                ui.seekBarG.progress=Color.green(myColor)
                ui.seekBarB.progress=Color.blue(myColor)
                //No asignamos el color al botón, porque con cada cambio en las Seekbar se actualiza
                pickColor=false //Misión cumplida
            } else { //Se debe aplicar el color del botón al punto seleccionado
                videoMem[y][x]= selectedColor //Copiamos el color seleccionado al Array
                ui.view.invalidate() //Refrescamos la vista (obligamos a llamar a onDraw)
                //Si estamos conectados...enviamos la imágen
                if(ui.switchConectar.isChecked) ui.view.send()
            }
        }
        return false //Permitimos que el evento pueda ser procesado por otras rutinas
    }
}

//Objeto View customizado para que representa una matriz de 8x8
class View8 @JvmOverloads constructor(context:Context,attrs:AttributeSet?=null,defStyleAttr:Int=0):View(context,attrs,defStyleAttr) {

    //Variable globales
    private var paint=Paint() //Creamos la variable Paint para no tener que instanciarla en el método onDraw

    fun setTouchListener(ui:ActivityMainBinding) {
        //Instanciamos un Listener de pulsaciones de View
        setOnTouchListener(MyOnTouchListener(ui))
    }

    fun send() { //Enviamos imágen actual
        val out=client.getOutputStream() //Anotamos el OutputStream para enviar por el cliente
        //No es obligatorio, pero sí conveniente lanzar la operación de envío en segundo plano
        //Las posibles esperas no afectarán a la fluidez de la aplicación
        Thread {
            try {
                out.write(255) //Index channel reset (nuevo color)
                for(row in 0..7) { //Recorremos todas las filas
                    for(col in 0..7) { //Recorremos todas las columnas
                        val color=videoMem[row][col] //Anotamos el color
                        out.write(Color.red(color)) //Enviamos canal R
                        out.write(Color.green(color)) //Enviamos canal G
                        out.write(Color.blue(color)) //Enviamos canal B
                    }
                }
            } catch (e: Exception) { //Si tenemos algún un problema...
                //...el envío se pierde
            }
        }.start() //Y lo lanzamos inmediatamente
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
}

//Gestión de SeekBar.progress
class MySeekBarListener(private var ui:ActivityMainBinding) : SeekBar.OnSeekBarChangeListener {
    override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
        //Obtenemos los valores de los canales
        val r=ui.seekBarR.progress
        val g=ui.seekBarG.progress
        val b=ui.seekBarB.progress
        selectedColor=Color.rgb(r,g,b) //Calculamos y guardamos el color
        ui.buttonColor.setBackgroundColor(selectedColor) //Asignamos el color al botón
    }
    //El resto de métodos no nos utilizaremos, pero su declaración es obligatoria
    override fun onStartTrackingTouch(seekBar: SeekBar?) {}
    override fun onStopTrackingTouch(seekBar: SeekBar?) {}
}

//Gestión de pulsación de botón de selección de color
class ButtonColorOnClickListener:View.OnClickListener {
    override fun onClick(v: View?) { //Se ha pulsado el botón de selección
        pickColor=true
    }
}

//Gestión de pulsación de switch de conexión
class ConnectOnClickListener(private var ui:ActivityMainBinding):View.OnClickListener {

    override fun onClick(v: View?) { //Se han cambiado el Switch de conectar
        Thread { //Creamos un Thread para gestionar la conexión/desconexión
            if(ui.switchConectar.isChecked) { //Si quiere conectar...
                //Solicitamos la desactivación de los controles de ip & puerto
                mainHandler.post { //Añadimos la rutina a ejecutar por el Handler
                    ui.editTextIP.isEnabled=false //Control ip desactivado
                    ui.editTextPort.isEnabled=false //Control puerto desactivado
                }
                try {
                    //Intentamos conectar
                    client=Socket(ui.editTextIP.text.toString(),ui.editTextPort.text.toString().toInt())
                    //No ha dado error. Hemos conectado!
                    ui.view.send() //Enviamos imágen actual
                } catch (e: Exception) { //Si ocurre cualquier problema (no hemos podido conectar)...
                    //Solicitamos dejar los controles como al principio
                    mainHandler.post { //Añadimos la rutina a ejecutar por el Handler
                        ui.switchConectar.isChecked=false //No estamos conectados
                        ui.editTextIP.isEnabled=true //Control IP activado
                        ui.editTextPort.isEnabled=true //Control puerto activado
                    }
                }
            } else { //Si se quiere desconectar...
                try {
                    client.close() //Cerramos la conexión con el Socket
                } catch (e: Exception) { //Si ocurre cualquier problema...
                    //No hacemos nada especial
                    //Esto puede ocurrir si queremos cerrar una conexión que ya se ha perdido
                }
                //Solicitamos la activación de los controles de ip & puerto
                mainHandler.post { //Añadimos la rutina a ejecutar por el Handler
                    ui.editTextIP.isEnabled=true
                    ui.editTextPort.isEnabled=true
                }
            }
        }.start() //Lanzamos el Thread que conecta/desconecta
    }
}

//Gestión principal de Activity
class MainActivity : AppCompatActivity() {
    private lateinit var ui: ActivityMainBinding //Acceso a objetos gráficos
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        ui=ActivityMainBinding.inflate(layoutInflater)
        setContentView(ui.root)

        mainHandler=Handler(Looper.getMainLooper()) //Handle de la Activity

        //Creamos memoria de vídeo
        val b=Color.BLACK //Se llenará del color negro
        for(row in 0..7) videoMem+=arrayOf(b,b,b,b,b,b,b,b) //Añadimos 8 filas de 8 elementos

        ui.buttonColor.setBackgroundColor(b) //Inicialmente el botón se de selección es negro

        //Instanciamos un único Listener para todas las SeekBar
        val mySeekBarListener=MySeekBarListener(ui)
        //Lo utilizamos como gestor de eventos de todas las SeekBar
        ui.seekBarR.setOnSeekBarChangeListener(mySeekBarListener)
        ui.seekBarG.setOnSeekBarChangeListener(mySeekBarListener)
        ui.seekBarB.setOnSeekBarChangeListener(mySeekBarListener)

        ui.buttonColor.setOnClickListener(ButtonColorOnClickListener()) //Gestión de pulsación
        ui.view.setTouchListener(ui) //Asignamos rutina de gestión de coordenadas de pulsación
        ui.switchConectar.setOnClickListener(ConnectOnClickListener(ui)) //Gestión de cambio de estado
    }
}
