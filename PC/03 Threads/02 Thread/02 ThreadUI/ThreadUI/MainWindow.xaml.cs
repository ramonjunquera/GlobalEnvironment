/*
 * Tema: Thread
 * Fecha: 20210929
 * Autor: Ramón Junquera
 * Objetivo: Acceso a los objetos gráficos desde Thread
 * 
 * Problema:
 * El hilo principal es el único que tiene acceso directo a los objetos gráficos.
 * Un Thread se lanza en un hilo distinto al principal.
 * Por lo tanto, desde un Thread no se puede trabajar con los objetos gráficos.
 * Este es un problema clásico.
 * 
 * Solución:
 * El hilo principal se encarga de atender todos los eventos generados por los objetos gráficos.
 * Cuando termina de gestionar todos estos eventos, atiende un Dispatcher.
 * El Dispatcher es una cola de tareas con trabajos pendientes.
 * Nosotros podemos añadir (invocar) nuevas tareas a esta cola, para que sean atendidas cuando
 * pueda. La gran ventaja es que:
 * - El Dispatche se ejecuta siempre en el hilo principal y puede interactuar con los objetos gráficos.
 * - Se pueden añadir (invocar) nuevas tareas al Dispatcher desde cualquier hilo.
 * 
 * Ejemplo:
 * La aplicación sólo tiene un botón y un Label.
 * Cuando se pulsa el bótón, se lanza un Thread que espera durante 3 segundos y después actualiza
 * el Label.
 * 
 * Funcionamiento:
 * Se pueden lanzar varios Threads al mismo tiempo.
 * Si lanzamos continuamente un Thread cada segundo, comprobaremos que según van finalizando, actualizan
 * el Label.
 */

using System;
using System.Threading;
using System.Windows;

namespace ThreadUI {
  /// <summary>
  /// Lógica de interacción para MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window {
    public MainWindow() {
      InitializeComponent();
    }

    private void UpdateLastTime() {
      Thread.Sleep(3000); //Esperamos 3 segundos
      Dispatcher.Invoke(() => { //Añadimos (invocamos) a la cola de tareas del hilo principal (Dispatcher)
        LabelInfo.Content = DateTime.Now.ToLongTimeString(); //El Label mostrará la hora actual larga (con segundos)
      });
    }

    private void ButtonLaunch_Click(object sender, RoutedEventArgs e) {
      //Creamos una nueva instancia de un Thread que llame al método UpdateLastTime y a
      //continuación lo lanzamos.
      new Thread(UpdateLastTime).Start();
    }
  }
}
