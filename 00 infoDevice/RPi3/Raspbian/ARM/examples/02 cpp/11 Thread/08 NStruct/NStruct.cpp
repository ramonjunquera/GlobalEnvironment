/*
 * Autor: Ramón Junquera
 * Tema: Programación en C++
 * Objetivo: Estructura de control de un thread
 * Material: Raspberry Pi
 * Descripción:
 * Creamos una estructura en la que guardaremos toda la información de
 * un thread.
 * La estructura contiene
 * - void (*functionPointer)() : el puntero de la función a ejecutar
 * - bool run : flag que lanzará la ejecución de la función
 * - bool running : flag que indica si la función se está ejecutando
 * Crearemos un diccionario cuyos nodos corresponden a una estructura.
 * Creamos dos nodos y los añadimos al diccionario.
 * Cada nodo apunta a una función distinta:
 * - Function1 : tarda 1 segundo en finalizar
 * - Function2 : tarda 2 segundos en finalizar
 * Llamamos con un thread a la función ThreadLoop que es la encargada de
 * comprobar contínuamente si hay algún nodo del diccionario que tiene
 * activo el flag "run" de petición de ejecución.
 * Cuando lo encuentra y si actualemente no se está ejecutando esa
 * función (evitamos la ejecución concurrente de la misma función),
 * entonces anota que la ejecución está activa y deshabilita el flag de 
 * petición de ejecución.
 * A continuación llama con un thread a la función FunctionLauncher
 * encargada de lanzar la función que corresponde y esperar a su
 * finalización para anotar en el flag running que ya ha terminado.
 * El thread se desvincula del programa principal para no preocuparnos
 * de tener que hacer join.
 * 
 * En el ejemplo crearemos el diccionario y realizaremos las siguientes
 * acciones:
 * - Tras esperar un par de segundos, pedimos la ejecución de la 
 *   función 1
 * - Esperamos otros dos segundos.
 * - Mientras esperamos la función 1 finaliza porque sólo tarda 1
 *   segundo en terminar.
 * - Solicitamos la ejecución de la funciones 1 y 2 y esperamos 1
 *   segundo.
 * - La función 1 es posible que termine o esté a punto, pero a la
 *   función 2 todavía le queda 1 segundo. Aunque no ha terminado, 
 *   activamos de nuevo la petición de ejecución de la función 2.
 *   Lo que ocurre en el bucle principal es que no lanza la función 2
 *   porque aunque tiene el flag "run" activo, todavía está en ejecución.
 * - Esperamos 4 segundos
 * - Mientras tanto la primera solicitud de ejecución de la función 2
 *   finalizará y a continuación se volverá a lanzar atendiendo a
 *   nuestra segunda petición. Antes de terminar nuestra espera,
 * la función 2 finalizará.
 * - Eliminamos los nodos
 * - Sin nodos el bucle principal finaliza sólo
 */


#include <thread> //Para gestionar threads
#include <unordered_map> //Para el diccionario
#include <chrono> //Para el tiempo de sleep_for
#include <iostream>

using namespace std;

struct functionInfo
{
	//Definimos la estructura con los detalles de una función
	
	//Puntero a función sin parámetros
	void (*functionPointer)(); 
	//Se debe ejecutar?. Por defecto no.
	bool run=false;
	//Está en ejecución?. Por defecto no.
	bool running=false;
};

void FunctionLauncher(functionInfo *fi)
{
	//Función que lanza una función determinada definida por la 
	//estuctura de información de función
	//Debe ser llamada como thread
	
	//Llamamos a la función
	fi->functionPointer();
	//Hemos terminado de ejecutar la función
	//Ya no está en ejecución
	fi->running= false;
} 

