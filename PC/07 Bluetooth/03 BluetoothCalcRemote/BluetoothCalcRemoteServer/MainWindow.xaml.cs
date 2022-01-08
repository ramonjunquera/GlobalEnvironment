//Server

//Nota 1
//Aunque el ejecutable de la calculadora de Windows siempre tiene el mismo nombre, la aplicación no.
//En un Windows en español se llama "calculadora" y en inglés es "calculator".

//Nota 2
//En sistemas operativos Windows 10 antiguos, la calculadora cambia el valor del parámetro AutomationId
//de sus botones. Por esta razón se hace crea una tabla con lso distintos valores de AutomationId de los
//botones que queremos controlar. Concreamente la opción FIS corresponde a un Windows 10 antiguo en español.

using InTheHand.Net.Bluetooth;
using InTheHand.Net.Sockets;
using System;
using System.ComponentModel;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Windows;
using System.Windows.Automation;
using System.Windows.Media;

namespace BluetoothCalcRemoteServer {
  /// <summary>
  /// Lógica de interacción para MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window {
    //Variables globales
    private readonly Guid _serviceId = new Guid("29913ABD-EB93-40Cf-1234-DEEE26477667");
    private bool requestCloseApplication = false; //Se quiere cerrar la aplicación
    private string appName = "Calculator";
    private byte systemVer = 0; //Versión de sistema operativo (columna de tabla automationIds
    private string[,] automationIds = new string[17,2]{
      { "num0Button", "130" } //0
      , {"num1Button", "131"} //1
      , {"num2Button", "132"} //2
      , {"num3Button", "133"} //3
      , {"num4Button", "134"} //4
      , {"num5Button", "135"} //5
      , {"num6Button", "136"} //6
      , {"num7Button", "137"} //7
      , {"num8Button", "138"} //8
      , {"num9Button", "139"} //9
      , {"multiplyButton", "92"} //*
      , {"minusButton", "94"} //-
      , {"plusButton", "93"} //+
      , {"equalButton", "121"} //=
      , {"decimalSeparatorButton", "84"} //,
      , {"backSpaceButton", "83"} //<-
      , {"CalculatorResults", "150"} //display
    };

    private void RadioButtonENG_Checked(object sender, RoutedEventArgs e) {
      appName = "Calculator"; systemVer = 0;
    }

    private void RadioButtonESP_Checked(object sender, RoutedEventArgs e) {
      appName = "Calculadora"; systemVer = 0;
    }

    private void RadioButtonTablet_Checked(object sender, RoutedEventArgs e) {
      appName = "Calculadora"; systemVer = 1;
    }

    private AutomationElement GetElementObj(AutomationElement appObj, String automationId) {
      //Creamos una nueva condición para buscar por la propiedad AutomationId
      System.Windows.Automation.Condition condition = new PropertyCondition(AutomationElement.AutomationIdProperty, automationId);
      //Aplicamos las condiciones de búsqueda a los elementos de la aplicación y devolvemos la primera referencia encontrada
      return appObj.FindFirst(TreeScope.Descendants, condition);
    }

    /// <summary>
    /// Genera el mensaje de Click en el botón de la aplicación indicada
    /// </summary>
    /// <param name="appObj"></param>
    /// <param name="automationId"></param>
    private void ClickRemoteButton(AutomationElement appObj, String automationId) {
      if (appObj != null) { //Si la aplicación está en ejecución...
        //Obtenemos el objeto del ejemento
        AutomationElement myButton = GetElementObj(appObj, automationId);
        if (myButton != null) { //Si se ha encontrado el elemento que buscamos...
          //Creamos el objeto que generará el mensage de pulsación del botón
          InvokePattern btnPattern = myButton.GetCurrentPattern(InvokePattern.Pattern) as InvokePattern;
          btnPattern.Invoke(); //Lanzamos el mensaje de pulsación del bótón
        }
      }
    }

    /// <summary>
    /// Obtiene el objeto de la primera instancia en ejecución de la aplicación
    /// </summary>
    /// <returns>Puede devolver null</returns>
    private AutomationElement GetAppObj() {
      //Creamos elemento raíz del árbol de aplicaciones en ejecución
      AutomationElement root = AutomationElement.RootElement;
      //Creamos condición que filtra por nombre de aplicación
      System.Windows.Automation.Condition condition = new PropertyCondition(AutomationElement.NameProperty, appName);
      //Filtramos el árbol de aplicaciones en ejecución por nombre y guardamos el primer resultado obtenido
      return root.FindFirst(TreeScope.Children, condition);
    }

    private void Window_Closing(object sender, CancelEventArgs e) {
      requestCloseApplication = true;
    }

