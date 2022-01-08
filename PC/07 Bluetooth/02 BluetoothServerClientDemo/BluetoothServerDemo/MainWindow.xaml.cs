//Servidor

using InTheHand.Net.Bluetooth;
using InTheHand.Net.Sockets;
using System;
using System.ComponentModel;
using System.Net.Sockets;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace BluetoothServerClientDemo {
  /// <summary>
  /// Lógica de interacción para MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window {
    //Variables globales
    private readonly Guid _serviceId = new Guid("29913A2D-EB93-40Cf-1234-DEEE26477667");
    private bool requestCloseApplication = false; //Se quiere cerrar la aplicación
    private string textBoxInfo = ""; //Copia local de TextBoxInfo.Text para poder ser accedida desde Thread

    private void Window_Closing(object sender, CancelEventArgs e) {
      requestCloseApplication = true;
    }

    private void TextBoxInfo_TextChanged(object sender, TextChangedEventArgs e) {
      textBoxInfo = TextBoxInfo.Text; //Activamos flag para forzar desconexión
      //No necesitamos Environment.Exit(0) para finalizar la aplicación. Ya estamos en proceso de Closing
    }

    protected void ConnWorker_ProgressChanged(object sender, ProgressChangedEventArgs e) {
      switch (e.ProgressPercentage) {
        case 1: //Seleccionar opción 1
          RadioButton1.IsChecked = true;
          break;
        case 2: //Seleccionar opción 2
          RadioButton2.IsChecked = true;
          break;
        case 3: //Seleccionar opción 3
          RadioButton3.IsChecked = true;
          break;
        case 4: //Semáforo amarillo
          RectangleSemaphore.Fill = new SolidColorBrush(System.Windows.Media.Colors.Yellow);
          break;
        case 5: //Semáforo verde
          RectangleSemaphore.Fill = new SolidColorBrush(System.Windows.Media.Colors.Green);
          break;
        case 6: //Actualiza TextBoxInfo
          TextBoxInfo.Text = e.UserState.ToString();
          break;
      }
    }

    protected void ConnWorker_DoWork(object sender, DoWorkEventArgs e) {
      BackgroundWorker thisWorker = (BackgroundWorker)sender;
      BluetoothListener listener = new BluetoothListener(_serviceId); //Instanciamos un nuevo listener para nuestro servicio de Bluetooth
      BluetoothClient client = null;
      NetworkStream stream = null;

      try {
        listener.Start(); //Intentamos arrancarlo. Si no hay Bluetooth o está desactivado se generará una excepción
      } catch {  //No hay Bluetooth o está desactivado
        return; //Hemos terminado
      }
      BluetoothRadio radio = BluetoothRadio.Default;
      thisWorker.ReportProgress(6, radio.LocalAddress); //TextBoxInfo
      thisWorker.ReportProgress(4); //Semáforo en amarillo

      while (true) { //Bucle infinito
        if (client == null) { //Si no estamos conectamos...
          if (listener.Pending()) { //Si tenemos alguna solicitud de conexión pendiente por atender...
            //Recogemos la posible petición de conexión
            //El método AcceptBluetoothClient es peligroso porque detiene el flujo a la espera de un nuevo cliente
            //Sólo debe usarse cuando tengamos la certeza de que hay una solicitud de conexión pendiente
            client = listener.AcceptBluetoothClient(); //Obtenemos la solicitud
            stream = client.GetStream(); //Creamos un Stream para la comunicación
            thisWorker.ReportProgress(5); //Semáforo en verde
          }
        } else { //Si ya estamos conectados...
          if (stream.DataAvailable) { //Si hay datos pendientes por recibir...
            int RxByte = stream.ReadByte(); //Leemos el siguiente byte
            switch (RxByte) {
              case 1: //Seleccionar opción 1
              case 2: //Seleccionar opción 2
              case 3: //Seleccionar opción 3
                thisWorker.ReportProgress(RxByte); //Actualiza opción
                break;
              case 4: //Solicitan contenido de TextBoxInfo
                //Codificamos el texto a enviar con UTF8 en un array de bytes para poder enviarlo por stream
                byte[] textByteArray = Encoding.UTF8.GetBytes(textBoxInfo);
                stream.Write(textByteArray, 0, textByteArray.Length);
                stream.WriteByte(10); //Fin de línea "\n"
                stream.Flush();
                break;
              case 127: //Fin de conexión
                stream.Close();
                client.Close();
                client = null; //Ya no tenemos conexión
                thisWorker.ReportProgress(4); //Semáforo en amarillo
                break;
            }
          }
        }
        if (requestCloseApplication) { //Si nos piden cerrar la aplicación...
          if (client != null) { //Si estamos conectamos...
            stream.WriteByte(127); //Carácter especial para informar desconexión
            stream.Flush();
            stream.Close();
            client.Close();
            client = null; //Ya no tenemos conexión
          }
        }
      }
    }

    public MainWindow() {
      InitializeComponent();

      //Nueva instancia para el Thread que gestionará las comunicaciones Bluetooth
      BackgroundWorker connWorker = new BackgroundWorker();
      //Añadimos a nuestra instancia la función principal del Thread
      connWorker.DoWork += new DoWorkEventHandler(ConnWorker_DoWork);
      //Indicamos que esta instancia admite el seguimiento de progreso
      connWorker.WorkerReportsProgress = true;
      //Indicamos la función que se debe ejecutar cuando informe del progreso
      connWorker.ProgressChanged += new ProgressChangedEventHandler(ConnWorker_ProgressChanged);
      connWorker.RunWorkerAsync(); //Lanzamos el Thread
    }

    

    
  }
}
