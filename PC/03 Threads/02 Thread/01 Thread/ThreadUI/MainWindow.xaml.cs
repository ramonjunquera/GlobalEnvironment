/*
 * Tema: Thread
 * Fecha: 20210929
 * Autor: Ramón Junquera
 * Objetivo: Lanzar un simple thread
 * 
 * Descripción:
 * Podemos lanzar un Thread a través de un botón.
 * El tread espera 3 segundos y después actualiza el valor de la variable global lastTime con
 * la hora actual.
 * Un segundo botón nos permite refrescar el Label con el valor de la variable lastTime.
 * 
 * Funcionamiento:
 * Cuando lanzamos el Thread, el UI no queda afectado.
 * Comprobamos que aunque pulsemos el botón de refresco, el Label no cambia hasta que el
 * Thread no termina su ejecución actualizando la variable global.
 */

using System;
using System.Threading;
using System.Windows;

namespace ThreadUI {
  /// <summary>
  /// Lógica de interacción para MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window {
    string lastTime = "";
    public MainWindow() {
      InitializeComponent();
    }

    private void ButtonRefresh_Click(object sender, RoutedEventArgs e) {
      LabelInfo.Content = lastTime;
    }

    private void UpdateLastTime() {
      Thread.Sleep(3000); //Esperamos 3 segundos
      lastTime = DateTime.Now.ToLongTimeString(); //Hora actual larga (con segundos)
    }

    private void ButtonLaunch_Click(object sender, RoutedEventArgs e) {
      //Creamos una nueva instancia de un Thread que llame al método UpdateLastTime y a
      //continuación lo lanzamos.
      new Thread(UpdateLastTime).Start();
    }
  }
}
