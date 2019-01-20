#include "datafont.h"

DataFont::DataFont()
{
    //Constructor

    //Cuando se crea una fuente sólo tiene un carácter. El cero
    charMin = charMax = 48;
    //Sólo tiene una página
    pages = 1;
    //Creamos el array de caracteres, con un sólo carácter
    dataChar = new DataChar[1];
    //Reservamos memoria para los datos del primer carácter
    //Tiene una anchura de 1
    dataChar[0].data=new byte[pages]();
    dataChar[0].width=1;
}

byte DataFont::charCount()
{
    //Devuelve el número de caracteres que tiene la fuente
    return (byte)(charMax - charMin + 1);
}

void DataFont::redimPages(unsigned char newPages)
{
    //Redimensiona las páginas de todos los caracteres de una fuente
    //Suponemos que el número de páginas ha cambiado

    //Anotamos el número de caracteres de la fuente
    byte charCount = this->charCount();
    //Recorremos todos los caracteres de la fuente
    for (byte c = 0; c < charCount; c++)
    {
       //Redimensionamos el carácter al nuevo número de páginas
       this->dataChar[c].redimPages(pages, newPages);
    }
    //Asignamos el nuevo número de páginas a la fuente
    pages = newPages;
}

void DataFont::redimCharMax(unsigned char newCharMax)
{
    //Redimensiona la fuente ajustando el último carácter
    //Suponemos que el último carácter ha cambiado

    //Calculamos el nuevo número de caracteres
    byte newCharCount = (byte)(newCharMax - charMin + 1);
    //Creamos un nuevo array de caracteres
    DataChar *newDataChar = new DataChar[newCharCount];
    //Si el nuevo último carácter es mayor que el anterior (si se amplia el número de caracteres)...
    if(newCharMax>charMax)
    {
        //Anotamos el número de caracteres actuales
        byte charCount=this->charCount();
        //Copiaremos los carácteres actuales
        for (byte c = 0; c < charCount; c++)
        {
            //Le asignamos los nuevos datos del carácter
            newDataChar[c].data=dataChar[c].data;
            //Le asignamos la misma anchura
            newDataChar[c].width=dataChar[c].width;
        }
        //Crearemos el resto
        for (byte c=charCount;c<newCharCount;c++)
        {
            //Deben ser caracteres vacíos. Con anchura 1
            //Reservamos memoria para los datos del carácter
            newDataChar[c].data=new byte[pages]();
            //Asignamos la anchura
            newDataChar[c].width=1;
        }
    }
    else //Se reduce el número de caracteres
    {
        //Copiaremos los carácteres que correspondan
        for (byte c=0; c<newCharCount; c++)
        {
            //Le asignamos los nuevos datos del carácter
            newDataChar[c].data=dataChar[c].data;
            //Le asignamos la misma anchura
            newDataChar[c].width=dataChar[c].width;
        }
    }
    //Liberamos la memoria del array de caracteres actual
    delete dataChar;
    //Asignamos el nuevo array de caracteres a la fuente
    dataChar = newDataChar;
    //Anotamos cuál es ahora el último carácter
    charMax = newCharMax;
}

void DataFont::redimCharMin(unsigned char newCharMin)
{
    //Redimensiona la fuente ajustando el primer carácter
    //Suponemos que el primer carácter ha cambiado

    //Calculamos el nuevo número de caracteres
    byte newCharCount = (byte)(charMax - newCharMin + 1);
    //Creamos un nuevo array de caracteres
    DataChar *newDataChar = new DataChar[newCharCount];
    //Si el nuevo primer carácter es menor que el anterior (si se amplia el núemro de caracteres)...
    if (newCharMin < charMin)
    {
       //Anotamos el número de caracteres nuevos
       byte diff = (byte)(charMin-newCharMin);
       //Crearemos los caracteres iniciales nuevos
       for (byte c = 0; c < diff; c++)
       {
           //Deben ser caracteres vacíos. Con anchura 1
           //Reservamos memoria para los datos del carácter
           newDataChar[c].data=new byte[pages]();
           //Asignamos la anchura
           newDataChar[c].width=1;
       }
       //Copiaremos los carácteres actuales
       for (byte c = diff; c < newCharCount; c++)
       {
           //Le asignamos los nuevos datos del carácter
           newDataChar[c].data=dataChar[c-diff].data;
           //Le asignamos la misma anchura
           newDataChar[c].width=dataChar[c-diff].width;
       }
    }
    else //Se reduce el número de caracteres
    {
       //Copiaremos los carácteres que correspondan
       for (byte c = 0; c < newCharCount; c++)
       {
           //Le asignamos los nuevos datos del carácter
           newDataChar[c].data=dataChar[c].data;
           //Le asignamos la misma anchura
           newDataChar[c].width=dataChar[c].width;
       }
    }
    //Liberamos la memoria del array de caracteres actual
    delete dataChar;
    //Asignamos el nuevo array de caracteres a la fuente
    dataChar = newDataChar;
    //Anotamos cuál es ahora el primer carácter
    charMin = newCharMin;
}

void DataFont::clear()
{
    //Borra los datos de todos los caracteres de la fuente

    //Anotamos el número de caracteres actuales
    byte charCount=this->charCount();
    //Recorremos todos los caracteres actuales y borramos sus datos
    for (byte c=0;c<charCount;c++) delete dataChar[c].data;
    //Borramos el array de caracteres
    delete dataChar;
}

