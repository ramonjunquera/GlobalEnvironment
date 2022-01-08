/*
  Tema: Creación y gestión de objeto gráfico
  Autor: Ramón Junquera
  Fecha: 20210827
  Controles:
  - Un EditText para la dirección ip.
  - Un EditText para el puerto.
  - Un Switch para poder conectar/desconectar
  - 3 SeekBar, una por canal de color, que nos permiten seleccionar el color.
  - View con el color seleccionado
  - Objeto View8 que representa una matriz de 16x16
  Funcionamiento:
  - Con el Switch podemos conectar con el servidor teniendo en cuenta los parámetros de ip & puerto
  - Las 3 SeekBar permiten componer el color selecionado.
  - Cuando se pulsa en el objeto View8 se pueden pintar los círculos con el color seleccionado.
  - La pulsación puede arrastrarse
  - Al pulsar en el View de color seleccionado se puede tomar alguno de los que ya se han utilizado.
  Protocolo:
  - Una imagen consta de 16 filas, 16 columnas y 3 canales de color por pixel.
  - Una vez establecida la conexión con el servidor, se mantiene abierta, aunque no se transmita nada.
  - La transmisión es unidireccional. Desde el cliente al servidor.
  - Un envío consta lo forma un sólo paquete con la información de un pixel. Su tamaño es de 4 bytes.
    el primero contiene las coordenadas con la codificación xxxxyyyy, los tres siguientes
    corresponden con los canales de color del pixel.
  - Cuando se establece conexión, se envía el contenido de la imagen completa a base de paquetes.
  Notas:
  - Dentro de la clase de la Activity, creamos una clase heredada del Listener de SeekBar que
    permite cambiar el color seleccionado.
  - Esta clase la instanciaremos y la aplicaremos como Listener a todas las SeekBar, porque todas
    tienen el mismo funcionamiento. Cuando hay una variación en cualquiera de ellas, se debe
    recalcular el color seleccionado y aplicar.
  - Ante cualquier cambio de color, el Listener calcula el nuevo color, lo aplica al View del color
    seleccionado.
  - Las tres SeekBar tienen un rango de byte [0,255] para que coincida con al canal de color.
  - La conexión es algo que se debe poder utilizar en cualquiera de las clases que definamos,
    por eso se declara como variable global.
  - Cuando se establece conexión se da valor a la variable clientOutputStream que contiene el
    OutputStream por el cual podemos enviar datos en la conexión.
  - Cuando solicita conexión con el control Switch, lanzamos un Thread para que gestione las
    operaciones de conexión y desconexión sin que afecte al hilo principal.
  - Si queremos conectar, desactivamos los controles de ip y puerto, tomamos sus valores e
    intentamos la conexión con el socket.
    Si no lo conseguimos, volveremos a dejar los controles como al principio.
    Si lo conseguimos, enviamos todos los pixels de la imágen.
  - Para que no se interfieran los paquetes enviados creamos una cola para que se envíen
    secuencialemente.
  - El Thread que gestiona la cola queda hibernado mientras la cola esté vacía.
  - El Listener de clicks del objeto View16 es llamado con cada evento... Un cambio de posición de
    las coordenadas, porque se ha arrastrado, hace saltar el evento. Pero el desplazamiento puede
    ser tan pequeño que no supone un cambio de pixel.
    Conclusión: podemos tener llamadas con pulsaciones repetidas.
    Esto es normal. Pero no podemos saturar la comunicación con repeticiones de paquetes.
    Para evitarlo se incluye en la rutina de gestión de la cola de comunicaciones una detección de
    repetición.
  - Puesto que la matriz de leds puede consumir mucho (pero también es muy brillante), en el momento
    del envío, se divide por 8 el nivel de brillo de cada canal.
  Requerimientos:
  - Es necesario indicar que la aplicación necesitará permisos para conectarte con el exterior.
    Esto se hace en el archivo app/manifest/AndroidManifest.xml
    Incluiremos al final del archivo, bajo la sección manifest, y después de la sección application
    la siguiente línea:
      <uses-permission android:name="android.permission.INTERNET" />
*/

