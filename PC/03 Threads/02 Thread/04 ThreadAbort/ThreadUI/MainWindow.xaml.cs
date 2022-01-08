/*
 * Tema: Thread
 * Fecha: 20210929
 * Autor: Ramón Junquera
 * Objetivo: Thread con parámetros
 * 
 * Problema:
 * Es normal incluir en un Thread un bucle infinito para atender una serie de eventos asíncronos.
 * El problema es que si queremos cerrar la aplicación, necesitamos cerrar todos los Threads
 * lanzados. O porque finalizan o porque lo cerramos a la fuerza (abortamos).
 * Si no lo hacemos, la aplicación seguirá en ejecución.
 * 
 * Solución:
 * Para poder abortar un Thread, necesitamos tener su referencia.
 * 
 * Ejemplo:
 * Tenemos un bótón que lanza un Thread con un bucle infinito que actualiza la hora en un Label.
 * Tenemos otro botón que es capaz de cerrarlo.
 * Para evitar lanzar más de una vez el Thread se juega con la activación y desactivación de los
 * botones de lanzado y cerrado.
 * 
 * Funcionamiento:
 * Al pulsar el botón de lanzar el Thread comprobamos que funciona porque se actualiza el contenido
 * del Label.
 * El cerrar el Thread finaliza la actualización.
 * Podemos comprobar que si lanzamos el Thread y cerramos la aplicación, se producirá una excepción.
 * La razón es que se cierra el hilo principal que mantenía los objetos gráficos, pero el Thread
 * pretende seguir añadiendo tareas a la cola del Dispatcher, que ya no existe.
 * La solución es sencilla. Se debe crear un método para el evento Closing de la ventana que se
 * encargue de abortar el Thread.
 */

using System;
using System.Threading;
using System.Windows;

namespace ThreadUI {
  /// <summary>
  /// Lógica de interacción para MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window {
    Thread myThread;
    public MainWindow() {
      InitializeComponent();
    }

    private void ClockThread() {
      while(true) { //Bucle infinito
        //Simplemente actualizamos la hora en el Label cada segundo
        Dispatcher.Invoke(() => { //Añadimos (invocamos) a la cola de tareas del hilo principal (Dispatcher)
          LabelInfo.Content = DateTime.Now.ToLongTimeString();
        });
        Thread.Sleep(1000);
      }
    }
    private void ButtonLaunch_Click(object sender, RoutedEventArgs e) {
      ButtonLaunch.IsEnabled = false;
      ButtonKill.IsEnabled = true;
      //Creamos una nueva instancia de un Thread.
      //El Thread llamará a un método
      myThread = new Thread(ClockThread);
      myThread.Start(); //Lo lanzamos
    }

    private void ButtonKill_Click(object sender, RoutedEventArgs e) {
      myThread.Abort(); //Cancelamos el Thread
      ButtonKill.IsEnabled = false;
      ButtonLaunch.IsEnabled = true;
    }
  }
}
