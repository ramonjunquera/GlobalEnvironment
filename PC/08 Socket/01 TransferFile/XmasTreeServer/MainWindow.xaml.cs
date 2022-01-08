//Servidor

using System;
using System.ComponentModel;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Windows;
using System.Windows.Media;

namespace XmasTreeServer {
  /// <summary>
  /// Lógica de interacción para MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window {
    const UInt16 port = 55334;
    const string receivedFolder = "received";
    const string logFilename = "log.txt";
    readonly string receivedPath; //Path de archivos recibidos
    readonly string logPath; //Path del archivo de log
    readonly TcpListener listener;
    UInt16 currentConnections = 0;
    bool requestCloseApplication = false;
    byte[] RxBuffer = new byte[1024];

    private void AddLog(string txt) {
      txt = DateTime.Now.ToShortDateString() + " " + DateTime.Now.ToLongTimeString() + " " + txt + "\n"; //Añadimos time stamp
      File.AppendAllText(logPath, txt); //Añadimos línea al final del archivo
    }

    //Recibe por el stream un determinado número de bytes, que los guarda en RxBuffer
    //Si no lo consigue en un tiempo determinado (timeout) devuelve false
    private bool Receive(NetworkStream stream,int expectedBytes) {
      const long timeOut = 2500; //Máximo tiempo de espera = 2.5 segundos
      long maxTime = DateTimeOffset.Now.ToUnixTimeMilliseconds() + timeOut;
      int receivedBytes = 0;
      int receivedBytesNow;
      while(receivedBytes < expectedBytes && DateTimeOffset.Now.ToUnixTimeMilliseconds() < maxTime) {
        receivedBytesNow = stream.Read(RxBuffer, receivedBytes, expectedBytes - receivedBytes);
        if(receivedBytesNow > 0) maxTime = DateTimeOffset.Now.ToUnixTimeMilliseconds() + timeOut;
        receivedBytes += receivedBytesNow;
      }
      return receivedBytes == expectedBytes;
    }

    private void ThreadReceiveFile(TcpClient client) {
      NetworkStream stream = client.GetStream(); //Creamos el Stream
      int filenameLength;
      string filename;
      int fileLength;
      FileStream f = null;
      //Si no recibimos la longitud del nombre del archivo...error
      if (!Receive(stream,4)) AddLog("Error. Longitud de nombre de archivo no recibida");
      else { //Logitud de nombre de archivo recibida
        filenameLength = BitConverter.ToInt32(RxBuffer, 0); //Extraemos longitud de nombre de archivo
        if (!Receive(stream, filenameLength)) AddLog("Error. Nombre de archivo no recibido");
        else { //Nombre de archivo recibido
          filename = Encoding.UTF8.GetString(RxBuffer, 0, filenameLength); //Extraemos nombre de archivo
          if(!Receive(stream,4)) AddLog("Error. Longitud de archivo (" + filename + ") no recibida");
          else { //Longitud de archivo recibida
            fileLength = BitConverter.ToInt32(RxBuffer, 0); //Extraemos longitud de archivo

            //Calculamos carpeta del día de hoy
            DateTime now = DateTime.Now;
            string todayPath = now.Year.ToString() + now.Month.ToString().PadLeft(2,'0') + now.Day.ToString().PadLeft(2,'0'); //YYYYMMDD
            todayPath = receivedPath + "\\" + todayPath;
            Directory.CreateDirectory(todayPath);

            f = File.OpenWrite(todayPath + "\\" + filename); //Creamos archivo en carpeta de recepción
            int bytesRead; //Cantidad de bytes que leeremos en el ciclo
            while (fileLength > 0) {
              bytesRead = fileLength;
              if (bytesRead > 1024) bytesRead = 1024; //Nunca leeremos más de 1Kb
              if(!Receive(stream,bytesRead)) { //No se han leido los bytes esperados...
                fileLength = -1; //Hemos terminado con error
              } else { //Se han leido los bytes esperados
                f.Write(RxBuffer, 0, bytesRead); //Guardamos datos leidos en el archivo
                fileLength -= bytesRead; //Restamos los bytes leidos en este ciclo
              }
            }
            f.Close(); //Cerramos archivo
            if (fileLength < 0) { //Si no hemos recibido el archivo en su totalidad...
              AddLog("Error. Recepción interrumpida en recepción de datos de archivo " + filename);
              File.Delete(todayPath + "\\" + filename); //...lo borramos
            } else { //Hemos recibido el archivo completo
              AddLog("Recepción correcta de archivo " + filename);
              stream.WriteByte(127); //Enviamos código de confirmación de recepción correcta
              stream.Flush();
            }
          }
        }
      }
      stream.Close(); //Cerramos stream
      client.Close(); //Cerramos conexión

      //Hemos terminado la ejecución del Thread
      //Si este era el último Thread en ejecución...volvemos a poner el semáforo en amarillo
      //Si no fuese el último...no lo tocamos. Seguirá en verde. Ya lo cambiará el último.
      if (--currentConnections == 0) {
        Dispatcher.Invoke(() => {
          RectangleSemaphore.Fill = new SolidColorBrush(System.Windows.Media.Colors.Yellow); //Semáforo en amarillo
        });
      }
    }

