//Client

using InTheHand.Net;
using InTheHand.Net.Sockets;
using System;
using System.Collections.Concurrent;
using System.ComponentModel;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Windows;

namespace BluetoothCalcRemoteClient {
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
    private readonly Guid _serviceId = new Guid("29913ABD-EB93-40Cf-1234-DEEE26477667");
    private readonly ConcurrentQueue<ActionRequest> actionQueue = new ConcurrentQueue<ActionRequest>();
    private bool requestQuit = false;

    private void SendAction(byte id, string text = "") {
      ActionRequest actionRequest = new ActionRequest(); //Nueva instacia de solicitud de acción
      actionRequest.id = id; //Identificador de acción
      actionRequest.text = text; //Texto de la acción
      actionQueue.Enqueue(actionRequest); //Añadimos a la cola de acciones
    }

    //Activa/desactiva botón de desconexión y controles inferiores
    private void ButtonDisconnectEnable(bool status) {
      ButtonDisconnect.IsEnabled = GroupBoxCalc.IsEnabled = status;
    }

    //Activa/desactiva botón de conexión & TextBoxDeviceId
    private void ButtonConnectEnable(bool status) {
      ButtonConnect.IsEnabled = TextBoxDeviceId.IsEnabled = status;
    }

    private void Button0_Click(object sender, RoutedEventArgs e) {
      SendAction(0); //0
    }
    private void Button1_Click(object sender, RoutedEventArgs e) {
      SendAction(1); //1
    }
    private void Button2_Click(object sender, RoutedEventArgs e) {
      SendAction(2); //2
    }
    private void Button3_Click(object sender, RoutedEventArgs e) {
      SendAction(3); //3
    }
    private void Button4_Click(object sender, RoutedEventArgs e) {
      SendAction(4); //4
    }
    private void Button5_Click(object sender, RoutedEventArgs e) {
      SendAction(5); //5
    }
    private void Button6_Click(object sender, RoutedEventArgs e) {
      SendAction(6); //6
    }
    private void Button7_Click(object sender, RoutedEventArgs e) {
      SendAction(7); //7
    }
    private void Button8_Click(object sender, RoutedEventArgs e) {
      SendAction(8); //8
    }
    private void Button9_Click(object sender, RoutedEventArgs e) {
      SendAction(9); //9
    }
    private void ButtonMultiply_Click(object sender, RoutedEventArgs e) {
      SendAction(10); //*
    }
    private void ButtonMinus_Click(object sender, RoutedEventArgs e) {
      SendAction(11); //-
    }
    private void ButtonPlus_Click(object sender, RoutedEventArgs e) {
      SendAction(12); //+
    }
    private void ButtonEqual_Click(object sender, RoutedEventArgs e) {
      SendAction(13); //=
    }
    private void ButtonDecimal_Click(object sender, RoutedEventArgs e) {
      SendAction(14); //,
    }
    private void ButtonBackSpace_Click(object sender, RoutedEventArgs e) {
      SendAction(15); //<-
    }
    private void ButtonOpen_Click(object sender, RoutedEventArgs e) {
      SendAction(16); //open
    }
    private void ButtonClose_Click(object sender, RoutedEventArgs e) {
      SendAction(17); //close
    }
    private void ButtonGetResult_Click(object sender, RoutedEventArgs e) {
      SendAction(18); //get result
    }
    private void ButtonConnect_Click(object sender, RoutedEventArgs e) {
      ButtonConnectEnable(false);
      SendAction(19, TextBoxDeviceId.Text); //connect
    }
    private void ButtonDisconnect_Click(object sender, RoutedEventArgs e) {
      ButtonDisconnectEnable(false); //Desactivamos ButtonDisconnect y resto de controles inferiores
      SendAction(20); //disconnect
    }
    private void Window_Closing(object sender, CancelEventArgs e) {
      SendAction(20); //disconnect
      Thread.Sleep(500);
      requestQuit = true; //Solicitamos finalización controlada del Thread
      //No hacemos nada más. El proceso de finalización de aplicación sigue en curso.
    }

