/*
 * Tema: Interlocked
 * Fecha: 20210907
 * Autor: Ramón Junquera
 * 
 * Descripción:
 * Interlocked es una clase que nos permite trabajar con valores enteros con varios Threads.
 * Nos facilita su lectura, asignación de valor, incremento, decremento, suma, etc.
 * La variable no necesita ser declarada de una manera especial. Simplemente debemos utilziarla a través
 * de la clase Interlocked.
 * 
 * Funcionamiento:
 * La aplicación lanzará varios Threads simultáneamente.
 * Los Threads se indentificarán con un número secuenciual.
 * Cada Thread decide su propia duraci´çon de manera aleatoria.
 * El número de Thread puede ser definido por el usuario a través de un Slider.
 * Se muestra en pantalla tanto el log de inicio y final de las ejecuciones como el número de Threads en
 * ejecución en tiempo real.
 * El número de Threads en ejecución es una variable global int.
 * Cuando un Thread finaliza, decrementa su valor.
 * Para evitar tener problemas al hacer esta operación, usaremos Interlocked.
 * 
 * Nota:
 * - El Slider (SliderThreadLaunched) no tiene el valor por defecto (0) al iniciar, porque su valor mínimo es 2.
 *   Por esta razón, en la inicialización del Slider se actualiza su valor, que hace saltar la llamada a la
 *   función de actualización. En esta función se muestra el valor del Slider en el LabelThreadLaunched.
 *   Es importante que LabelThreadLaunched se defina en el apartado gráfico antes que el LabelThreadLaunched.
 *   Si no se hace así, cuando se inicialice el Slider y se llama a la función de actualización, no detectará
 *   el objeto del Label y dará un error en tiemmpo de ejecución.
 */

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace InterlockedDemo {
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window {
    //Definición de variables globales
    int threadRunning = 0;
    Random rnd = new Random();
    public MainWindow() {
      InitializeComponent();
    }

    private void SliderThreadLaunched_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e) {
      //Al cambiar el Slider, cambiamos también el valor del Label
      LabelThreadLaunched.Content = e.NewValue.ToString();
    }
    protected void ThreadMain(object sender, DoWorkEventArgs e) {
      BackgroundWorker currentThread = (BackgroundWorker)sender; //Anotamos el objeto del Thread actual
      int threadId = (int)e.Argument; //Anotamos el identificador del Thread
      int sleepTime = rnd.Next(1000, 5000); //Calculamos un tiempo de trabajo aleatorio
      //Informamos que el Thread ha iniciado y cuanto durará
      currentThread.ReportProgress(0, "Thread " + threadId.ToString() + " iniciado con " + sleepTime.ToString() +"ms");
      Thread.Sleep(sleepTime); //A trabajar!
      //Informamos de identificador de Thread para que la función que gestiona la finalización lo pueda identificar
      e.Result = threadId; 
    }

    protected void ThreadCompleted(object sender, RunWorkerCompletedEventArgs e) {
      //Ha finalizado otro Thread
      //Informamos en el TextBox
      TextBoxInfo.Text += "Thread" + e.Result.ToString() + " finalizado\n";
      //Decrementamos el número de Threads en ejecución con Interlocked para evitar problemas por actualizaciones
      //simultáneas de varios Threads y tomamos el nuevo valor decrementado
      int currentThreadRunning = Interlocked.Decrement(ref threadRunning);
      //Actualizamos el Label que muestra el número de Threads en ejecución
      LabelThreadRunning.Content = currentThreadRunning.ToString();
      //Si ya no hay Thread en ejecución...readctivamos botón de start
      if (currentThreadRunning == 0) ButtonStart.IsEnabled = true; 
    }

    protected void ThreadProgress(object sender, ProgressChangedEventArgs e) {
      //Simplemente mostramos el mensaje anaiéndolo a TextBox
      TextBoxInfo.Text += e.UserState.ToString() + "\n";
    }

    private void ButtonStart_Click(object sender, RoutedEventArgs e) { //Botón start pulsado
      ButtonStart.IsEnabled = false; //Lo desactivamos para evitar que se vuelva a lanzar
      TextBoxInfo.Text = ""; //Limpiamos la info del TextBox
      int threadCount = Convert.ToInt32(SliderThreadLaunched.Value); //Número de Threads a lanzar
      threadRunning = threadCount; //Anotamos el número de Threads en ejecución
      LabelThreadRunning.Content = threadCount.ToString(); //Mostramos el número de Threads en ejecución
      for (int t=0;t<threadCount;t++) { //Recorremos todos los Threads
        BackgroundWorker myWorker = new(); //Creamos un nuevo Thread
        myWorker.DoWork += new DoWorkEventHandler(ThreadMain); //Función principal del Thread
        myWorker.RunWorkerCompleted += new RunWorkerCompletedEventHandler(ThreadCompleted); //Función de finalización
        myWorker.ProgressChanged += new ProgressChangedEventHandler(ThreadProgress); //Función de actualziación de progreso
        myWorker.WorkerReportsProgress = true; //Se pèrmite informar del progreso
        myWorker.RunWorkerAsync(t+1); //Ejecutamos el Thread y le indicamos su identificador
      }
    }
  }
}
