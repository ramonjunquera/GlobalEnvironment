/*
 * Autor: Ramón Junquera
 * Tema: Programación en C++
 * Objetivo: Tablas hash
 * Material: Raspberry Pi
 * Descripción:
 * Una tabla hash es lo mismo que un diccionario en tecnología .NET.
 * Se trata de un array de dos campos.
 * El primero de ellos llamado clave (key) y el segundo valor (value).
 * Los datos quedan indexados por el campo clave.
 * El campo clave es único (no permite duplicidades).
 * Los tipos de ambos campos son seleccionables.
 * 
 * Se gestiona con la librería unordered_map.
 * 
 * En nuestro ejemplo utilizaremos el tipo string para el campo clave y
 * int para el valor.
 * 
 * Se muestra cómo crear un objeto unordered_map.
 * Cómo añadir registros.
 * Cómo mostrar el número total de registros
 * Cómo utilizar un iterator para recorrer y mostrar todos los registros
 * Cómo acceder al valor de una clave.
 * Cómo comprobar si existe una clave o una lista de ellas.
 * Cómo eliminar un registro.
 * Cómo eliminar todos los registros.
 * Cómo crear y añadir registros en un sólo paso.
 * Cómo modificar los valores de una clave.
 * Cómo eliminar un rango de registros.
 * Y por último, comprobamos cómo no se pueden introducir registros con
 * una clave existente.
 */

#include <iostream>
#include <unordered_map>

using namespace std;

int main(int argc, char **argv)
{
	//Declaramos la tabla hash para que utilice strings como clave
	//y enteros como valor
	unordered_map<string,int> myDicc;
	
	//Añadimos varios valores a la tabla
	myDicc.emplace("Lucas",45);
	myDicc.emplace("Juan",34);
	myDicc.emplace("Mateo",40);
	myDicc.emplace("Marcos",37);
	
	//Mostramos el número de registros
	cout << "Número de registros: " << myDicc.size() << endl;
	
	//Mostramos el contenido de la tabla
	//Recorremos todos los registros de la tabla con un iterator
	for(auto &itr : myDicc)
	{
		//Mostramos la primera columna de la tabla (key)
		//Mostramos la segunda columna de la tabla (value)
		cout << itr.first << " : " << itr.second << endl;
	}
	
	cout << "Edad de Juan: " << myDicc["Juan"] << endl;
	cout << "Veces que aparece Pedro: " << myDicc.count("Pedro") << endl;
	cout << "Veces que aparece Mateo: " << myDicc.count("Mateo") << endl;
	cout << "Veces que aparece mateo: " << myDicc.count("mateo") << endl;

	//Comprobamos si existen los elementos de una lista
	for(auto &x : {"Marcos","David","Lucas"})
	{
		cout << x << (myDicc.count(x)>0?" ":" no ") << "está\n";
	}
	
	//Eliminamos un elemento
	cout << "Mateo ha sido eliminado\n";
	myDicc.erase("Mateo");
	
	//Mostramos de nuevo el contenido de la tabla
	for(auto &itr : myDicc) cout << itr.first << " : " << itr.second << endl;
	
	//Vaciamos la tabla
	cout << "Tabla vaciada\n";
	myDicc.clear();
	
	//Creamos una nueva tabla indicando los valores que contendrá
	//Atención: realmente los valores quedan en el orden inverso
	//El último elemento que entra es el primero que queda en la tabla
	unordered_map<string,int> myDicc1 =
	{
		{"Lucas",45},
		{"Juan",34},
		{"Mateo",40},
		{"Marcos",37}
	};
	
	//Mostramos el contenido de la tabla
	for(auto &itr : myDicc1) cout << itr.first << " : " << itr.second << endl;
	
	//Cambiamos algunos valores
	//Ya sea con la función at o como si fuese un array
	myDicc1.at("Juan") = 44;
	myDicc1.at("Marcos") += 13;
	myDicc1["Lucas"]=myDicc1.at("Marcos");
	cout << "Valores alterados\n";
	
	//Mostramos de nuevo el contenido de la tabla
	for(auto &itr : myDicc1) cout << itr.first << " : " << itr.second << endl;
	
	//Eliminamos desde Juan al final
	myDicc1.erase(myDicc1.find("Juan"),myDicc1.end());
	cout << "Eliminados desde Juan al final\n";
	
	//Mostramos de nuevo el contenido de la tabla
	for(auto &itr : myDicc1) cout << itr.first << " : " << itr.second << endl;

	//Intentamos añadir un elemento existente con un nuevo valor
	//Cuando esto ocurre, NO se añade un nuevo elemento, porque la clave
	//debe ser única.
	//Tampoco cambia el valor asociado a la clave
	myDicc1.emplace("Marcos",45);
	cout << "Añadido Marcos con 45\n";
	
	//Mostramos de nuevo el contenido de la tabla
	for(auto &itr : myDicc1) cout << itr.first << " : " << itr.second << endl;

	//Todo ok
	return 0;
}

