using System;
using System.ComponentModel;
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
      //Indicamos que esta instancia admite la cancelación
      myWorker.WorkerSupportsCancellation = true;
      //Indicamos la función que se debe ejecutar cuando se complete el proceso
      myWorker.RunWorkerCompleted += new RunWorkerCompletedEventHandler(myWorker_RunWorkerCompleted);
      //Indicamos que esta instancia admite el seguimiento de progreso
      myWorker.WorkerReportsProgress = true;
      //Indicamos la función que se debe ejecutar cuando informe del progreso
      myWorker.ProgressChanged += new ProgressChangedEventHandler(myWorker_ProgressChanged);
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
      //El valor máximo será el máximo de la barra de progreso
      toolStripProgressBar1.Maximum = maxValue;
      //reseteamos la barra de progreso (la ponemos a cero)
      toolStripProgressBar1.Value = 0;

      //Si se ha pulsado el botón de start es porque no se está ejecutando ningún proceso ahora mismo
      //No necesitamos comprobarlo
      //Desactivamos el botón de start para que no se lanze otro mientras este no termine
      toolStripButton1.Enabled = false;
      //Lanzamos un nuevo proceso
      //Siempre tiene un único parámetro que es el que le llega a la función como e.Argument
      myWorker.RunWorkerAsync(maxValue);
      //Activamos el botón de cancelar
      toolStripButton2.Enabled = true;
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
      //Si tenemos que pasar más de un parámetro tendremos que crear una clase, o utilizar un array de 
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
        //Si tenemos pendiente una solicitud de cancelación...
        if (sendingWorker.CancellationPending) {
          //Indicamos que la ejecución se ha cancelado
          e.Cancel = true;
          //Hemos terminado
          return;
        }
        //Informamos de un cambio en el progreso
        //Siempre es un número entero (int=int32) no se puede elegir
        sendingWorker.ReportProgress(i);
      }
      //Hemos finalizado nuestro proceso en background

      //Anotamos en la variable e.Result la cadena resultante para que se pueda mostrar
      e.Result = concatenatedString;
    }
    protected void myWorker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e) {
      //Función que se llama al terminar el proceso en background

      //Desactivamos el botón de cancelar
      toolStripButton2.Enabled = false;
      //Activamos el botón de start
      toolStripButton1.Enabled = true;

      //Si se ha cancelado...
      if (e.Cancelled) {
        //Indicamos que se ha cancelado
        //No podemos mostrar lo que se había hecho hasta el momento a través de la variable e.Result
        //porque es incompatible con la cancelación
        //Si se activa el flag de cancelación, aunque hayas asignado valor a e.Result, siempre
        //dará un error al intentar recuperarlo.
        textBox1.Text = "Proceso cancelado por el usuario";
        return;
      }
      //No se ha cancelado

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
    protected void myWorker_ProgressChanged(object sender, ProgressChangedEventArgs e) {
      //Función que se llama al informar de un cambio en el progreso

      //Asignamos a la barra de progreso el valor que me indican como nuego progreso
      toolStripProgressBar1.Value = e.ProgressPercentage;
    }
    private void toolStripButton2_Click(object sender, EventArgs e) {
      //Pulsado el botón de cancelación

      //Activamos el flag de cancelación para el proceso de background
      myWorker.CancelAsync();
    }
  }
}