package com.rojo.app01

import android.graphics.Color
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.widget.SeekBar
import com.rojo.app01.databinding.ActivityMainBinding
import java.io.OutputStream
import java.net.Socket
import java.util.concurrent.LinkedBlockingQueue

class MainActivity : AppCompatActivity() {
    //Definimos clase para guardar los parámetros de una solicitud de cambio de color
    //No necesitamos métodos. Sólo queremos que los parámetros del constructor sean públicos
    //para poder acceder a ellos.
    //El problema es que pretendemos utilizar la clase como estructura para recoger toda la información
    //necesaria para incluirla en un nodo de la cola de peticiones.
    //Una de las funciones que permite una cola es la comparación. Es capaz de eliminar cierto nodo si
    //descubre que es igual que otro dado. Para ello necesita una función que compare dos valores.
    //En este caso, dos clases.
    //Para cumplir con este requisito (aunque no se utilice) debemos heredar la clase de Comparable
    //con el tipo de objeto a comparar. En este caso sería otra clase igual.
    //Ya que es una clase comparable, tiene la obligación de sobreescribir el método compareTo
    //Lo hacemos, porque lo utilizaremos más adelante para saber si tenemos dos clase iguales.
    inner class ChangeRequest(var row:Int,var col:Int,var color:Int) : Comparable<ChangeRequest> {
        override operator fun compareTo(other: ChangeRequest): Int {
            //Si los componentes de la clase son iguales...respondemos que igual=0
            if(col==other.col && row==other.row && color==other.color) return 0
            //Si los componentes de other son mayores...respondemos -1
            //Si los componentes de other son menores...respondemos 1
            //Como las dos últimas opciones no son lógicas...respondemos 1 si es distinto
            return 1
        }
    }

    //Variables globales
    private lateinit var ui:ActivityMainBinding
    private var selectedColor:Int=Color.BLACK //Color seleccionado. Inicialmente negro
    private var pickColor=false //Hay que recuperar un color?. Inicialmente no.
    private lateinit var client: Socket //Objeto de gestión del Socket
    private lateinit var clientOutputStream: OutputStream //OutputStream del cliente para poder enviar
    private var queue=LinkedBlockingQueue<ChangeRequest>() //Creamos cola de peticiones

    //Creamos clase heredada del Listener de SeekBar
    //Puede acceder a los objetos gráficos a través de la variable global ui
    inner class MySeekBarListener:SeekBar.OnSeekBarChangeListener {
        override fun onProgressChanged(seekBar:SeekBar?,progress:Int,fromUser:Boolean) {
            //Obtenemos los valores de los canales y componemos el color
            selectedColor=Color.rgb(ui.seekBarR.progress,ui.seekBarG.progress,ui.seekBarB.progress)
            //Asignamos el color al objeto gráfico
            ui.viewSelectedColor.setBackgroundColor(selectedColor)
        }
        //El resto de métodos no los utilizaremos, pero su declaración es obligatoria
        override fun onStartTrackingTouch(seekBar:SeekBar?) {}
        override fun onStopTrackingTouch(seekBar:SeekBar?) {}
    }

