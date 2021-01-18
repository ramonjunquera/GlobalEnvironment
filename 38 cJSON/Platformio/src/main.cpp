/*
  Autor: Ramón Junquera
  Fecha: 20210117
  Tema: Librería cJSON
  Objetivo: Demo
  Material: M5Stack Atom Lite
  Descripción:
    La librería cJSON facilita el tratamiento de mensajes de texto formateados en JSON.
    La gran ventaja es que la librería está integrada dentro de las reconocidas por el
    entorno de desarrollo de Platformio y no es necesario descargarla ni tenerla
    disponible dentro de la carpeta lib del proyecto.

    Se muestran varios ejemplos de uso.
*/

#include <Arduino.h>
#include <cJSON.h>

//Creación de objeto JSON
void test01() { 
  Serial.println("----test01----");
  cJSON *obj=cJSON_CreateObject(); //Creamos objeto JSON
  if(obj==NULL) Serial.println("Error creando objeto");
  else { //El objeto se ha creado correctamente
    Serial.println(cJSON_Print(obj)); //Mostramos el objeto vacío
    cJSON_Delete(obj); //Liberamos memoria del objeto
  }
}

//Añadir un items de distintos tipos simples
void test02() { 
  Serial.println("----test02----");
  cJSON *obj=cJSON_CreateObject(); //Creamos objeto JSON
  if(obj==NULL) Serial.println("Error creando objeto");
  else { //El objeto se ha creado correctamente
    cJSON_AddStringToObject(obj,"stringItem","value1"); //Añadimos item de cadena
    cJSON_AddBoolToObject(obj,"boolItemByValue",false); //Añadimos item booleano con su valor
    cJSON_AddTrueToObject(obj,"boolTrueItem"); //Añadimos item booleano true
    cJSON_AddFalseToObject(obj,"boolFalseItem"); //Añadimos item booleano false
    cJSON_AddNumberToObject(obj,"intItem",58); //Añadimos item entero
    cJSON_AddNumberToObject(obj,"floatItem",-34.25); //Añadimos item decimal
    cJSON_AddNullToObject(obj,"nullItem"); //Añadimos objeto nulo
    Serial.println(cJSON_Print(obj)); //Mostramos el objeto vacío
    cJSON_Delete(obj); //Liberamos memoria del objeto
  }
}

//Reemplazar valores de items
void test03() { 
  Serial.println("----test03----");
  cJSON *obj=cJSON_CreateObject(); //Creamos objeto JSON
  if(obj==NULL) Serial.println("Error creando objeto");
  else { //El objeto se ha creado correctamente
    cJSON_AddStringToObject(obj,"stringItem","value1"); //Añadimos item de cadena
    cJSON_AddFalseToObject(obj,"boolItem"); //Añadimos item booleano false
    cJSON_AddNumberToObject(obj,"intItem",58); //Añadimos item entero
    Serial.println(cJSON_Print(obj)); //Mostramos estado inicial del objeto
    //Reemplazamos valor de item de cadena
    cJSON *newObj=cJSON_CreateString("newValue"); //Creamos una cadena JSON
    cJSON_ReplaceItemInObject(obj,"stringItem",newObj);
    //Reemplazamos valor de item booleano
    newObj=cJSON_CreateBool(true);
    cJSON_ReplaceItemInObject(obj,"boolItem",newObj);
    //Reemplazamos valor de item entero
    newObj=cJSON_CreateNumber(-21);
    cJSON_ReplaceItemInObject(obj,"intItem",newObj);
    Serial.println(cJSON_Print(obj)); //Mostramos el objeto actualizado
    cJSON_Delete(obj); //Liberamos memoria del objeto y sub-objetos contenidos
  }
}

