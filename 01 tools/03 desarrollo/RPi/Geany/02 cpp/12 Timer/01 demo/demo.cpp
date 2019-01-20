/*
 * Autor: Ramón Junquera
 * Tema: Programación en C++
 * Objetivo: Aprender a utilizar el timer setitimer
 * Material: Raspberry Pi
 * Descripción:
 * 
 * Linux permite generar interrupciones por software que saltan en base
 * a un timer.
 * Al timer se le especifican dos valores:
 * - El tiempo que debe transcurrir para su primera ejecución
 * - El tiempo entre repeticiones
 * Cuando este tiempo transcurra, el kernel genera una señal.
 * Si hemos definido una función que deba ser ejecutada cuando se reciba
 * esa señal, tendremos la interrupción por software operativa.
 * 
 * La función setitimer permite una resolución en microsegundos, aunque
 * no es exacta.
 * 
 * El timer es único para cada proceso. Esto significa que sólo podemos
 * definir una función a la que llamar cuando el timer salte.
 * 
 * Lo primero que deberíamos hacer es crear la función de llamada del
 * timer.
 * La función no devuelve nada y tiene un parámetro tipo int obligatorio
 * El parámetro indicará el tipo de señal que lo ha generado.
 * Aunque la señal saltará cuando haya transcurrido el tiempo
 * especificado, podemos contar este tiempo de tres maneras distintas,
 * y cada una de ellas generará una señal distinta.
 * 
 * Tipos de contadores de tiempo:
 * ITIMER_REAL. Cuenta el tiempo real de reloj con independencia de lo
 *   que haga el programa. Es la opción más utilizada.
 * ITIMER_VIRTUAL. Cuenta el tiempo de ejecución del proceso, sin contar
 *   las llamadas al kernel.
 * ITIMER_PROF. Cuenta el tiempo de ejecución del proceso, contando las
 *   llamadas al kernel.
 * 
 * Dependiendo de cómo contemos el tiempo, la señal generada será
 * distinta.
 * 
 * tipo de contador  -->  tipo de señal
 * ----------------       -------------
 *  ITIMER_REAL             SIGALRM
 *  ITIMER_VIRTUAL          SIGVTALRM
 *  ITIMER_PROF             SIGPROF
 * 
 * Por lo tanto, si contamos el tiempo real usando un contador de tipo
 * ITIMER_REAL, se generará una señal SIGALRM.
 * Este mismo tipo de señal es el que se recibe en el primero de los 
 * parámetros de la función a la que se llama cuando salta.
 * 
 * En el programa principal lo primero que hacemos es asociar el tipo de
 * señal generada con su correspondiente función, con signal:
 * Ej: signal(SIGALRM,catch_alarm);
 * Esto asocia el tipo de señal SIGALRM con la función catch_alarm()
 * 
 * La carácterísticas de un timer son:
 * - Tipo de contador utilizar, que genera determinada señal.
 * - La función a la que se llamará cuando se genere la señal
 * - Cuándo debe comenzar el timer
 * - Si se repite, cada cuánto tiepo lo hace.
 * 
 * Las dos primeras características ya las tenemos gestionadas.
 * Para los tiempos, necesitaremos una estructura de tipo itimerval
 * Esta estructura tiene 2 elementos:
 *   it_interval : tiempo transcurrido entre 2 señales (repetición)
 *   it_value : tiempo de espera para la primera ejecución
 * Si it_interval es cero, la señal sólo se generará una vez y no tendrá
 * repetición.
 * Si it_value es cero, se supone que queremos desactivar el timer.
 * 
 * A su vez, cada uno de estos elementos se subdividen en dos:
 *   tv_sec : tiempo en segundos
 *   tv_usec : tiempo en microsegundos
 * El tiempo total será la suma de ambos. Por lo tanto, podemos
 * especificar cualquier tiempo con una resolución de microsegundos.
 * 
 * Como sólo tenemos un timer, es posible que alguna vez necesitemos
 * consultar la configuración actual. Para ello utilizaremos la función
 *   int getitimer(int counterType,struct itimerval *timerConfig)
 * Devuelve -1 ante cualquier error.
 * Por ejemplo:
 *   //Definimos la estructura donde guardaremos la configuración
 *   struct itimerval timerConfig;
 *   //Obtenemos la configuración actual para el contador del tipo ITIMER_REAL
 *   int answer = getitimer(ITIMER_REAL,&timerConfig);
 *   //Si ha ocurrido algún error...terminamos
 *   if(answer) return;
 * 
 * Finalmente sólo nos queda aplicar la configuración deseada al timer
 * Creamos una variable con estructura itimerval.
 * Asignamos los tiempos de primera ejecución y repetición.
 * Aplicamos la configuración con la función:
 *   int setitimer(int counterType,struct itimerval *timerConfig,struct itimerval *timerOldConfig)
 * Devuelve -1 ante cualquier error.
 * El primer parámetro es el tipo de contador.
 * El segundo es la estructura itimerval que contiene la configuración.
 * El tercer parámetro es similar al segundo, pero guardará en esa
 * variable la configuración que tiene actualmente antes de aplicar la
 * solicitada. El segultado serái el mismo que obtendríamos con la
 * función getitimer.
 * Este parámetro no es obligatorio. Podemos sustituirlo por NULL y
 * entonces no se tendrá en cuenta la configuración actual.
 * Tenemos un ejemplo de uso en el siguiente ejemplo.
 * 
 * Qué ocurre si ante un timer periódico, la función de interrupción
 * tarda más que el periodo del timer?.
 * Mientras se está ejecutando la función de interrupción de un timer NO
 * se producen nuevas llamadas a la función.
 * Pero sí queda anotado que se ha intentado ejecutar la función pero no
 * se ha podido porque no había terminado.
 * En cuanto la función termina, se comprueba que existe una petición
 * de ejecución pendiente y se vuelve a lanzar.
 */
 