    override fun onCreate(savedInstanceState:Bundle?) {
        super.onCreate(savedInstanceState)
        ui= ActivityMainBinding.inflate(layoutInflater)
        setContentView(ui.root)

        //Instanciamos un único Listener para todas las SeekBar
        val mySeekBarListener=MySeekBarListener()
        ui.seekBarR.setOnSeekBarChangeListener(mySeekBarListener)
        ui.seekBarG.setOnSeekBarChangeListener(mySeekBarListener)
        ui.seekBarB.setOnSeekBarChangeListener(mySeekBarListener)

        //Definimos objeto para poder añadir tareas al Looper
        val mainHandler=Handler(Looper.getMainLooper()) //Handle de la Activity

        //Definimos función de gestión de cambio del objeto Switch (conexión)
        ui.switchConectar.setOnClickListener {
            Thread { //Creamos un Thread para gestionar la conexión/desconexión
                if(ui.switchConectar.isChecked) { //Si quiere conectar...
                    //Solicitamos la desactivación de los controles de ip & puerto
                    mainHandler.post { //Añadimos la rutina a ejecutar por el Handler
                        ui.editTextIP.isEnabled=false //Control ip desactivado
                        ui.editTextPort.isEnabled=false //Control puerto desactivado
                    }
                    try {
                        //Intentamos conectar. Utilizamos la variable pública client de View8
                        client=Socket(ui.editTextIP.text.toString(),ui.editTextPort.text.toString().toInt())
                        //No ha dado error. Hemos conectado!
                        clientOutputStream=client.getOutputStream() //Obtenemos el OutputStream para enviar
                        //Enviaremos la imágen completa
                        for(row in 0..15) {
                            for(col in 0..15) {
                                //Solicitamos el color del pixel de estas coordenadas
                                //Creamos una instancia de la clase ChangeRequest con las coordenadas
                                //Añadimos la clase a la cola de comunicación
                                queue.add(ChangeRequest(row,col,ui.view8.getPixel(row,col)))
                            }
                        }
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

        //Definimos la función que atenderá los clicks de View8 (Listener de click)
        ui.view8.onClickListener { row, col -> //Nos devuelve las coordenadas de la pulsación
            if(pickColor) { //Si se quiere recuperar el color seleccionado...
                pickColor=false
                val color=ui.view8.getPixel(row,col) //Recuperamos el color el pixelç
                //Lo descomponemos por canales y aplicamos los valores a los SeekBar
                ui.seekBarR.progress=Color.red(color)
                ui.seekBarG.progress=Color.green(color)
                ui.seekBarB.progress=Color.blue(color)
                //Finalmente no asignamos el color al background del viewSelectedColor porque
                //con cada asignación a las SeekBar ya se ha actualizado
            } else { //Se quiere pintar el color seleccionado...
                ui.view8.drawPixel(row,col,selectedColor)
                //Si tenemos conexión...además enviamos el cambio
                if(ui.switchConectar.isChecked) queue.add(ChangeRequest(row,col,selectedColor))
            }
        }

        //Definimos función de gestión de click en el view de selección de color
        ui.viewSelectedColor.setOnClickListener {
            pickColor=true //La próxima vez, recuperaremos el color
        }

        //Creamos el Thread de gestión de cola de peticiones
        Thread {
            val pack=ByteArray(4) //Paquete de datos a enviar
            var lastChangeRequest=ChangeRequest(0,0,0) //Solicitud anterior
            while(true) { //bucle infinito
                val changeRequest=queue.take() //Recibimos una nueva solicitud
                //No enviaremos solicitudes repetidas. Para evitarlo comparamos la solicitud actual
                //con la anterior.
                //Utilizamos el método de comparación que hemos definido en la clase
                //Devuelve 0 si son iguales. 1 si son distintos
                //Si la solicitud recibida es distinta a la anterior...
                if(changeRequest.compareTo(lastChangeRequest)>0) {
                    lastChangeRequest=changeRequest //Ahora la anterior será la actual
                    if(ui.switchConectar.isChecked) { //Si tenemos conexión...
                        //Creamos contenido de paquete
                        pack[0]=(changeRequest.col*16+changeRequest.row).toByte() //Coordenadas xxxxyyyy
                        //Reducimos el brillo dividiendo por 8 cada canal
                        pack[1]=(Color.red(changeRequest.color)/8).toByte() //Canal R
                        pack[2]=(Color.green(changeRequest.color)/8).toByte() //Canal G
                        pack[3]=(Color.blue(changeRequest.color)/8).toByte() //Canal B
                        clientOutputStream.write(pack) //Enviamos paquete
                    }
                }
            }
        }.start()
    }
}