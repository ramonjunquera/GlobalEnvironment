/*
 * Tema: Thread
 * Fecha: 20210929
 * Autor: Ramón Junquera
 * Objetivo: Interactuar con Thread
 * 
 * Problema:
 * Supongamos que nuestra aplicación genera varias instancias de un mismo Thread.
 * Eso significa que cada Thread tendrá sus propios parámetros.
 * A partir de ese momento perdemos el control de la ejecución del Thread.
 * No podemos enviarle órdenes para que las ejecute en backgroud.
 * 
 * Solución 1:
 * Podemos lanzar el Thread con un parámetro.
 * El parámetro será una estructura tan compleja como necesitemos.
 * Lo importante es que pasaremos este parámetro como referencia y no por valor (por defecto).
 * Esto significa que mientras mantengamos la instancia de la estructura en el hilo principal,
 * podremos cambiar los valores de sus campos, que serán accedidos al instante por el Thread.
 * 
 * Solución 2:
 * Si hacemos que el Thread gestione una cola concurrente (ConcurrentQueue), podemos pasarle
 * por ahí las estructuras de parámetros de cada tarea a realizar.
 * 
 * Ejemplo:
 * En este ejemplo tratamos la solución 1.
 * La estructura de parámetros (MyParams) es muy sencilla. Sólo tiene un campo (value).
 * Creamos una variable global con esta estructura (globalParams), para poder cambiar el
 * valor de sus campos desde el hilo principal.
 * Al iniciar la aplicación se instancia un Thread y se le pasa por referencia la estructura
 * global de parámetros (globalParams).
 * El Thread recibe la estructura de parámetros con el nombre de localParams.
 * El Thread actualiza cada segundo la hora local en un Label, incluyendo como prefijo el
 * campo value de la estructura de parámetros que se le ha pàsado por referencia.
 * Es importante recalcar que las tareas que añadimos al Dispatcher deben ser autocontenidas
 * (no pueden referenciar a valores externos). Esta es la razón por la que extraemos el contenido
 * del campo value a una variable local.
 * Mientras se ejecuta el Thread, desde el hilo principal somos capaces de alterar el campo value
 * de la estructura global de parámetros. Estas modificación son compartidas con los parámetros
 * del Thread porque se ha pasado por referencia.
 * También se define como global la variable del Thread, para poder abortarlo antes de cerrar la 
 * aplicación.
 * 
 * Funcionamiento:
 * Al actualizar el valor desde la ventana, se modifica también el valor con el que trabaja el Thread
 * y lo refleja en el Label.
 */

using System;
using System.Threading;
using System.Windows;

namespace ThreadUI {
  /// <summary>
  /// Lógica de interacción para MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window {
    private struct MyParams { //Estructura en la que guardaremos los parámetros del Thread
      public string value;
    };
    MyParams globalParams; //Instanciamos una estructura de parámetros como variable global
    Thread myThread;
    public MainWindow() {
      InitializeComponent();

      //Instanciamos el Thread pasando la estructura de parámetros como referencia
      myThread = new Thread(() => MyThread(ref globalParams));
      myThread.Start(); //Lo lanzamos
    }

    //Función de Thread
    //Tiene un parámetro con una estructura de parámetros que se pasa como referencia
    private void MyThread(ref MyParams localParams) { 
      while (true) { //Bucle infinito
        //Copiamos el campo que nos interesa a una variable local.
        //Lo hacemos así porque no podemos entregar una variable por referencia a una tarea
        //que añadimos al Dispatcher. Las tareas no deben depender de nada.
        string localParam = localParams.value;
        //Simplemente actualizamos la hora en el Label cada segundo con el prefijo del parámetro
        Dispatcher.Invoke(() => { //Añadimos (invocamos) a la cola de tareas del hilo principal (Dispatcher)
          LabelInfo.Content = localParam + " " + DateTime.Now.ToLongTimeString();
        });
        Thread.Sleep(1000);
      }
    }

    private void ButtonUpdate_Click(object sender, RoutedEventArgs e) {
      //Actualizamos el campo value de la estructura de parámetros mantenida en la variable
      //global globalParams
      globalParams.value = TextBoxValue.Text;
    }

    private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e) {
      //Se ha solicitado el cierre de la aplicación
      //Abortamos el thread para que no se quede huérfano y produzca errores
      myThread.Abort();
    }
  }
}