    private void ThreadConn() {
      BluetoothListener listener = new BluetoothListener(_serviceId); //Instanciamos un nuevo listener para nuestro servicio de Bluetooth
      BluetoothClient client = null;
      NetworkStream stream = null;

      try {
        listener.Start(); //Intentamos arrancarlo. Si no hay Bluetooth o está desactivado se generará una excepción
      } catch {  //No hay Bluetooth o está desactivado
        return; //Hemos terminado
      }
      BluetoothRadio radio = BluetoothRadio.Default;
      Dispatcher.Invoke(() => {
        TextBoxDeviceId.Text = radio.LocalAddress.ToString(); //Actualiza TextBoxDeviceId
        RectangleSemaphore.Fill = new SolidColorBrush(System.Windows.Media.Colors.Yellow); //Semáforo amarillo
      });
      while (true) { //Bucle infinito
        if (client == null) { //Si no estamos conectamos...
          if (listener.Pending()) { //Si tenemos alguna solicitud de conexión pendiente por atender...
            //Recogemos la posible petición de conexión
            //El método AcceptBluetoothClient es peligroso porque detiene el flujo a la espera de un nuevo cliente
            //Sólo debe usarse cuando tengamos la certeza de que hay una solicitud de conexión pendiente
            client = listener.AcceptBluetoothClient(); //Obtenemos la solicitud
            stream = client.GetStream(); //Creamos un Stream para la comunicación
            Dispatcher.Invoke(() => {
              RectangleSemaphore.Fill = new SolidColorBrush(System.Windows.Media.Colors.Green); //Semáforo verde
            });
          }
        } else { //Si ya estamos conectados...
          if (stream.DataAvailable) { //Si hay datos pendientes por recibir...
            int RxByte = stream.ReadByte(); //Leemos el siguiente byte
            switch (RxByte) {
              case 0: //0
              case 1: //1
              case 2: //2
              case 3: //3
              case 4: //4
              case 5: //5
              case 6: //6
              case 7: //7
              case 8: //8
              case 9: //9
              case 10: //*
              case 11: //-
              case 12: //+
              case 13: //=
              case 14: //,
              case 15: //BackSpace
                ClickRemoteButton(GetAppObj(), automationIds[RxByte, systemVer]);
                break;
              case 16: //Open
                //Abrimos una nueva instancia de la aplicación y esperamos un máximo de 5
                //segundos para que termine de abrirse
                System.Diagnostics.Process.Start("calc").WaitForInputIdle(5000);
                break;
              case 17: //Close
                ClickRemoteButton(GetAppObj(), "Close");
                break;
              case 18: //Get result
                AutomationElement appObj = GetAppObj(); //Obtenemos el objeto de la aplicación
                if (appObj != null) { //Si tenemos la aplicación en ejecución
                  //Obtenemos el objeto del elemento que muestra el resultado (display)
                  AutomationElement elementObj = GetElementObj(appObj, automationIds[16, systemVer]);
                  if (elementObj != null) { //Se ha encontrado el elemento del display
                    //Obtenemos el contenido del objeto...
                    //La propiedad Name contiene el resultado actual de la operación, pero tiene un
                    //prefijo añadido:
                    //-Español: "La pantalla muestra "
                    //-Inglés: "Display is "
                    //Para que la eliminación del prefijo no dependa del idioma, buscaremos la posición
                    //del último espacio y tomaremos sólo el resto.
                    string res = elementObj.Current.Name; //Obtenemos el texto del elemento (contiene prefijo).
                    //El texto que nos interesa está después del úlñtimo espacio
                    int lastSpaceIndex = res.LastIndexOf(" ") + 1;
                    res = res.Substring(lastSpaceIndex); //Tomamos sólo la última palabra
                    //Codificamos el texto a enviar con UTF8 en un array de bytes para poder enviarlo por stream
                    byte[] textByteArray = Encoding.UTF8.GetBytes(res);
                    stream.Write(textByteArray, 0, textByteArray.Length);
                    stream.WriteByte(10); //Fin de línea "\n"
                    stream.Flush();
                  }
                }
                break;
              case 127: //Fin de conexión
                stream.Close();
                client.Close();
                client = null; //Ya no tenemos conexión
                Dispatcher.Invoke(() => {
                  RectangleSemaphore.Fill = new SolidColorBrush(System.Windows.Media.Colors.Yellow); //Semáforo amarillo
                });
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
          return; //Salimos del método de Thread
        }
      }
    }

    public MainWindow() {
      InitializeComponent();

      new Thread(ThreadConn).Start(); //Nueva instancia para el Thread que gestionará las comunicaciones Bluetooth
    }

   
  }
}