void ThreadLoop(unordered_map<int,functionInfo> *dicc)
{
	//Función para el bucle principal de ejecución
	//Debe ser llamada como thread
	//Se le pasa el diccionario de funciones a ejecutar
	
	cout << "ThreadLoop start" << endl;
	
	//Mientras haya algún nodo en el diccionario...
	while(dicc->size()>0)
	{
		//Recorreremos todos los nodos del diccionario
		for(auto &itr : *dicc)
		{
			//Si la función no está en ejecución y se debe lanzar...
			if(!itr.second.running && itr.second.run)
			{
				//Anotamos que el thread ya está en ejecución
				itr.second.running=true;
				//Desactivamos el flag de petición de ejecución
				itr.second.run=false;
				
				//Ejecutamos la función pasando como parámetro sus datos
				thread thread1(FunctionLauncher,&(itr.second)); 
				//Desvinculamos el thread del programa principal
				thread1.detach();
			}
		}
	}
	cout << "ThreadLoop end" << endl;
}

void Function1()
{
	//Ejemplo de función
	
	//Informamos que se ejecuta la función
	cout << "Start Function1" << endl;
	//Esperamos 1 segundo
	this_thread::sleep_for(chrono::seconds(1));
	//Informamos que se ejecuta la función
	cout << "End Function1" << endl;
}

void Function2()
{
	//Ejemplo de función
	
	//Informamos que se ejecuta la función
	cout << "Start Function2" << endl;
	//Esperamos 2 segundos
	this_thread::sleep_for(chrono::seconds(2));
	//Informamos que se ejecuta la función
	cout << "End Function2" << endl;
}

int main(void)
{
	//Creamos un diccionaro que tiene como clave un entero y como
	//dato una estructura functionInfo
	unordered_map<int,functionInfo> functionsDicc;
	
	//Creamos una estructura para guardar los datos de una función
	functionInfo myFunctionInfo1;
	//Indicamos la función a llamar
	myFunctionInfo1.functionPointer = Function1;
	//Añadimos la estructura al diccionario
	functionsDicc.emplace(1,myFunctionInfo1);
	
	//Repetimos para otra función
	//Creamos una estructura para guardar los datos de una función
	functionInfo myFunctionInfo2;
	//Indicamos la función a llamar
	myFunctionInfo2.functionPointer = Function2;
	//Añadimos la estructura al diccionario
	functionsDicc.emplace(2,myFunctionInfo2);
	
	//Tenemos un diccionario con tantos nodos como funciones queremos que
	//ejecute.
	//Lanzamos el bucle principal con un thread y le pasamos el
	//diccionario de funciones
	thread thread1(ThreadLoop,&functionsDicc);
	//Desvinculamos el thread que controla el bucle principal para no
	//tener que preocuparnos por el join. Sabemos que cuando el
	//diccionario esté vacío, finalizará el thread
	thread1.detach();

	//Esperamos 2 segundos
	this_thread::sleep_for(chrono::seconds(2));
	//Pedimos que se ejecute la función 1
	functionsDicc.at(1).run=true;
	//Esperamos 2 segundos
	this_thread::sleep_for(chrono::seconds(2));
	//La función 1 dura 1 segundo. Debería haber terminado.
	//Pedimos que se ejecute de nuevo la función 1
	functionsDicc.at(1).run=true;
	//Pedimos que se ejecute la función 2
	functionsDicc.at(2).run=true;
	//Esperamos 1 segundo
	this_thread::sleep_for(chrono::seconds(1));
	//La función 2 tarda 2 segundos y sólo hemos esperado 1
	//Todavía estárá en ejecución
	//Aun así activamos el flag de ejecución para cuando pueda
	functionsDicc.at(2).run=true;
	//Esperamos 4 segundos
	this_thread::sleep_for(chrono::seconds(4));
	//Eliminamos las 2 referencias del diccionario
	functionsDicc.erase(1);
	functionsDicc.erase(2);
	//Esperamos 1 segundo para darle tiempo a que finalice el bucle
	//principal. No es obligatorio, pero si lo hacemos veremos todos
	//los mensajes.
	this_thread::sleep_for(chrono::seconds(4));
	
    //Hemos terminado. Todo ok
    return 0;
}
