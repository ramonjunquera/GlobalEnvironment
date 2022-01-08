/*
 * Tema: Funciones delegadas / funciones lambda
 * Fecha: 20210907
 * Autor: Ramón Junquera
 * 
 * Descripción:
 * Las funciones delegadas son aquellas que se definen en tiempo de ejecución.
 * Lo que se define como función es la estructura
 * - Definición de parámetros
 * - Definición de resultado
 * Después ya se crearán las funciones delegadas (derivadas) de esta cuando corresponda.
 * Hay varias maneras de definir el contenido de una función delegada:
 * - Mediante una función Lambda
 * - Referenciando a otra función la creada
 * En el ejemplo se muestran ambos casos.
 * 
 * Funcionamiento:
 * Tomando como parámetros los dos TextBox superiores, definimos funciones delegadas segun
 * la pulsación del botón operación.
 * Mostramos el resultado en el Label inferior.
 */

using System;
using System.Windows;

namespace DelegateDemo {
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>

  //Clase externa a la principal
  class MyClass {
    //Definimos un método que será utilizado con función delegada desde la clase principal
    //Debe ser público para que se pueda acceder desde otras clases
    //Lo hacemos estático para no tener que instanciar la clase
    public static int MethodMult(int a,int b) {
      return a * b;
    }
  }
  
  //Definbición de la clase principal (de la ventana)
  public partial class MainWindow : Window {
    public MainWindow() {
      InitializeComponent();
    }

    //Definimos una función delegada con dos parámetros int, que devuelve un int
    private delegate int DelegateFunction(int a, int b);

    private void ButtonPlus_Click(object sender, RoutedEventArgs e) {
      //Creamos una función delegada basada en la estructura de DelegateFunction
      //La función la define una función Lambda
      DelegateFunction sum = (a, b) => {
        return a + b;
      }; //Importante el ; al final de la función Lambda
      //Convertimos parámetros a enteros. Ejecutamos la función delegada y mostramos el resultado en el Label
      labelRes.Content = sum(Convert.ToInt32(textbox1.Text), Convert.ToInt32(textbox2.Text)).ToString();
    }

    private void ButtonMinus_Click(object sender, RoutedEventArgs e) {
      //Creamos una función delegada basada en la estructura de DelegateFunction
      //La función la define una función Lambda, pero es tan sencilla (una sola línea) que ni siguiera utilizamos {}
      DelegateFunction subtract = (a, b) => a - b;
      //Convertimos parámetros a enteros. Ejecutamos la función delegada y mostramos el resultado en el Label
      labelRes.Content = subtract(Convert.ToInt32(textbox1.Text), Convert.ToInt32(textbox2.Text)).ToString();
    }

    private void ButtonStar_Click(object sender, RoutedEventArgs e) {
      //Creamos una función delegada basada en la estructura de DelegateFunction
      //La función la como el método de una clase externa. La única restricción es que tenga la misma estructura
      //de parámetros y resultado.
      DelegateFunction mult = MyClass.MethodMult;
      //Convertimos parámetros a enteros. Ejecutamos la función delegada y mostramos el resultado en el Label
      labelRes.Content = mult(Convert.ToInt32(textbox1.Text), Convert.ToInt32(textbox2.Text)).ToString();
    }
  }
}