#include <sys/time.h> //para itimerval
#include <signal.h> //para signal
#include <iostream>
using namespace std;

void catch_alarm (int signalType)
{
	//Función a la que llama la interrupción
	//Es obligatorio que tengo un parámetro de tipo int
	//El parámetro indica qué tipo de señal ha producido la llamada
	//En este caso siempre será SIGALRM
	
	//Mostramos el tiempo que lleva el programa en ejecución en
	//microsegundos
	cout << "Tiempo de ejecución en microsegundos: " << clock() << endl;
}
 
int

main(void)
{
	//Indicamos que cuando se produzca una señal/evento del tipo SIGALRM
	//Se llame a la función catch_alarm
	//La señal SIGALRM se produce cuando el tiempo real llega a un
	//valor determinado
	signal(SIGALRM,catch_alarm);
	
	//Creamos una estructura en al que guardaremos los parámetros de 
	//funcionamiento del timer
	struct itimerval timerConfig;

	//Definimos el tiempo que hay entre dos eventos (1 segundo)
	timerConfig.it_interval.tv_sec = 1; //Tiempo en segundos
	timerConfig.it_interval.tv_usec = 0; //Tiempo de microsegundos
	//Definimos el tiempo que debe transcurrir para el primer evento
	//Si seleccionamos tiempo 0, lo desactivaremos
	//Indicaremos que el primer evento se lanzará medio segundo después
	timerConfig.it_value.tv_sec = 0; //Tiempo en segundos
	timerConfig.it_value.tv_usec = 500000; //Tiempo en microsegundos

	//Arrancamos el timer con los parámetros definidos
	//ITIMER_REAL corresponde con con el reloj de tiempo real, que 
	//generará al terminar una señal de tipo SIGALRM
	//En el segundo parámetro enviamos el puntero de la estructura que
	//guarda la configuración que tendrá el timer
	//En el tercer parámetro enviamos un NULL porque no nos importa la
	//posible configuración que pueda tener anteriormente
	//Podríamos enviar otro puntero con la estructura itimerval y se nos
	//rellenaría con la configuración que tenía hasta ahora
	//La razón es que sólo tenemos un timer
	//Si no conseguimos crear el timer...terminamos con error
	if (setitimer(ITIMER_REAL,&timerConfig,NULL) < 0)
	{
		cout << "Error al crear el timer" << endl;
		return 1;
	}
	//Hemos podido crear el timer correctamente
      
	//Bucle infinito
	while(1)
	{
		//Nada especial que hacer en el bucle
	}
	 
	//Todo ok
	return 0;
}
