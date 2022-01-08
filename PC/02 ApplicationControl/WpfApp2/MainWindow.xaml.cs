/*
 * Objetivo: Controlar una aplicación en ejecución local
 * Fecha: 20210903
 * Autor: Ramón Junquera
 * 
 * Descripción:
 * Como prueba de concepto utilizaremos la aplicación de la calculadora de Windows.
 * Desarrollaremos una aplicación que se comunicará con la aplicación controlada de manera
 * bidireccional. Enviaremos tanto pulsaciones como recogerenmos el contenido de algún elemento,
 * concretamente el que muestra el resultado de las operaciones.
 * Nuestra aplicación tendrá algunos de los botones más importantes de la calculadora y al pulsarlos
 * se replicará la pulsación en la aplicación controlada.
 * 
 * El primer problema con el que nos encontramos es la identificación de los elementos de la aplicación
 * que queremos controlar.
 * Visual Studio nos facilita la herramienta Spy++ (o Spy64++), pero no siempre es útil.
 * Si los elementos gráficos de la aplicación no tienen identificadores asignados o los identificadores
 * son dinámicos dependiendo de otros factores (como en el caso de la calculadora con el idioma), Spy++ no
 * nos servirá.
 * De todas formas existe una herramienta gratuita de terceros que nos facilita esta información.
 * Su nombre es: Accessibility Insights For Windows
 * Se puede descargar desde: https://accessibilityinsights.io/docs/en/windows/overview/
 * La sección "Inspect" es la es capaz de identificar los distintos elementos gráficos de cualquier
 * aplicación en ejecución y mostrar dus detalles.
 * Es muy conveniente entrar en la configuración de esta sección y pedir que se muestren todas las propiedades
 * que tengan valor. No sólo las básicas. Cuantas más tengamos, más fácil será identificar los elementos.
 * Entre todos ellos debemos definir cuál es la condición que identifica a un elemento.
 * Por ejemplo, el elemento debe ser de tipo botón (Button) y tenemos como nombre (Name) "Uno".
 * Podemos utilizar cualquier características. Las más útiles son: Name, ClassName y AutomationId.
 * Si analizamos la calculadora en versión española con esta herramienta veremos que el botón 1 tiene
 * las siguientes características principales:
 * - AutomationId = num1Button
 * - ClassName = Button
 * - Name = Uno
 * La propiedad Name cambia con el idioma. En cambio AutomationId no. Así que nos conviene utilizar ésta última.
 * También obtenemos otros botones que nos interesan, con los siguientes AutomationId: "decimalSeparatorButton","multiplyButton",
 * "minusButton","plusButton","equalButton","backSpaceButton", etc.
 * Los botones pertenecientes a la ventana (maximizar, minimizar, cerrar, ...) se comportan como un botón más.
 * Concretamente el botón de cerrar tiene como AutomationId "Close".
 * Hay un elemento especial que es el que muestra el resultado de las operaciones (display).
 * Comprobamos que su AutomationId es "CalculatorResults" y su Name guarda el resultado.
 * Pero Name no sólo guarda el resultado de la operación, sino que tiene un prefijo que depende del idioma:
 * - Español: "La pantalla muestra "
 * - Inglés: "Display is "
 * Si no queremos que esto dependa del idioma tendremos que utilizar algún truco, como por ejemplo tomar sólo los
 * caracteres que hay después del último espacio.
 * De esta propiedad leeremos la información que nos interesa.
 * 
 * Resuelto el problema de la identificación de elementos, aparece el problema de cómo gestionarlos.
 * Se utilizaran las librerías de Microsoft de UIAutomation (System.Windows.Automation) especializadas en
 * el desarrollo de RPAs (Robot Process Automation).
 * Estas son compatibles con todo tipo de estructura (Forms, NETCore, .NET Framework, WPF, ...).
 * Necesitamos seguir algunos pasos para poder utilizarlas en nuestra solución:
 * - Creamos la solución vacía en Visual Studio.
 * - En el explorador de soluciones, localizamos la sección de "Dependencias"
 * - Click con el botón derecho y seleccionamos "Agregar referencia COM..."
 * - Localizamos "UIAutomationClient" y marcamos su check
 * - Aceptamos la selección
 * A partir de ahora sólo tenemos que indicar al principio del código que utilizaremos esas librerías con:
 *   using System.Windows.Automation;
 * 
 * Nota 1. Condiciones por distintos parámetros
 * En el ejemplo sólo realizamos búsquedas de elementos basadas en el parámetro AutomationId.
 * No hay problema en utilizar el resto de campos.
 * Ejemplo. Creamos una condición para buscar el elemento que en el parámetro Name tenga el valor "Uno":
 * System.Windows.Automation.Condition condition = new PropertyCondition(AutomationElement.NameProperty, "Uno");
 * 
 * Nota 2. Combinación de condiciones
 * Afortunadamente todos los elementos con los que queremos trabajar tienen un AutomationId único.
 * Esta es la razón por la que las condiciones de búsqueda son simples.
 * No hay problema utilizar más de una condición. Lo que deberíamos hacer es crear las condiciones por separado
 * y después combinarlas en una única.
 * Ejemplo: Buscamos un elemento que sea un botón (ClassName=Button) y además el parámetro Name sea "Uno":
 * System.Windows.Automation.Condition condition1 = new PropertyCondition(AutomationElement.ClassNameProperty, "Button");
 * System.Windows.Automation.Condition condition2 = new PropertyCondition(AutomationElement.NameProperty, "Uno");
 * System.Windows.Automation.Condition condition = new AndCondition(condition1, condition2);
 */

