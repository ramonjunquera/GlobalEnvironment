//Cliente

using InTheHand.Net;
using InTheHand.Net.Sockets;
using System;
using System.Collections.Concurrent;
using System.ComponentModel;
using System.Net.Sockets;
using System.Text;
using System.Windows;

namespace BluetoothClientDemo {
  /// <summary>
  /// Lógica de interacción para MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window {
    //Estructuras
    struct ActionRequest {
      public byte id;
      public string text;
    }

    //Variables globales
    private readonly Guid _serviceId = new Guid("29913A2D-EB93-40Cf-1234-DEEE26477667");
    private readonly ConcurrentQueue<ActionRequest> actionQueue = new ConcurrentQueue<ActionRequest>();

    private void SendAction(byte id, string text = "") {
      ActionRequest actionRequest = new ActionRequest(); //Nueva instacia de solicitud de acción
      actionRequest.id = id; //Identificador de acción
      actionRequest.text = text; //Texto de la acción
      actionQueue.Enqueue(actionRequest); //Añadimos a la cola de acciones
    }

    private void ButtonConnect_Click(object sender, RoutedEventArgs e) {
      ButtonConnect.IsEnabled = TextBoxDeviceId.IsEnabled = false;
      SendAction(5, TextBoxDeviceId.Text); //Solicitud de conexión
    }
    private void ButtonDisconnect_Click(object sender, RoutedEventArgs e) {
      //Desactivamos ButtonDisconnect y resto de controles inferiores
      ButtonDisconnect.IsEnabled = GroupBoxOptions.IsEnabled = TextBoxInfo.IsEnabled = ButtonRequestText.IsEnabled = false;
      SendAction(6); //Solicitud de desconexión
    }

    private void Window_Closing(object sender, CancelEventArgs e) {
      SendAction(6); //Solicitud de desconexión
      //No hacemos nada más. El proceso de finalización de aplicación sigue en curso.
    }

    private void RadioButton1_Checked(object sender, RoutedEventArgs e) {
      SendAction(1); //Seleccionada opción 1
    }

    private void RadioButton2_Checked(object sender, RoutedEventArgs e) {
      SendAction(2); //Seleccionada opción 2
    }

    private void RadioButton3_Checked(object sender, RoutedEventArgs e) {
      SendAction(3); //Seleccionada opción 3
    }

    private void ButtonRequestText_Click(object sender, RoutedEventArgs e) {
      SendAction(4); //Solicitud de TextBoxInfo remoto
    }

    protected void ConnWorker_ProgressChanged(object sender, ProgressChangedEventArgs e) {
      switch (e.ProgressPercentage) {
        case 0: //Cambiar estado de ButtonConnect & TextBoxDeviceId
          ButtonConnect.IsEnabled = TextBoxDeviceId.IsEnabled = e.UserState.ToString() == "true";
          break;
        case 1: //Cambiar estado de ButtonDisconnect & resto de controles inferiores
          ButtonDisconnect.IsEnabled = GroupBoxOptions.IsEnabled = TextBoxInfo.IsEnabled = ButtonRequestText.IsEnabled = e.UserState.ToString() == "true";
          break;
        case 2: //Actualizar contenido de TextBoxInfo
          TextBoxInfo.Text = e.UserState.ToString();
          break;
      }
    }

    protected void ConnWorker_DoWork(object sender, DoWorkEventArgs e) {
      BackgroundWorker thisWorker = (BackgroundWorker)sender;
      BluetoothListener listener = new BluetoothListener(_serviceId); //Instanciamos un nuevo listener para nuestro servicio de Bluetooth
      BluetoothClient client = null;
      NetworkStream stream = null;
      ActionRequest actionRequest;
      byte[] RxBuffer = new byte[1024]; //El buffer de entrada será de 1Kb
      int RxBufferLength = 0; //Tamaño actual del buffer de entrada

      try {
        listener.Start(); //Intentamos arrancarlo. Si no hay Bluetooth o está desactivado se generará una excepción
      } catch {  //No hay Bluetooth o está desactivado
        return; //Hemos terminado
      }
      //El dispositivo Bluetooth local está activado!
      listener.Dispose(); //Ya no necesitamos el Listener. Esto es una aplicación cliente. No atenderemos solcitudes de conexión.
      thisWorker.ReportProgress(0, "true"); //Activado ButtonConnect & TextBoxDeviceId

      while (true) { //Bucle infinito
        if (actionQueue.TryDequeue(out actionRequest)) { //Si conseguimos obtener un nuevo elemento...
          switch (actionRequest.id) {
            case 1: //Seleccionada opción 1
            case 2: //Seleccionada opción 2
            case 3: //Seleccionada opción 3
            case 4: //Solicitud de TextBoxInfo remoto
              stream.WriteByte(actionRequest.id);
              stream.Flush();
              break;
            case 5: //Solicitud de conexión
              BluetoothAddress deviceId;
              BluetoothClient clientNew = new BluetoothClient();
              if (!BluetoothAddress.TryParse(actionRequest.text, out deviceId)) { //Si el DeviceId no tiene el formato correcto...
                thisWorker.ReportProgress(0, "true"); //Reactivamos ButtonConnect & TextBoxDeviceId
              } else { //El DeviceId tiene un formato válido
                try {
                  clientNew.Connect(deviceId, _serviceId); //Intentamos establecer conexión (puede tardar)
                  stream = clientNew.GetStream(); //Creamos un Stream para la comunicación
                  client = clientNew; //Esta será la nueva conexión
                  thisWorker.ReportProgress(1, "true"); //Activamos ButtonDisconnect y resto de controles inferiores
                } catch { //No se ha podido conectar
                  thisWorker.ReportProgress(0, "true"); //Reactivamos ButtonConnect & TextBoxDeviceId
                }
              }
              break;
            case 6: //Solicitud de desconexión
              if (client != null) { //Si estamos conectados...
                stream.WriteByte(127); //Carácter especial para informar desconexión
                stream.Flush();
                stream.Close();
                client.Close();
                client = null; //Ya no tenemos conexión
                thisWorker.ReportProgress(0, "true"); //Reactivamos ButtonConnect & TextBoxDeviceId
              }
              break;
          }
        }
        if (client != null) { //Si estamos conectamos...
          if (stream.DataAvailable) { //Si hay datos pendientes por recibir...
            int RxByte = stream.ReadByte(); //Leemos el siguiente byte
            switch (RxByte) {
              case 10: //Fin de línea "\n"
                //Detectado el caracter de fin de línea
                //Convertiremos el buffer de entrada en un string con codificación UTF8
                //Y lo mostramos como texto recibido
                thisWorker.ReportProgress(2, Encoding.UTF8.GetString(RxBuffer, 0, RxBufferLength));
                RxBufferLength = 0; //Reseteamos el buffer de recepción
                break;
              case 127: //Fin de conexión
                stream.Close();
                client.Close();
                client = null; //Ya no tenemos conexión
                RxBufferLength = 0; //Reseteamos el buffer de recepción
                thisWorker.ReportProgress(1, "false"); //Desactivamos ButtonDisconnect y resto de controles inferiores
                thisWorker.ReportProgress(0, "true"); //Reactivamos ButtonConnect & TextBoxDeviceId
                break;
              default: //Cualquier otro carácter
                RxBuffer[RxBufferLength++] = (byte)RxByte; //Añadimos el byte recibido al buffer de entrada
                break;
            }
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
