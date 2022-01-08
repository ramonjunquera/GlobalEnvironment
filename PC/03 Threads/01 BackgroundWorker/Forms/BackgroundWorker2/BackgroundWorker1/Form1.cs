using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace BackgroundWorker1 {
  public partial class Form1 : Form {
    //Variables globales de clase
    //Creamos una instancia de la clase BackgroundWorker que nos permitirá lanzar un proceso en background
    private BackgroundWorker myWorker = new BackgroundWorker();

    public Form1() {
      InitializeComponent();

      //Añadimos a nuestra instancia una nueva función para lanzarla en background
      myWorker.DoWork += new DoWorkEventHandler(myWorker_DoWork);
      //Indicamos la función que se debe ejecutar cuando se complete el proceso
      myWorker.RunWorkerCompleted += new RunWorkerCompletedEventHandler(myWorker_RunWorkerCompleted);
    }
    private void trackBar1_ValueChanged(object sender, EventArgs e) {
      //Cambiado el valor del primer TrackBar

      //Le asignamos el mismo valor al segundo TrackBar
      trackBar2.Value = trackBar1.Value;
    }
    private void toolStripButton1_Click(object sender, EventArgs e) {
      //Pulsado el botón start

      //Tomamos nota del valor máximo
      int maxValue = Convert.ToInt32(toolStripTextBox1.Text);
      //Si no tiene ningún proceso en brackground corriendo...
      if (!myWorker.IsBusy) {
        //...lo lanzamos.
        //Siempre tiene un único parámetro que es el que le llega a la función como e.Argument
        myWorker.RunWorkerAsync(maxValue);
        //Informamos
        MessageBox.Show("Proceso lanzado");
      } else {
        MessageBox.Show("Aun está siendo ejecutado el proceso anterior");
      }
    }
    protected void myWorker_DoWork(object sender, DoWorkEventArgs e) {
      //Función que se lanza en background 
      //Es obligatorio que sea protected
      //No puede devolver nada
      //Sólo admite los dos parámetros indicados

      //Tomamos nota de la instacia de BackgroundWorker que ha lanzado la ejecución
      BackgroundWorker sendingWorker = (BackgroundWorker)sender;
      //Tomamos nota del máximo número a contar
      //Sabemos que es un número entero
      //La única manera de pasar parámetros a un función que se lanza en backgroud es a través de e.Argument
      //Si tenemos que pasar más de un parámetro tendremos que crear una clase, outilizar un array de 
      //objetos object[]
      int maxValue = (int)e.Argument;
      //Variable que contendrá la cadena resultante. Inicialmente vacía
      string concatenatedString = "";
      //Comenzamos a ejecutar el proceso pesado
      //Recorremos todos los números desde el 1 hasta el máximo
      for (int i = 1; i <= maxValue; i++) {
        //Añadimos el número a la cadena resultante seguido de un espacio
        concatenatedString += i.ToString() + " ";
        //Esperamos un segundo
        System.Threading.Thread.Sleep(1000);
      }
      //Hemos finalizado nuestro proceso en background

      //Anotamos en la variable e.Result la cadena resultante para que se pueda mostrar
      e.Result = concatenatedString;
    }
    protected void myWorker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e) {
      //Función que se llama al terminar el proceso en background

      //Si no hay errores...
      if (e.Error == null) {
        //Mostramos que todo ha ido bien y el resultado
        textBox1.Text = "Finalización correcta!" + Environment.NewLine + e.Result.ToString();
        return;
      }
      //Tenemos errores

      //Informamos del error
      textBox1.Text = "Finalizado con errores.";
    }



  }
}
