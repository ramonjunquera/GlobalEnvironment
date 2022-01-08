/*
 * Tema: Bluetooth
 * Fecha: 20210922
 * Autor: Ramón Junquera
 * 
 * Descripción:
 * Crear una aplicación que se comporte tanto como cliente como servidor, para hacer un chat que comunique por Bluetooth.
 * Es una aplicación de escritorio desarrollada en C#, con estructura WPF y con las librerías de .NET Framework 4.6.1.
 * Para la gestión del Bluetooth se utiliza el paquete NuGet InTheHand.Net.Bluetooth.
 * Esta dll ha sido compilada con .NET Framework 4.6.1. Es la razón por la que desarrollamos el proyecto en esta versión.
 * 
 * Gestión de comunicaciones.
 * La gestión de las comunicaciones de debe realizar en segundo plano.
 * Se utiliza un objeto BackgroundWorker como Thread.
 * Puesto que toda la gestión la realiza un único Thread, no tenemos que preocuparnos por la sincronización de tareas,
 * ni por el bloqueo a variables compartidas por varios Threads.
 * La interacción con los objetos gráficos se hace mediante la función de progreso, que afortunadamente se ejecuta en
 * primer plano.
 * Las peticiones al Thread se hacer mediante una cola concurrente para evitar problemas de bloqueos.
 * La estructura de una petición tiene dos campos: id & text.
 * Para simplificar, sólo se permite una conexión simultánea.
 * Cuando establece una conexión se dejan de atender la solicitudes de conexión de dispositivos remotos. Esto conllevará
 * un error de timeout para ellos.
 * 
 * Desconexiones.
 * Según la documentación de InTheHand, la única manera que tenemos para detectar una desconexión es a través del
 * método StreamReader.ReadLine(), que puede devolver un string con la línea recibida, o un null si se ha perdido la
 * conexión.
 * El problema es que ReadLine es síncrono. La llamada se quedará esperando hasta que se detecte una desconexión
 * o se reciba una nueva línea de texto.
 * Como sólo tenemos un Thread, no podemos bloquear su flujo de esta manera.
 * Para hacerlo bien, se debería crear un Thread sólo para la recepción de mensajes con esta instrucción y sincronizarlo
 * con otro Thread que realice el resto de tareas.
 * Para solventar el problema de forma simplificada, cuando se quiere desconectar, antes de cerrar, se envía un carácter
 * con un código especial cuyo significado es "desconexión".
 * Cuando se recibe este carácter, también se cierra la conexión local.
 * El código del carácter es 127.
 * La razón de este valor es que según la codificación UTF, todos los caracteres con código superior a 127, necesitarán
 * al menos 2 bytes para codificarse.
 * Por ejemplo: 255=195+191, 254=195+190, 128=194+128, ...
 * 
 * Codificación.
 * Lo que recibimos y enviamos son bytes, no caracteres.
 * Para poder utilziar los caracteres especiales (ñ, á, ...) codificaremos el texto con UTF8.
 * Para enviar codificamos el mensaje y generamos un array de bytes, que enviamos por el Stream.
 * Al recibir, tenemos un buffer de recepción en el que vamos guardando cada byte recibido.
 * Al detectar el código de fin de línea, decodificamos el array de bytes del buffer de recepción con UTF8, generando
 * un string.
 * No se ha implementado ningún control para el overflow del buffer de entrada (mensajes con tamaño superior a 1Hb).
 * 
 * Identificador de servicio Bluetooth
 * Un dispositivo Bluetooth puede tener varios servicios corriendo simultáneamente, por eso es imprescindible indicar
 * el identificador de servicio tanto al conectar como al publicar un servicio con el Listener.
 * El identificador puede tener cualquier valor.
 * Todos los servicios Bluetooth ofrecidos en un dispositivo deben tener identificadores distintos.
 */