    private void ThreadConn() { //Thread de gestión de conexiones
      BluetoothListener listener = new BluetoothListener(_serviceId); //Instanciamos un nuevo listener para nuestro servicio de Bluetooth
      ActionRequest actionRequest;
      NetworkStream stream = null;
      BluetoothClient client = null;
      byte[] RxBuffer = new byte[1024]; //El buffer de entrada será de 1Kb
      int RxBufferLength = 0; //Tamaño actual del buffer de entrada

      try {
        listener.Start(); //Intentamos arrancarlo. Si no hay Bluetooth o está desactivado se generará una excepción
      } catch {  //No hay Bluetooth o está desactivado
        return; //Hemos terminado
      }
      //El dispositivo Bluetooth local está activado!
      listener.Dispose(); //Ya no necesitamos el Listener. Esto es una aplicación cliente. No atenderemos solcitudes de conexión.
      Dispatcher.Invoke(() => {
        ButtonConnectEnable(true); //Activamos ButtonConnect & TextBoxDeviceId
      });
      while (!requestQuit) { //Mientras no se solicite terminar...
        if (actionQueue.TryDequeue(out actionRequest)) { //Si conseguimos obtener un nuevo elemento...
          if (actionRequest.id <= 18) { //Si se trata se una pulsación directa...
            //...enviamos su código
            stream.WriteByte(actionRequest.id);
            stream.Flush();
          } else {
            switch (actionRequest.id) {
              case 19: //Solicitud de conexión
                BluetoothAddress deviceId;
                BluetoothClient clientNew = new BluetoothClient();
                if (!BluetoothAddress.TryParse(actionRequest.text, out deviceId)) { //Si el DeviceId no tiene el formato correcto...
                  Dispatcher.Invoke(() => {
                    ButtonConnectEnable(true); //Reactivamos ButtonConnect & TextBoxDeviceId
                  });
                } else { //El DeviceId tiene un formato válido
                  try {
                    clientNew.Connect(deviceId, _serviceId); //Intentamos establecer conexión (puede tardar)
                    stream = clientNew.GetStream(); //Creamos un Stream para la comunicación
                    client = clientNew; //Esta será la nueva conexión
                    Dispatcher.Invoke(() => {
                      ButtonDisconnectEnable(true); //Activamos ButtonDisconnect y resto de controles inferiores
                    });
                  } catch { //No se ha podido conectar
                    Dispatcher.Invoke(() => {
                      ButtonConnectEnable(true); //Reactivamos ButtonConnect & TextBoxDeviceId
                    });
                  }
                }
                break;
              case 20: //Solicitud de desconexión
                if (client != null) { //Si estamos conectados...
                  stream.WriteByte(127); //Carácter especial para informar desconexión
                  stream.Flush();
                  stream.Close();
                  client.Close();
                  client = null; //Ya no tenemos conexión
                  Dispatcher.Invoke(() => {
                    ButtonConnectEnable(true); //Reactivamos ButtonConnect & TextBoxDeviceId
                  });
                }
                break;
            }
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
                string txt = Encoding.UTF8.GetString(RxBuffer, 0, RxBufferLength);
                Dispatcher.Invoke(() => {
                  TextBoxResult.Text = txt; //Actualizar contenido de TextBoxResult
                });
                RxBufferLength = 0; //Reseteamos el buffer de recepción
                break;
              case 127: //Fin de conexión
                stream.Close();
                client.Close();
                client = null; //Ya no tenemos conexión
                RxBufferLength = 0; //Reseteamos el buffer de recepción
                Dispatcher.Invoke(() => {
                  ButtonDisconnectEnable(false); //Desactivamos ButtonDisconnect y resto de controles inferiores
                  ButtonConnectEnable(true); //Reactivamos ButtonConnect & TextBoxDeviceId
                });
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

      new Thread(ThreadConn).Start(); //Instanciamos nuevo Thread de gestión de conexiones
    }
  }
}
