// Cliente

using Microsoft.Win32;
using System;
using System.IO;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Windows;

namespace XmasTreeClient {
  /// <summary>
  /// Lógica de interacción para MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window {
    //Variables globales
    //Instanciamos el diálogo de selección de archivos aquí para que los sguientes usos conserven el path
    readonly OpenFileDialog openFileDialog = new OpenFileDialog();

    public MainWindow() {
      InitializeComponent();
    }

    private void ButtonSelect_Click(object sender, RoutedEventArgs e) {
      //Si se acepta el diálogo de selección de archivo...guardamos la selección en el TextBox
      if (openFileDialog.ShowDialog() == true) TextBoxPath.Text = openFileDialog.FileName;
    }

    private void ControlsEnabled(bool status) {
      TextBoxServer.IsEnabled = TextBoxPort.IsEnabled = ButtonSelect.IsEnabled = ButtonSend.IsEnabled = status;
    }

    protected void ThreadSend(string ipString,string portString,string pathString) {
      string errorMessage = "";
      int port = 0;
      int fileLength;
      TcpClient client = null;
      NetworkStream stream;
      string filename;
      FileStream f = null;

      try {
        port = Convert.ToInt32(portString); //Podemos convertir el texto del puerto a valor entero?
      } catch {
        errorMessage = "Error. El puerto debe ser un número entero.";
      }
      if(errorMessage.Length == 0) { //Si no hay errores...
        if (port < 0 || port > 65535) errorMessage = "Error. El puerto debe estar entre 0 y 65535.";
        else { //El puerto está en el rango correcto...
          if (!File.Exists(pathString)) errorMessage = "Error. El archivo no existe.";
          else { //El archivo existe...
            try {
              f = File.OpenRead(pathString); //Intentamos abrir el archivo como sólo lectura
            } catch {
              errorMessage = "Error. El archivo no se puede abrir.";
            }
            if(errorMessage.Length == 0) { //Si hemos podido abrir el archivo...
              //Nota. La longitud de un archivo es un número long (8 bytes).
              //Pero la recepción de un stream utiliza int (4 bytes).
              //El valor máximo que puede contener un int es 2147483647.
              //Como no transmitiremos archivo mayores de 2.1Gb, normalizaremos los tipos a int.
              if (f.Length > 2147483647) errorMessage = "Error. Archivo demasiado grande.";
              else { //Si el archivo tiene un tamaño aceptable...
                fileLength = (int)f.Length; //Obtenemos tamaño en int
                Dispatcher.Invoke(() => {
                  ProgressBarSent.Value = 0; //Borramos la barra de progreso actual
                  ProgressBarSent.Maximum = fileLength; //El máximo de la barra de progreso coincide con el tamaño del archivo
                });
                try {
                  client = new TcpClient(ipString, port); //Intentamos conectar con el servidor
                } catch (SocketException ex) {
                  switch (ex.ErrorCode) {
                    case 10061:
                      errorMessage = "Error. El servidor no tiene la aplicación en ejecución.";
                      break;
                    case 10060:
                    case 11001:
                      errorMessage = "Error. Servidor desconocido.";
                      break;
                    default:
                      errorMessage = "Error de comunicación desconocido [" + ex.ErrorCode.ToString() + "]";
                      break;
                  }
                }
                if (errorMessage.Length == 0) { //Si hemos podido conectar con el servidor...
                  stream = client.GetStream(); //Obtenemos el stream
                  filename = Path.GetFileName(pathString); //Extraemos sólo el nombre
                  byte[] filenameByteArray = Encoding.UTF8.GetBytes(filename);
                  int filenameLength = filenameByteArray.Length; //Longitud del nombre de archivo
                  stream.Write(BitConverter.GetBytes(filenameLength), 0, 4); //Enviamos longitud de nombre de archivo (int = 4bytes)
                  stream.Write(filenameByteArray, 0, filenameLength); //Enviamos el nombre del archivo
                  stream.Write(BitConverter.GetBytes(fileLength), 0, 4); //Enviamos longitud de archivo (int = 4bytes)
                  byte[] TxBuffer = new byte[1024];
                  int bytesRead;
                  while (fileLength > 0) {
                    bytesRead = fileLength;
                    if (bytesRead > 1024) bytesRead = 1024; //Nunca leeremos más de 1Kb
                    f.Read(TxBuffer, 0, bytesRead); //Leemos del archivo
                    try {
                      stream.Write(TxBuffer, 0, bytesRead); //Intentamos enviar al servidor
                    } catch { //Algo ha salido mal. Seguramente el servidor ýa no está escuchando
                      errorMessage = "Error. El servidor se ha desconectado.";
                      fileLength = bytesRead; //Obliga a terminar en el siguiente ciclo
                    }
                    Dispatcher.Invoke(() => {
                      ProgressBarSent.Value += bytesRead; //Incrementamos el el progreso de envío
                    });
                    fileLength -= bytesRead; //Restamos los bytes enviados en este ciclo
                  }
                  stream.Flush(); //Obligamos a enviar datos pendientes
                  if (errorMessage.Length == 0) { //Si el archivo se ha enviado correctamente...
                    //Esperamos confirmación de recepción correcta por parte del servidor o timeout
                    //Calculamos el tiempo de timeout como el actual (en milisegundos) + 2500 (2.5 segundos)
                    long maxTime = DateTimeOffset.Now.ToUnixTimeMilliseconds() + 2500;
                    //Mantenemos el bucle mientras no lleguemos al timeout y no tengamos datos para recibir
                    while (DateTimeOffset.Now.ToUnixTimeMilliseconds() < maxTime && stream.DataAvailable == false);
                    if (stream.DataAvailable) { //Si tenemos datos para recibir...
                      if(stream.ReadByte() == 127) { //Si el código recibido es "recepción correcta!"...
                        errorMessage = "Ok. Archivo " + filename + " enviado correctamente.";
                      }
                    }
                    if(errorMessage.Length == 0) { //Si no tenemos asignado mensaje...
                      //...es porque el código recibido no es el esperado o hemos tenido un timeout
                      errorMessage = "Error. Sin confirmación de recepción correcta.";
                    }
                  }
                  stream.Close(); //Cerramos stream
                  client.Close(); //Cerramos conexión con servidor
                }
              }
              f.Close(); //Cerramos archivo
            }
          }
        }
      }
      //Hemos terminado la ejecución del Thread. Mostramos el mensaje resultante en el Label
      Dispatcher.Invoke(() => {
        LabelInfo.Content = DateTime.Now.ToShortDateString() + " " + DateTime.Now.ToLongTimeString().ToString() + " " + errorMessage;
        ControlsEnabled(true); //Reactivamos controles UI
      });
    }

    private void ButtonSend_Click(object sender, RoutedEventArgs e) {
      ControlsEnabled(false); //Desactivamos todos los controles
      LabelInfo.Content = ""; //Borramos la línea de información
      //Copiamos contenido de los objetos gráficos a variables locales para poder hacer la llamada con parámetros del Thread
      string ipString = TextBoxServer.Text;
      string portString = TextBoxPort.Text;
      string pathString = TextBoxPath.Text;
      //Creamos Thread de envío
      Thread threadSend = new Thread(() => ThreadSend(ipString,portString,pathString));
      threadSend.Start(); //Lo lanzamos
    }
  }
}
