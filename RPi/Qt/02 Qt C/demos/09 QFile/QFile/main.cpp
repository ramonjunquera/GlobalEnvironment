#include <QCoreApplication>
#include <iostream> //Para cout
#include <QFile>
#include <QTextStream>

using namespace std; //Para cout

void saveText()
{
    //Ejemplo de escritura de un archivo de texto
    //Creamos un objeto QFile
    QFile f;
    //Le asignamos un nombre de archivo
    f.setFileName("textfile.txt");
    //Podríamos haber abierto el archivo y asignado el nombre utilizando
    //otro constructor:
    //  QFile f("textfile,txt");
    //Abrimos el archivo para sólo escritura como texto
    //Si no se consigue...terminamos
    if(!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    f.write("Hola\n");
    f.write("Esto es una prueba\n");
    f.write("Adiós");
    //Cerramos el archivo
    f.close();
    cout << "Guardado archivo de texto" << endl;
}

void readText()
{
    //Leemos el texto de un archivo
    //Creamos el objeto QFile
    QFile f("textfile.txt");
    //Lo abrimos como sólo lectura y de texto
    //Si no se puede...hemos terminado
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    //Creamos el objeto QTextStream para gestionar la lectura
    //del QFile abierto
    QTextStream ts(&f);
    //Separamos los resultados del ejercicio anterior
    cout << endl;
    //Mientras no se alcance el final...
    while(!ts.atEnd())
    {
        //Leemos la siguiente línea del archivo
        QString s = ts.readLine();
        //Mostramos el texto leido
        //Puesto que cout no permite mostrar variables QString directamente
        //lo convertimos a string estándar
        cout << s.toStdString() << endl;
    }
    //Cerramos el archivo
    f.close();
}

void saveBin()
{
    //Ejemplo de escritura de un archivo de datos
    //Creamos un objeto QFile
    QFile f("datafile.dat");
    //Abrimos el archivo para sólo escritura
    //Si no se consigue...terminamos
    if(!f.open(QIODevice::WriteOnly)) return;
    //Creamos algunas variables simples
    int v1=42;
    double v2=321.77;
    float v3=234.56;
    //Guardamos cada una de ellas
    //write sólo admite punteros char. Debemos hace un cast en todos los casos
    f.write((char*)&v1,sizeof(int));
    f.write((char*)&v2,sizeof(double));
    f.write((char*)&v3,sizeof(float));
    //Cerramos el archivo
    f.close();
    cout << endl << "Guardado archivo de datos" << endl;
    cout << v1 << "," << v2 << "," <<  v3 << endl;
}

void readBin()
{
    //Ejemplo de lectura de un archivo de datos
    //Creamos un objeto QFile
    QFile f("datafile.dat");
    //Abrimos el archivo para sólo escritura
    //Si no se consigue...terminamos
    if(!f.open(QIODevice::ReadOnly)) return;
    //Creamos las variables donde guardaremos los datos
    int v1;
    double v2;
    float v3;
    //Leemos cada una de ellas
    //read sólo admite punteros char. Debemos hace un cast en todos los casos
    f.read((char*)&v1,sizeof(int));
    f.read((char*)&v2,sizeof(double));
    f.read((char*)&v3,sizeof(float));
    //Cerramos el archivo
    f.close();
    cout << endl << "Leido archivo de datos" << endl;
    cout << v1 << "," << v2 << "," <<  v3 << endl;
}

void saveBinArray()
{
    //Ejemplo de escritura de un array en un archivo de datos
    //Creamos un objeto QFile
    QFile f("datafile.dat");
    //Abrimos el archivo para sólo escritura
    //Si no se consigue...terminamos
    if(!f.open(QIODevice::WriteOnly)) return;
    //Crearemos un array de enteros de un tamaño definido
    //que contendrá números consecutivos a partir del 0
    const int itemsCount = 14;
    int *myArray = new int[itemsCount];
    for(int i=0;i<itemsCount;i++) myArray[i]=i;
    //Guardamos el número de elementos del array
    f.write((char*)&itemsCount,sizeof(int));
    //Guardamos el array completo
    f.write((char*)myArray,sizeof(int)*itemsCount);
    //Cerramos el archivo
    f.close();
    cout << endl << "Guardado archivo de datos con array" << endl;
    for(int i=0;i<itemsCount;i++) cout << myArray[i] << " ";
    cout << endl;
    //Ya no necesitamos el array
    delete[] myArray;
}


void readBinArray()
{
    //Ejemplo de lectura de un array en un archivo de datos
    //Creamos un objeto QFile
    QFile f("datafile.dat");
    //Abrimos el archivo para sólo lectura
    //Si no se consigue...terminamos
    if(!f.open(QIODevice::ReadOnly)) return;
    //Creamos la variable que mantendrá el número de items del array
    int itemsCount;
    //Leemos el número de items del archivo de datos
    f.read((char*)&itemsCount,sizeof(int));
    //Creamos un array y reservamos memoria para todos sus items
    int *myArray = new int[itemsCount];
    //Leemos todos el array del archivo
    f.read((char*)myArray,sizeof(int)*itemsCount);
    //Cerramos el archivo
    f.close();
    cout << endl << "Leido archivo de datos con array" << endl;
    for(int i=0;i<itemsCount;i++) cout << myArray[i] << " ";
    cout << endl;
    //Ya no necesitamos el array
    delete[] myArray;
}

struct point
{
    //Definición de una estructura simple
    //Sólo contiene las coordenadas de un punto
    int x,y;
};


void saveBinArrayStruct()
{
    //Ejemplo de escritura de un array de estructuras en un archivo de datos
    //Creamos un objeto QFile
    QFile f("datafile.dat");
    //Abrimos el archivo para sólo escritura
    //Si no se consigue...terminamos
    if(!f.open(QIODevice::WriteOnly)) return;
    //Crearemos un array de estructuras point de un tamaño definido
    const int itemsCount = 3;
    struct point *myArray = new struct point[itemsCount];
    for(int i=0;i<itemsCount;i++)
    {
        myArray[i].x=i;
        myArray[i].y=i*2;
    }
    //Guardamos el número de elementos del array
    f.write((char*)&itemsCount,sizeof(int));
    //Guardamos el array completo
    f.write((char*)myArray,sizeof(struct point)*itemsCount);
    //Cerramos el archivo
    f.close();
    cout << endl << "Guardado archivo de datos con array de struct" << endl;
    for(int i=0;i<itemsCount;i++) cout << myArray[i].x << "," << myArray[i].y << " ";
    cout << endl;
    //Ya no necesitamos el array
    delete[] myArray;
}

void readBinArrayStruct()
{
    //Ejemplo de lectura de un array de estructuras en un archivo de datos
    //Creamos un objeto QFile
    QFile f("datafile.dat");
    //Abrimos el archivo para sólo lectura
    //Si no se consigue...terminamos
    if(!f.open(QIODevice::ReadOnly)) return;
    //Creamos la variable que mantendrá el número de items del array
    int itemsCount;
    //Leemos el número de items del archivo de datos
    f.read((char*)&itemsCount,sizeof(int));
    //Creamos un array de struct point y reservamos memoria para todos sus items
    struct point *myArray = new struct point[itemsCount];
    //Leemos todos el array del archivo
    f.read((char*)myArray,sizeof(struct point)*itemsCount);
    //Cerramos el archivo
    f.close();
    cout << endl << "Leido archivo de datos con array de struct" << endl;
    for(int i=0;i<itemsCount;i++) cout << myArray[i].x << "," << myArray[i].y << " ";
    cout << endl;
    //Ya no necesitamos el array
    delete[] myArray;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //Guardar texto en una archivo con QFile
    saveText();
    //Leer texto de un archivo
    readText();
    //Guarda información en archivo de datos
    saveBin();
    //Lee archivo de datos
    readBin();
    //Guarda array en archivo de datos
    saveBinArray();
    //Lee array de archivo de datos
    readBinArray();
    //Guarda array de struct en archivo de datos
    saveBinArrayStruct();
    //Lee array de struct en archivo de datos
    readBinArrayStruct();

    return a.exec();
}
