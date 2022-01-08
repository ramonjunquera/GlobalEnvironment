/*
 * Tema: Demo de clase ConcurrentQueue
 * Fecha: 20210907
 * Autor: Ramón Junquera
 * 
 * Descripción:
 * ConcurrentQueue es una cola FIFO de tipos configurables especialmente creada para
 * trabajar en multitarea.
 * Añadir, retirar o mirar un nuevo elemento no supone un problema cuando se trabajan
 * con varios Threads.
 * 
 * Funcionamiento:
 * La aplicación crear utiliza una ConcurrentQueue para alimentar a un Thread.
 * El Thread procesa un elemento por segundo.
 * Los botones de letras A B C D añaden un nuevo elemento a la cola.
 * El último contol muestra el número de elementos pendientes de procesar.
 * En la línea inferior se van acumulando los elementos procesados.
 * Se añade un botón para limpiar el texto de elementos procesados.
 * Aunque añadamos elementos más rápido de que lo que están procesando, quedarán encolados.
 */

using System.Collections.Concurrent;
using System.ComponentModel;
using System.Threading;
using System.Windows;

namespace ConcurrentQueueDemo {
  /// <summary>
  /// Lógica de interacción para MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window {
    //Definición de variables globales
    private readonly ConcurrentQueue<string> q = new ConcurrentQueue<string>();

    protected void MyWorkerMain(object sender, DoWorkEventArgs e) {
      BackgroundWorker currentWorker = (BackgroundWorker)sender;
      string s;
      while (true) { //Bucle infinito
        if (q.TryDequeue(out s)) { //Si conseguimos obtener un nuevo elemento...
          //Actualizamos el progreso con el número de elementos pendientes por procesar
          //y el texto que procesa
          currentWorker.ReportProgress(q.Count,s);
        }
        Thread.Sleep(1000);
      }
    }

    protected void MyWorkerProgress(object sender, ProgressChangedEventArgs e) {
      labelPending.Content = e.ProgressPercentage; //Actualizamos el número de elementos pendientes de procesar
      labelProcessed.Content += e.UserState.ToString(); //Añadimos el texto del elemento procesado
    }

    public MainWindow() {
      InitializeComponent();
      BackgroundWorker myWorker = new BackgroundWorker();
      myWorker.DoWork += new DoWorkEventHandler(MyWorkerMain);
      myWorker.ProgressChanged += new ProgressChangedEventHandler(MyWorkerProgress);
      myWorker.WorkerReportsProgress = true;
      myWorker.RunWorkerAsync(); //Ejecutamos el Thread
    }

    private void ButtonA_Click(object sender, RoutedEventArgs e) {
      q.Enqueue("A");
    }

    private void ButtonB_Click(object sender, RoutedEventArgs e) {
      q.Enqueue("B");
    }

    private void ButtonC_Click(object sender, RoutedEventArgs e) {
      q.Enqueue("C");
    }

    private void ButtonD_Click(object sender, RoutedEventArgs e) {
      q.Enqueue("D");
    }

    private void ButtonClear_Click(object sender, RoutedEventArgs e) {
      labelProcessed.Content = "";
    }
  }
}