using InTheHand.Net;
using InTheHand.Net.Bluetooth;
using InTheHand.Net.Sockets;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace RoJoBluetoothChat {
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
    private readonly Guid _serviceId = new Guid("29913A2D-EB93-40Cf-1234-DEEE26452197");
    private readonly ConcurrentQueue<ActionRequest> actionQueue = new ConcurrentQueue<ActionRequest>();
    private IReadOnlyCollection<BluetoothDeviceInfo> devices; //Dispositivos detectados

    /// <summary>
    /// Añade una nueva línea al ListBox de mensajes y hace scroll hasta su última línea
    /// </summary>
    /// <param name="message"></param>
    /// <param name="messageType">0=Error,1=Info,2=Rx,3=Tx</param>
    private void AddMessage(string message, int messageType) {
      string prefix;
      switch(messageType) {
        case 0:
          prefix = "Error: ";
          break;
        case 1:
          prefix = "Info: ";
          break;
        case 2:
          prefix = "Rx: ";
          break;
        case 3:
          prefix = "Tx: ";
          break;
        default:
          prefix = "???: ";
          break;
      }
      textBoxInfo.Text += prefix + message + "\n"; //Componemos texto y mostramos como nueva línea
      textBoxInfo.ScrollToEnd(); //Siempre mostramos la última línea, aunque haya que hacer scroll
    }

    protected void ScanWorker_Completed(object sender, RunWorkerCompletedEventArgs e) {
      listBoxDevices.Items.Clear();
      foreach (BluetoothDeviceInfo d in devices) {
        listBoxDevices.Items.Add("(" + d.DeviceAddress + ") " + d.DeviceName);
      }
      buttonScan.IsEnabled = true;
      AddMessage("Scan finalizado. " + devices.Count.ToString() + " dispositivos encontrados.", 1); //Info
    }

    protected void ScanWorker_DoWork(object sender, DoWorkEventArgs e) {
      BluetoothClient client = new BluetoothClient();
      devices = client.DiscoverDevices(); //Guardamos dos dispositivos diecubiertos la variable global devices
    }

    private void buttonScan_Click(object sender, RoutedEventArgs e) {
      buttonScan.IsEnabled = false; //Lo desactivamos mientras dure el scan
      AddMessage("Info: Scan iniciado...", 1); //Info
      BackgroundWorker scanWorker = new BackgroundWorker();
      scanWorker.DoWork += new DoWorkEventHandler(ScanWorker_DoWork);
      scanWorker.RunWorkerCompleted += new RunWorkerCompletedEventHandler(ScanWorker_Completed);
      scanWorker.RunWorkerAsync(); //Lanzamos el Thread
    }

    private void listBoxDevices_SelectionChanged(object sender, SelectionChangedEventArgs e) {
      ListBox lb = (ListBox)sender;
      textBoxDeviceId.Text = devices.ElementAt(lb.SelectedIndex).DeviceAddress.ToString();
    }

    private void SendAction(byte id,string text="") {
      ActionRequest actionRequest = new ActionRequest(); //Nueva instacia de solicitud de acción
      actionRequest.id = id; //Identificador de acción
      actionRequest.text = text; //Texto de la acción
      actionQueue.Enqueue(actionRequest); //Añadimos a la cola de acciones
    }

    private void buttonDisconnect_Click(object sender, RoutedEventArgs e) {
      buttonDisconnect.IsEnabled = false; //Evitamos nuevas pulsaciones en el proceso de desconexión
      SendAction(0); //Desconexión
    }

    private void Window_Closing(object sender, CancelEventArgs e) {
      SendAction(3); //Desconexión y cerrado de aplicación
    }

    private void buttonConnect_Click(object sender, RoutedEventArgs e) {
      buttonConnect.IsEnabled = false; //Evitamos nuevas pulsaciones en el proceso de conexión
      SendAction(1, textBoxDeviceId.Text); //Conexión
    }

    private void textBoxMessage_KeyDown(object sender, KeyEventArgs e) {
      TextBox tb = (TextBox)sender;
      if (e.Key.ToString() == "Return") {
        SendAction(2, tb.Text); //Enviar mensaje
        tb.Text = "";
      }
    }

    protected void ConnWorker_ProgressChanged(object sender, ProgressChangedEventArgs e) {
      switch (e.ProgressPercentage) {
        case 0: //Añadir mensaje de error
        case 1: //Añadir mensaje informativo
        case 2: //Añadir mensaje recibido
        case 3: //Añadir mensaje enviado
          AddMessage(e.UserState.ToString(), e.ProgressPercentage);
          break;
        case 4: //Cambiar estado a botón Scan
          buttonScan.IsEnabled = e.UserState.ToString() == "true";
          break;
        case 5: //Cambiar estado a botón Conectar
          buttonConnect.IsEnabled = e.UserState.ToString() == "true";
          break;
        case 6: //Cambiar estado a botón Desconectar & Enviar
          buttonDisconnect.IsEnabled = e.UserState.ToString() == "true";
          break;
        default:
          AddMessage("Identificador de acción desconocido (" + e.ProgressPercentage.ToString() + ")", 0); //Error
          break;
      }
    }

    protected void ConnWorker_DoWork(object sender, DoWorkEventArgs e) {
      BackgroundWorker thisWorker = (BackgroundWorker)sender;
      BluetoothListener listener = new BluetoothListener(_serviceId); //Instanciamos un nuevo listener para nuestro servicio de Bluetooth
      BluetoothClient client = null;
      NetworkStream stream = null;
      byte[] RxBuffer = new byte[1024]; //El buffer de entrada será de 1Kb
      int RxBufferLength = 0; //Tamaño actual del buffer de entrada

      try {
        listener.Start(); //Intentamos arrancarlo. Si no hay Bluetooth o está desactivado se generará una excepción
      } catch {  //No hay Bluetooth o está desactivado
        thisWorker.ReportProgress(0, "Bluetooth no disponible."); //Error
        return; //Hemos terminado
      }
      //El dispositivo Bluetooth local está activado!. Mostramos su nombre
      BluetoothRadio radio = BluetoothRadio.Default;
      thisWorker.ReportProgress(1, "Bluetooth local (" + radio.LocalAddress + ") " + radio.Name); //Info
      thisWorker.ReportProgress(4, "true"); //Activamos botón Scan
      thisWorker.ReportProgress(5, "true"); //Activamos botón Conectar
      ActionRequest actionRequest;
      while(true) { //Bucle infinito
        if (client != null) { //Si estamos conectamos...
          if (stream.DataAvailable) { //Si hay datos pendientes por recibir...
            int RxByte = stream.ReadByte(); //Leemos el siguiente byte
            switch(RxByte) {
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
                thisWorker.ReportProgress(6, "false"); //Desactivamos botón Desconectar
                thisWorker.ReportProgress(5, "true"); //Activamos botón Conectar
                thisWorker.ReportProgress(1, "El dispositivo remoto ha finalizado la conexión"); //Info
                break;
              default: //Cualquier otro carácter
                RxBuffer[RxBufferLength++] = (byte)RxByte; //Añadimos el byte recibido al buffer de entrada
                break;
            }
          }
        } else { //Si no estamos conectados
          if(listener.Pending()) { //Si tenemos alguna solicitud de conexión pendiente por atender...
            //Recogemos la posible petición de conexión
            //El método AcceptBluetoothClient es peligroso porque detiene el flujo a la espera de un nuevo cliente
            //Sólo debe usarse cuando tengamos la certeza de que hay una solicitud de conexión pendiente
            BluetoothClient clientNew = listener.AcceptBluetoothClient(); 
            stream = clientNew.GetStream(); //Creamos un Stream para la comunicación
            client = clientNew; //Esta será la nueva conexión
            thisWorker.ReportProgress(1, "Conexión aceptada de (" + ((BluetoothEndPoint)client.Client.RemoteEndPoint).Address + ") " + client.RemoteMachineName);
            thisWorker.ReportProgress(5, "false"); //Desactivamos botón Conectar
            thisWorker.ReportProgress(6, "true"); //Activamos botón Desconectar
          }
        }
        if (actionQueue.TryDequeue(out actionRequest)) { //Si conseguimos obtener un nuevo elemento...
          switch(actionRequest.id) {
            case 0: //Solicitud de desconexión
              if(client != null) { //Si estamos conectados...
                stream.WriteByte(127); //Carácter especial para informar desconexión
                stream.Flush();
                stream.Close();
                client.Close();
                client = null; //Ya no tenemos conexión
                thisWorker.ReportProgress(5, "true"); //Activamos botón Conectar
                thisWorker.ReportProgress(1, "Desconectado"); //Info
              }
              break;
            case 1: //Solicitud de conexión. Suponemos que estamos desconectados
              BluetoothAddress deviceId;
              BluetoothClient clientNew = new BluetoothClient();
              if (!BluetoothAddress.TryParse(actionRequest.text, out deviceId)) { //Si el DeviceId no tiene el formato correcto...
                thisWorker.ReportProgress(0, "El identificador no tiene un formato válido"); //Error
                thisWorker.ReportProgress(5, "true"); //Reactivamos botón Conectar
              } else { //El DeviceId tiene un formato válido
                thisWorker.ReportProgress(1, "Intentando conectar con " + actionRequest.text); //Info
                string errorMessage = "";
                try {
                  clientNew.Connect(deviceId, _serviceId); //Intentamos establecer conexión (puede tardar)
                } catch (SocketException ex) {
                  switch (ex.ErrorCode) {
                    case 10049:
                      errorMessage = "El dispositivo no tiene el servicio en ejecución";
                      break;
                    case 10060:
                      errorMessage = "Dispositivo no encontrado";
                      break;
                    default:
                      errorMessage = "Error desconocido (" + ex.ErrorCode.ToString() + ")";
                      break;
                  }
                }
                if(errorMessage.Length > 0) { //Si ha ocurrido un error...
                  thisWorker.ReportProgress(0, errorMessage); //Error
                  thisWorker.ReportProgress(5, "true"); //Reactivamos botón Conectar
                } else { //No hubo errores
                  stream = clientNew.GetStream(); //Creamos un Stream para la comunicación
                  client = clientNew; //Esta será la nueva conexión
                  thisWorker.ReportProgress(1, "Conexión establecida con (" + ((BluetoothEndPoint)client.Client.RemoteEndPoint).Address + ") " + client.RemoteMachineName); //Info
                  thisWorker.ReportProgress(6, "true"); //Activamos botón Desconectar
                }
              }
              break;
            case 2: //Solicitud de envío de mensaje. Suponemos que estamos conectados
              //Codificamos el texto a enviar con UTF8 en un array de bytes para poder enviarlo por stream
              byte[] textByteArray = Encoding.UTF8.GetBytes(actionRequest.text);
              stream.Write(textByteArray, 0, textByteArray.Length);
              stream.WriteByte(10); //Fin de línea "\n"
              stream.Flush();
              thisWorker.ReportProgress(3, actionRequest.text); //Tx
              break;
            case 3: //Solicitud de desconexión y cerrado de aplicación
              //Mismo código que desconexión + cerrado de aplicación
              if (client != null) { //Si estamos conectados...
                stream.WriteByte(127); //Carácter especial para informar desconexión
                stream.Flush();
                stream.Close();
                client.Close();
                client = null; //Ya no tenemos conexión
                thisWorker.ReportProgress(5, "true"); //Activamos botón Conectar
                thisWorker.ReportProgress(1, "Desconectado"); //Info
              }
              Environment.Exit(0); //Terminamos la aplicación
              break;
            default: //Identificador de acción desconocido!!!
              thisWorker.ReportProgress(0, "Identificador de acción desconocido (" + actionRequest.id.ToString() + ")"); //Error
              break;
          }
        }
      }
    }

    private void BluetoothBegin() {
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

    public MainWindow() {
      InitializeComponent();
      BluetoothBegin();
    }
  }
}