using System;
using System.Windows;
using System.Windows.Automation;

namespace WpfApp2 {
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window {
    public MainWindow() {
      InitializeComponent();
    }

    private AutomationElement GetElementObj(AutomationElement appObj, String automationId) {
      //Creamos una nueva condición para buscar por la propiedad AutomationId
      System.Windows.Automation.Condition condition = new PropertyCondition(AutomationElement.AutomationIdProperty, automationId);
      //Aplicamos las condiciones de búsqueda a los elementos de la aplicación y devolvemos la primera referencia encontrada
      return appObj.FindFirst(TreeScope.Descendants, condition);
    }

    //Genera el mensaje de Click en el botón de la aplicación indicada
    //Devuelve true si lo consigue
    private void ClickRemoteButton(AutomationElement appObj,String automationId) {
      if (appObj == null) MessageBox.Show("Application is not running");
      else { //La aplicación está en ejecución
        //Obtenemos el objeto del ejemento
        AutomationElement myButton = GetElementObj(appObj, automationId);
        if (myButton == null) MessageBox.Show("Button not found"); 
        else { //Se ha encontrado el elemento que buscamos
          //Creamos el objeto que generará el mensage de pulsación del botón
          InvokePattern btnPattern = myButton.GetCurrentPattern(InvokePattern.Pattern) as InvokePattern;
          btnPattern.Invoke(); //Lanzamos el mensaje de pulsación del bótón
        }
      }
    }

    //Obtiene el objeto de la primera instancia en ejecución de la aplicación
    //Puede devolver null
    private AutomationElement GetAppObj() {
      //Creamos elemento raíz del árbol de aplicaciones en ejecución
      AutomationElement root = AutomationElement.RootElement;
      //Creamos condición que filtra por nombre de aplicación
      System.Windows.Automation.Condition condition = new PropertyCondition(AutomationElement.NameProperty, "Calculadora"); //Español
      //System.Windows.Automation.Condition condition = new PropertyCondition(AutomationElement.NameProperty, "Calculator"); //Inglés
      //Filtramos el árbol de aplicaciones en ejecución por nombre y guardamos el primer resultado obtenido
      return root.FindFirst(TreeScope.Children, condition);
    }
 
    private void ButtonOpen_Click(object sender, RoutedEventArgs e) {
      //Abrimos una nueva instancia de la aplicación
      System.Diagnostics.Process calc = System.Diagnostics.Process.Start("calc");
      //Esperamos un máximo de 5 segundos para que termine de abrirse
      calc.WaitForInputIdle(5000);
    }

    private void Button0_Click(object sender, RoutedEventArgs e) {
      ClickRemoteButton(GetAppObj(), "num0Button");
    }
    private void Button1_Click(object sender, RoutedEventArgs e) {
      ClickRemoteButton(GetAppObj(), "num1Button");
    }
    private void Button2_Click(object sender, RoutedEventArgs e) {
      ClickRemoteButton(GetAppObj(), "num2Button");
    }
    private void Button3_Click(object sender, RoutedEventArgs e) {
      ClickRemoteButton(GetAppObj(), "num3Button");
    }
    private void Button4_Click(object sender, RoutedEventArgs e) {
      ClickRemoteButton(GetAppObj(), "num4Button");
    }
    private void Button5_Click(object sender, RoutedEventArgs e) {
      ClickRemoteButton(GetAppObj(), "num5Button");
    }
    private void Button6_Click(object sender, RoutedEventArgs e) {
      ClickRemoteButton(GetAppObj(), "num6Button");
    }
    private void Button7_Click(object sender, RoutedEventArgs e) {
      ClickRemoteButton(GetAppObj(), "num7Button");
    }
    private void Button8_Click(object sender, RoutedEventArgs e) {
      ClickRemoteButton(GetAppObj(), "num8Button");
    }
    private void Button9_Click(object sender, RoutedEventArgs e) {
      ClickRemoteButton(GetAppObj(), "num9Button");
    }
    private void ButtonTimes_Click(object sender, RoutedEventArgs e) {
      ClickRemoteButton(GetAppObj(), "multiplyButton");
    }
    private void ButtonMinus_Click(object sender, RoutedEventArgs e) {
      ClickRemoteButton(GetAppObj(), "minusButton");
    }
    private void ButtonPlus_Click(object sender, RoutedEventArgs e) {
      ClickRemoteButton(GetAppObj(), "plusButton");
    }
    private void ButtonEqual_Click(object sender, RoutedEventArgs e) {
      ClickRemoteButton(GetAppObj(), "equalButton");
    }
    private void ButtonDecimal_Click(object sender, RoutedEventArgs e) {
      ClickRemoteButton(GetAppObj(), "decimalSeparatorButton");
    }
    private void ButtonDel_Click(object sender, RoutedEventArgs e) {
      ClickRemoteButton(GetAppObj(), "backSpaceButton");
    }
    private void ButtonGetScr_Click(object sender, RoutedEventArgs e) {
      AutomationElement appObj = GetAppObj(); //Obtenemos el objeto de la aplicación
      if (appObj == null) MessageBox.Show("Application is not running");
      else { //Si tenemos la aplicación en ejecución
        //Obtenemos el objeto del elemento que muestra el resultado (display)
        AutomationElement elementObj = GetElementObj(appObj, "CalculatorResults");
        if(elementObj == null) MessageBox.Show("Display element not found");
        else { //Se ha encontrado el elemento del display
          //Obtenemos el contenido del objeto...
          //La propiedad Name contiene el resultado actual de la operación, pero tiene un
          //prefijo añadido:
          //-Español: "La pantalla muestra "
          //-Inglés: "Display is "
          //Para que la eliminación del prefijo no dependa del idioma, buscaremos la posición
          //del último espacio y tomaremos sólo el resto.
          String res = elementObj.Current.Name; //Obtenemos el texto del elemento (contiene prefijo).
          //El texto que nos interesa está después del último espacio
          int lastSpaceIndex = res.LastIndexOf(" ") + 1;
          res = res.Substring(lastSpaceIndex); //Tomamos sólo la última palabra
          labelDisplay.Content = res; //Finalmente asignaremos el resultado al label
        }
      }
    }

    private void ButtonClose_Click(object sender, RoutedEventArgs e) {
      ClickRemoteButton(GetAppObj(), "Close");
    }
  }
}
