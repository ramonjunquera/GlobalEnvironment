using System;
using System.ComponentModel;
using System.Threading;
using System.Windows;
using System.Windows.Controls;

namespace ThreadDemo {
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window {
    //Definición de variables globales
    private readonly BackgroundWorker myWorker = new(); //Objeto de gestión del Thread

    //Función principal del Thread
    protected void MyWorkerMain(object sender, DoWorkEventArgs e) {
      BackgroundWorker currentWorker = (BackgroundWorker)sender;
      int secMax = (int)e.Argument;
      for (int sec = 1; sec <= secMax; sec++) {
        Thread.Sleep(1000);
        if(currentWorker.CancellationPending) {
          e.Cancel = true;
          return;
        }
        currentWorker.ReportProgress(sec);
      }
      //Hemos terminado
      e.Result = "Thread finalizado"; //Informamos del resultado del cálculo
    }

    //Función de finalización del Thread
    protected void MyWorkerCompleted(object sender, RunWorkerCompletedEventArgs e) {
      //Hemos terminado la ejecución del Thread
      //Informamos si se ha cancelado
      //Si no se ha cancelado, informamos el del resultado generado por el Thread
      labelInfo.Content = e.Cancelled ? "Thread cancelado" : e.Result.ToString();
      //Nota. Ante una cancelación, el resultado no queda disponible (e.Result)
      buttonStart.IsEnabled = true;
      buttonCancel.IsEnabled = false;
    }

    //Función de actualización de estado del Thread
    protected void MyWorkerProgress(object sender, ProgressChangedEventArgs e) {
      progressBar.Value = e.ProgressPercentage;
    }

    private void ButtonCancel_Click(object sender, RoutedEventArgs e) {
      myWorker.CancelAsync();
    }

    public MainWindow() {
      InitializeComponent();
      //Inicializamos el Thread con las funciones principal, de finalización y actualización.
      myWorker.DoWork += new DoWorkEventHandler(MyWorkerMain);
      myWorker.RunWorkerCompleted += new RunWorkerCompletedEventHandler(MyWorkerCompleted);
      myWorker.WorkerSupportsCancellation = true;
      myWorker.ProgressChanged += new ProgressChangedEventHandler(MyWorkerProgress);
      myWorker.WorkerReportsProgress = true;
    }

    //Pulsado el botón start
    private void ButtonStart_Click(object sender, RoutedEventArgs e) {
      buttonStart.IsEnabled = false; //Desactivamos el botón start para que no se pulse hasta que termine el Thread
      labelInfo.Content = "Thread lanzado"; //Informamos
      int maxValue = Convert.ToInt32(textBoxSecs.Text); //Obtenemos el número de segundos a esperar
      progressBar.Value = 0;
      progressBar.Maximum = maxValue;
      myWorker.RunWorkerAsync(maxValue); //Ejecutamos el Thread
      buttonCancel.IsEnabled = true; //Ya se puede pulsar el botón de cancelar
    }

    //Función de gestión del comportamiento de los Slider
    private void Slider1_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e) {
      //Asignamos al segundo slider el valor del primero
      //El objeto del primer slider se entrega en el parámetro sender. No es necesario asignarle un identificador.
      slider2.Value = ((Slider)sender).Value;
      //Esto lo hacemos para comprobar que el lanzamiento de un Thread no afecta a la ejecución del hilo principal
    }
  }
}
