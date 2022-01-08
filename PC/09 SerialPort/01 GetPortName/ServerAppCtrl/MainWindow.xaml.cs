/*
 * Autor: Ramón Junquera
 * Tema: Detección de puertode comunicación asociado a un dispositivo
 * Fecha: 20211008
 * 
 * Descripción:
 * Se crea un función simple que devuelve el puerto de comunicaciones asociado a un dispositivo en concreto.
 * Si no se detecta el dispositivo, se devuelve una cadena vacía.
 * Si hubiese más de un dispositivo del mismo tipo conectado, sólo devolvería el puerto del primero.
 * Se incluyen varios ejemplos de distintos dispositivos.
 * Se muestra el puerto de aquellos dispositivos detectados.
 * 
 * Identificación de dispositivos:
 * Con las siguientes líneas creamos una consulta de todos los dispositivos asociados a un puerto serie, la ejecutamos
 * y concatemos los resultados en formato "DeviceID : Caption".
 *   ManagementObjectSearcher deviceSearcher = new ManagementObjectSearcher("SELECT * FROM Win32_PnPEntity WHERE Caption like '%(COM%'");
 *   List<ManagementBaseObject> devices = deviceSearcher.Get().Cast<ManagementBaseObject>().ToList();
 *   foreach(ManagementBaseObject device in devices) portName += device.GetPropertyValue("DeviceID") + " : " + device.GetPropertyValue("Caption") + "\n"; ;
 * También podemos obtener el DeviceID mirando en el Administrador de dispositivos, sección de puertos, localizando
 * el puerto y mirando su "Ruta de acceso a la instancia del dispositivo"
 */

using System.Collections.Generic;
using System.Linq;
using System.Management;
using System.Windows;


namespace ServerAppCtrl {
  /// <summary>
  /// Lógica de interacción para MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window {

    //Devuelve el nombre del puerto serie en formato COM19. Si no lo encuenta devuelve una cadena vacía.
    //device ID es el identificador del dispositivo.
    private string GetPortName(string deviceId) {
      deviceId = deviceId.Replace("\\", "\\\\"); //Duplicamos el carácter \ porque lo utilizaremos para una consulta!
      string portName = ""; //Inicialmente no tenemos nombre de puerto
      //Creamos una consulta a los dispositivos del sistema que en su Caption
      //contengan la palabra COM (puertos de comunicación) y su DeviceID sea
      //el que buscamos
      ManagementObjectSearcher deviceSearcher = new ManagementObjectSearcher("SELECT * FROM Win32_PnPEntity WHERE Caption like '%(COM%' AND DeviceID = '" + deviceId + "'");
      //Creamos una lista con todos los dispositivos
      List<ManagementBaseObject> devices = deviceSearcher.Get().Cast<ManagementBaseObject>().ToList();
      if(devices.Count>0) { //Si tenemos algún resultado...
        //Obtenemos el Caption del primer dispositivo en formato: "USB Serial Port (COM19)"
        string caption = devices[0].GetPropertyValue("Caption").ToString();
        //Para evitar problemas con idiomas, sólo buscaremos el texto que comienza por COM y llega hasta el siguiente )
        int indexCOM = caption.IndexOf("COM"); //Localizamos el texto "COM"
        //Devolvemos el puerto que comienza en indexCOM y llega hasta el siguiente )
        portName = caption.Substring(indexCOM, caption.IndexOf(")", indexCOM + 3) - indexCOM);
      }
      return portName; //Devolvemos el nombre del puerto
    }

    public MainWindow() { //Constructor
      InitializeComponent();

      //Comprobamos distintos dispositivos y si los encontramos mostramos en qué pue
      TextBoxReceive.Text += "M5Stack Atom Lite : " + GetPortName("FTDIBUS\\VID_0403+PID_6001+6952BB03B4A\\0000") + "\n";
      TextBoxReceive.Text += "M5Stack Atom Echo : " + GetPortName("FTDIBUS\\VID_0403+PID_6001+&1\\0000") + "\n";
      TextBoxReceive.Text += "M5Stack Atom Matrix : " + GetPortName("FTDIBUS\\VID_0403+PID_6001+6D560C14B4A\\0000") + "\n";
      TextBoxReceive.Text += "M5Stack Watch : " + GetPortName("USB\\VID_10C4&PID_EA60\\01402D59") + "\n";
      TextBoxReceive.Text += "M5Stack Fire : " + GetPortName("USB\\VID_10C4&PID_EA60\\018ADCA9") + "\n";
      TextBoxReceive.Text += "Funduino UNO : " + GetPortName("USB\\VID_2341&PID_0043\\75430333437351E07021") + "\n";
      TextBoxReceive.Text += "Arduino MEGA : " + GetPortName("USB\\VID_2341&PID_0042\\55438303539351E0A120") + "\n";
      TextBoxReceive.Text += "ESP8266 ESP-12E : " + GetPortName("USB\\VID_1A86&PID_7523\\5&30719ADF&0&1") + "\n";
      TextBoxReceive.Text += "ESP8266 WeMosD1Mini : " + GetPortName("USB\\VID_10C4&PID_EA60\\011E7302") + "\n";
    }
  }
}

