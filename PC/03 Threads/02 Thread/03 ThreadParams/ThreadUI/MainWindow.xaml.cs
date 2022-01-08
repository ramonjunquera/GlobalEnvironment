/*
 * Tema: Thread
 * Fecha: 20210929
 * Autor: Ramón Junquera
 * Objetivo: Thread con parámetros
 * 
 * Problema:
 * Un Thread puede tener acceso a variables globales y trabajar con ellas.
 * Pero si instanciamos más de un Thread del mismo tipo, compartirán variables globales.
 * No hay manera de asignarles parámetros específicos.
 *
 * Solución:
 * La función que lanmzaremos como Thread puede tener parámetros propios.
 * La llamada al Thread varía ligeramente.
 * Sin parámetros, simplemente hacemos referencia al método del Thread.
 * Ahora definiremos una funión anónima () => que llama al método del Thread con sus
 * correspondientes parámetros.
 * 
 * Ejemplo:
 * La aplicación sólo tiene un TextBox, un botón y un Label.
 * Cuando se pulsa el bótón, se convierte el texto del textBox a numérico y se le pasa como parámetro
 * al Thread, que lo único que hace es multiplicar el valor por 2 y mostrar elñ resultado en el Label.
 * 
 * Nota:
 * No se hace ningún control de la conversión de texto a entero. Si el texto no tuviese formato
 * numérico, produciría una excepción.
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

    private void ThreadX2(int value) {
      this.Dispatcher.Invoke(() => { //Añadimos (invocamos) a la cola de tareas del hilo principal (Dispatcher)
        LabelInfo.Content = (value * 2).ToString(); //El Label mostrará el doble del vlor del parámetro
      });
    }
    private void ButtonLaunch_Click(object sender, RoutedEventArgs e) {
      //Convertimos el texto del TextBox a entero
      int value = Convert.ToInt32(TextBoxValue.Text);
      //Creamos una nueva instancia de un Thread.
      //El Thread se define como una función que llama al método con un parámetro.
      //A continuación lo lanzamos.
      new Thread(() => ThreadX2(value)).Start();
    }
  }
}