    private void ThreadAcceptConnections() {
      AddLog("Servidor iniciado"); 
      while(!requestCloseApplication) { //Mientras no se solicite terminar la aplicación...
        //Nota.
        //Podríamos utilizar directamente AcceptTcpClient dentro del bucle infinito para que espere
        //a la siguiente conexión. El problema de ese método es que esperará para siempre.
        //Ni siquiera una solicitud de abortar el Thread lo detiene.
        //Esto provoca que el Thread quede activo aunque se cierre el interface gráfico, impidiendo
        //que la aplicación finalice.
        //Para que esto no ocurra, sólo utilizaremos este método cuando tengamos la certeza de que
        //hay alguna solicitud de conexión pendiente.
        if (listener.Pending()) { //Si tenemos solicitudes de conexión pendientes...
          TcpClient client = listener.AcceptTcpClient(); //Anotamos el cliente que quiere conectar
          //Si este es el único Thread...ponemos el semáforo en verde
          //Si no es el único, ya estará en verde
          if (++currentConnections == 1) {
            Dispatcher.Invoke(() => {
              RectangleSemaphore.Fill = new SolidColorBrush(System.Windows.Media.Colors.Green); //Semáforo en verde
            });
          }
          //Creamos un Thread para que la atienda. Le pasamos el cliente como parámetro
          new Thread(() => ThreadReceiveFile(client)).Start();
        }
      }
      listener.Stop();
      AddLog("Servidor finalizado");
    }

    public MainWindow() {
      InitializeComponent();

      logPath = Directory.GetCurrentDirectory() + "\\" + logFilename;
      receivedPath = Directory.GetCurrentDirectory() + "\\" + receivedFolder;
      Directory.CreateDirectory(receivedPath); //Si ya existe, no lo crea, ni destruye su contenido

      string hostName = Dns.GetHostName(); //Obtenemos el hostname
      IPAddress[] localIPs = Dns.GetHostEntry(hostName).AddressList; //Obtenemos todas las IPs de la máquina
      foreach (IPAddress ip in localIPs) { //Recorremos todas las IPs
        //Si es una IPv4...la añadimos
        if (ip.AddressFamily == AddressFamily.InterNetwork) hostName += "[" + ip.ToString() + "]";
      }
      hostName += ":" + port.ToString(); //Completamos el nombre con el puerto
      TextBoxIP.Text = hostName; //Mostramos las posibilidades de hostname

      //Ponemos en marcha el Listener
      //La forma correcta es: listener = new TcpListener(localIP,port);
      //Este sistema nos obliga a seleccionar una sola IP para que la atienda el Listener.
      //Si tenemos varias IPs, de distintos adaptadores (físicos o virtuales), estamos restringiendo
      //las conexiones por un adaptador.
      //Hay un método que permite crear un Listener para cualquier adaptador. Sólo debemos indicar el puerto.
      //El problema es que este método se considera obsoleto.
      //En este caso lo utilizaremos y añadiremos una excepción en esa línea para que no se queje el compilador.
#pragma warning disable CS0618 //Desactivar warnings de métodos obsoletos
      listener = new TcpListener(port); //El Listener escuchará el puerto en cualquier adaptador.
#pragma warning restore CS0618 //Activar warnings de métodos obsoletos
      listener.Start();
     
      new Thread(ThreadAcceptConnections).Start(); //Instanciamos el Thread de gestión de solicitudes de conexión y lo lanzamos
      //Nota. No guardamos el objeto porque tiene un sistema de finalización controlada
    }

    //Se ha solicitado cerrar la aplicación
    private void Window_Closing(object sender, CancelEventArgs e) {
      requestCloseApplication = true;
    }
  }
}