//Obtención de valores de items
void test04() { 
  Serial.println("----test04----");
  cJSON *obj=cJSON_CreateObject(); //Creamos objeto JSON
  cJSON_AddNumberToObject(obj,"intItem",10); //Añadimos item entero
  cJSON_AddStringToObject(obj,"stringItem","value1"); //Añadimos item de cadena
  Serial.println(cJSON_Print(obj)); //Mostramos estado inicial del objeto

  //Añadimos 3 al item intItem
  cJSON *item=cJSON_GetObjectItem(obj,"intItem"); //Obtenemos el item intItem
  byte intValue=item->valueint; //Obtenemos su valor entero
  intValue+=3; //Le añadimos 3
  item=cJSON_CreateNumber(intValue); //Creamos un objeto de un número
  cJSON_ReplaceItemInObject(obj,"intItem",item); //Reemplazamos el valor de intItem por el nuevo objeto

  //Añadimos "hola" a stringItem
  item=cJSON_GetObjectItem(obj,"stringItem"); //Obtenemos el item stringItem
  String stringValue=String(item->valuestring); //Obtenemos su valor de cadena
  stringValue+="hola"; //Le añadimos "hola"
  item=cJSON_CreateString(stringValue.c_str()); //Creamos un objeto de un cadena
  cJSON_ReplaceItemInObject(obj,"stringItem",item); //Reemplazamos el valor de stringItem por el nuevo objeto

  Serial.println(cJSON_Print(obj)); //Mostramos el resultado

  //Hacemo lo mismo pero en una única línea
  cJSON_ReplaceItemInObject(obj,"intItem",cJSON_CreateNumber(cJSON_GetObjectItem(obj,"intItem")->valueint+3));
  cJSON_ReplaceItemInObject(obj,"stringItem",cJSON_CreateString((String(cJSON_GetObjectItem(obj,"stringItem")->valuestring)+"hola").c_str()));

  Serial.println(cJSON_Print(obj)); //Mostramos el resultado

  cJSON_Delete(obj); //Liberamos memoria del objeto y sub-objetos contenidos
}

//Gestión de arrays
void test05() {
  Serial.println("----test05----");
  cJSON *obj=cJSON_CreateObject(); //Creamos objeto JSON
  //- Crearemos un objeto array
  //- Añadiremos sus elementos
  //- Añadimos el array como un item al objeto JSON
  cJSON *arr=cJSON_CreateArray(); //Creamos array JSON
  cJSON *item=cJSON_CreateNumber(11);
  cJSON_AddItemToArray(arr,item);
  item=cJSON_CreateNumber(22);
  cJSON_AddItemToArray(arr,item);
  item=cJSON_CreateNumber(33);
  cJSON_AddItemToArray(arr,item);
  cJSON_AddItemToObject(obj,"intArray",arr);
  //- Añadimos un item array al objeto JSON
  //- Añadimos los elementos al array
  arr=cJSON_AddArrayToObject(obj,"stringArray");
  item=cJSON_CreateString("uno");
  cJSON_AddItemToArray(arr,item);
  item=cJSON_CreateString("dos");
  cJSON_AddItemToArray(arr,item);
  item=cJSON_CreateString("tres");
  cJSON_AddItemToArray(arr,item);
  //- Creamos una array de enteros JSON en base a un aray de enteros
  //- Añadimos el array como un item al objeto JSON
  int arrInt[3]={111,222,333};
  arr=cJSON_CreateIntArray(arrInt,3);
  cJSON_AddItemToObject(obj,"intArray2",arr);
  //- Creamos un array de entero en base a un string (parseo)
  //- Añadimos el array como un item al objeto JSON
  arr=cJSON_Parse("[1111,2222,3333]");
  cJSON_AddItemToObject(obj,"intArray3",arr);
  //- Creamos un array de cadenas en base a un string (parseo)
  //- Añadimos el array como un item al objeto JSON
  arr=cJSON_Parse("[\"UNO\",\"DOS\",\"TRES\"]");
  cJSON_AddItemToObject(obj,"stringArray2",arr);

  Serial.println(cJSON_Print(obj)); //Mostramos el resultado
  cJSON_Delete(obj); //Liberamos memoria del objeto y sub-objetos contenidos
}

//Modificación de elementos de arrays
void test06() {
  Serial.println("----test06----");
  cJSON *obj=cJSON_CreateObject(); //Creamos objeto JSON
  //Creamos un item cuyo valor es un array de enteros
  cJSON_AddItemToObject(obj,"intArray",cJSON_Parse("[11,22,33]"));
  Serial.println(cJSON_Print(obj)); //Mostramos el resultado inicial
  
  cJSON *arr=cJSON_GetObjectItem(obj,"intArray"); //Obtenemos el valor del item del array
  cJSON *valueJSON=cJSON_GetArrayItem(arr,1); //Obtenemos segundo elemento del array
  byte value=valueJSON->valueint; //Obtenemos el valor entero del elemento
  value+=10; //Le sumamos 10
  valueJSON=cJSON_CreateNumber(value); //Creamos un objeto JSON con un valor entero
  cJSON_ReplaceItemInArray(arr,1,valueJSON); //Reemplazamos el segundo elemento del array
  Serial.println(cJSON_Print(obj)); //Mostramos el resultado

  //Repetimos lo mismo en una sola línea
  cJSON_ReplaceItemInArray(cJSON_GetObjectItem(obj,"intArray"),1,cJSON_CreateNumber(cJSON_GetArrayItem(cJSON_GetObjectItem(obj,"intArray"),1)->valueint+10));
  Serial.println(cJSON_Print(obj)); //Mostramos el resultado

  cJSON_Delete(obj); //Liberamos memoria del objeto y sub-objetos contenidos
}

