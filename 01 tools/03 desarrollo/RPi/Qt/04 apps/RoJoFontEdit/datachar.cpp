#include "datachar.h"

DataChar::DataChar()
{
    //Constructor

    //Inicialmente el carácter estará vacío y no tendrá anchura
    width=0;
}

void DataChar::redimWidth(byte newWidth,byte pages)
{
    //Redimensiona la anchura de un carácter
    //Suponemos que la anchura es distinta

    //Creamos un array de bytes con la nueva anchura
    byte *newData = new byte[newWidth * pages]();
    //Si la anchura es mayor (si se han añadido columnas)...
    if (newWidth>width)
    {
        //Copiamos en el nuevo array los actuales valores
        for (byte p = 0; p < pages; p++)
            for (int x = 0; x < width; x++)
                newData[x + p * newWidth] = this->data[x + p * width];
        //No es necesario rellenar de ceros las columnas nuevas
    }
    else //La nueva anchura es menor que la actual (si se han eliminado columnas)
    {
        //Copiamos en el nuevo array los actuales valores
        for (byte p = 0; p < pages; p++)
            for (int x = 0; x < newWidth; x++)
                newData[x + p * newWidth] = this->data[x + p * width];
    }
    //Anotamos la nueva anchura
    width = newWidth;
    //Liberamos la memoria reservada actualmente
    delete this->data;
    //Asignamos el nuevo array al carácter
    this->data = newData;
}

void DataChar::redimPages(byte oldPages,byte newPages)
{
    //Redimensiona las páginas de un carácter
    //Suponemos que el número de páginas ha cambiado

    //Creamos un array de bytes con el nuevo número de páginas
    byte *newData = new byte[width * newPages]();
    //Si se ha reducido el número de páginas...
    if (newPages < oldPages)
    {
        //Copiamos sólo los bytes de las primeras páginas
        for (byte p = 0; p < newPages; p++)
            for (byte b = 0; b < width; b++)
                newData[p * width + b] = this->data[p * width + b];
    }
    else //Si ha aumentado el número de páginas...
    {
        //Copiamos los bytes de las páginas actuales
        for (byte p = 0; p < oldPages; p++)
            for (byte b = 0; b < width; b++)
                newData[p * width + b] = this->data[p * width + b];
        //No es necesario rellenar de ceros las páginas nuevas
    }
    //Liberamos la memoria reservada actualmente
    delete this->data;
    //Asignamos el nuevo array al carácter
    this->data = newData;
}