//Parseo complejo
void test07() {
  Serial.println("----test07----");
  String txt="\
    {\
     \"personas\":[\
       {\
         \"nombre\":\"Pedro\",\
         \"edad\":30\
       },\
       {\
         \"nombre\":\"Juan\",\
         \"edad\":23\
       },\
       {\
         \"nombre\":\"Jesús\",\
         \"edad\":33\
       }\
     ]\
    }\
  ";
  cJSON *obj=cJSON_Parse(txt.c_str()); //Creamos objeto JSON con un parseo
  Serial.println(cJSON_Print(obj)); //Mostramos el resultado
  txt="\
    {\
      \"desired\": {\
        \"led\": false\
      },\
      \"reported\": {\
        \"led\": true\
      },\
      \"delta\": {\
        \"led\": false\
      }\
    }\
  ";
  obj=cJSON_Parse(txt.c_str()); //Creamos objeto JSON con un parseo
  Serial.println(cJSON_Print(obj)); //Mostramos el resultado

  cJSON_Delete(obj); //Liberamos memoria del objeto y sub-objetos contenidos
}

//Identificación de items
void test08() {
  Serial.println("----test07----");
  String txt="\
    {\
      \"desired\": {\
        \"led\": false\
      },\
      \"reported\": {\
        \"led\": true\
      },\
      \"delta\": {\
        \"led\": false\
      }\
    }\
  ";
  cJSON *obj=cJSON_Parse(txt.c_str()); //Creamos objeto JSON con un parseo
  Serial.println(cJSON_Print(obj)); //Mostramos el resultado

  Serial.println("Tiene un item llamado delta? "+String(cJSON_HasObjectItem(obj,"reported")));
  Serial.println("Tiene un item llamado reparted? "+String(cJSON_HasObjectItem(obj,"reparted")));

  Serial.println("Items del objeto:");
  cJSON *item=obj->child; //Tomamos el primero de los items
  while(item!=NULL) { //Mientras el item exista...
    Serial.print("- ");
    Serial.println(item->string); //Mostramos el nombre del item
    item=item->next; //Pasamos al siguiente item
  }
  
  //Obtenemos el valor de /delta/led
  item=cJSON_GetObjectItem(obj,"delta"); //Obtenemos el item delta
  item=cJSON_GetObjectItem(item,"led"); //Obtenemos el item delta/item
  bool value=item->valueint; //Obtenemos valor de delta/item
  Serial.println("Valor de /delta/led="+String(value));

  //Todo en una línea
  Serial.println("Valor de /delta/led="+String(cJSON_GetObjectItem(cJSON_GetObjectItem(obj,"delta"),"led")->valueint));

  //Invertimos el valor de /delta/led
  Serial.println("Invertimos valor de /delta/led");
  item=cJSON_GetObjectItem(obj,"delta"); //Obtenemos el item /delta
  //- Obtenemos el valor de /delta/led
  //- Creamos un valor booleano opuesto
  //- Reemplazamos el item /delta/led con el nuevo valor booleano
  cJSON_ReplaceItemInObject(item,"led",cJSON_CreateBool(!cJSON_GetObjectItem(item,"led")->valueint));

  Serial.println(cJSON_Print(obj)); //Mostramos el resultado
  cJSON_Delete(obj); //Liberamos memoria del objeto y sub-objetos contenidos
}

void setup() {
  Serial.begin(115200);
  test01(); //Creación de objeto JSON
  test02(); //Añadir un item de cadena
  test03(); //Reemplazar el valor de un item String
  test04(); //Creación de arrays
  test05(); //Gestión de arrays
  test06(); //Modificación de elementos de arrays
  test07(); //Parseo complejo
  test08(); //Identificación de items
}

void loop() {
  //Nada especial que hacer aquí
  delay(1000);
}


