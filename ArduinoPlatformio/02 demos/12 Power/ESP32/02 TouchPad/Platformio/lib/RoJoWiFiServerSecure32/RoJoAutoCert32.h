/*
  Nombre de la librería: RoJoAutoCert32.h
  Autor: Ramón Junquera
  Fecha: 20211021
  Descripción:
    Generación de certificados autofirmados
    La librería no contiene una clase. Sólo una única función encargada de la 
    generación de los archivos en SPIFFS para la clave privada y su correspondiente
    certificado autofirmado.

    Tiempo aproximado de generación de clave privada y certificado en base a la 
    longitud de clave de encriptación.
      1024 bits :    4s
      2048 bits :   13s
      4096 bits : 2m54s
*/

//Impedimos que se defina los métodos más de una vez
#ifndef RoJoAutoCert32_H
#define RoJoAutoCert32_H

#include <Arduino.h>
#include <SPIFFS.h> //Para acceso a archivos de certificado y clave
#include <mbedtls/entropy.h> //Para contexto de entropía
#include <mbedtls/ctr_drbg.h> //Para contexto drbg
#include <mbedtls/pk.h> //Para objeto de clave privada
#include <mbedtls/x509_crt.h> //Para objeto de certificado
#include <WiFi.h>

//Genera una clave privada y un certificado autofirmado y los guarda en archivos en SPFISS.
//Parámetros:
//  certFileName : nombre y path del archivo que contiene el certificado (Ej. "/autocert.crt")
//  pkFileName : nombre y path del archivo que contiene la clave privada (Ej. "/private.key")
//  name : nombre descriptivo del certificado en formato "CN=esp32.local,O=MyCompany,C=SP"
//  validityFrom : Cadena de fecha de validez desde en formato "YYYYMMDDhhmmss"
//  validityTo : Cadena de fecha de validez hasta en formato "YYYYMMDDhhmmss"
//  keySize : tamaño de la clave en bits: 512, 1024, 2048, 4096
//  overwrite
//    true : si los archivos existen se eliminarán y crearán de nuevo
//    false : si ambos archivos existen, no se actualizan (opción por defecto)
//Respuesta: códigos de error
//  0 : No hubo errores
//  1 : Error inicializando el sistema de archivos
//  2 : Error creando semilla de contexto drbg de clave privada
//  3 : Error configurando contecto de clave privada
//  4 : Error generando clave privada
//  5 : Error asignando subject a certificado
//  6 : Error asignando issuer a certificado
//  7 : Error asignando fechas de validez al certificado
//  8 : Error configurando el certificado como autofirmado
//  9 : Error importando número de serie
// 10 : Error asignando el número de serie al certificado
// 11 : Error creando buffer de extracción
// 12 : Error extrayendo la clave privada
// 13 : Error creando archivo de clave privada
// 14 : Error guardando clave privada en archivo
// 15 : Error extrayendo el certificado
// 16 : Error creando archivo de certificado
// 17 : Error guardando certificado en archivo
byte RoJoAutoCert(String certFileName,String pkFileName,String name,String validityFrom,String validityTo,bool overwrite,uint16_t keySize) {
  //Definición de variables globales
  int errorCode=1; //Código de error. Inicialmente 1 para el error de SPIFFS
  mbedtls_entropy_context entropy; //Contexto de entropía
  mbedtls_ctr_drbg_context ctr_drbg; //Contexto de drbg
  mbedtls_pk_context pk; //Contexto de clave privada
  mbedtls_x509write_cert crt; //Contexto de certificado
  mbedtls_mpi serial; //Número de serie del certificado
  File f; //Archivo de salida
  byte *extractBuffer=NULL; //Buffer de extracción
  int extractBufferLength; //Tamaño real del contenido del buffer de extracción
  byte *offset; //Puntero a inicio de datos del buffer de extracción

  //Si no podemos inicializar el sistema de archivos...terminamos con error 1
  if(!SPIFFS.begin()) goto freeResources;
  //Hemos podido inicializar el sistema de archivos de SPIFFS

  //Si hay que sobreescribir o falta algún archivo...
  if(overwrite || !SPIFFS.exists(pkFileName) || !SPIFFS.exists(certFileName))   {
    //...borramos los archivos sin preocuparnos si existían
    SPIFFS.remove(pkFileName);
    SPIFFS.remove(certFileName);
  }
  //Si existe un archivo...es que ambos existen. No generaremos nada. Hemos terminado Ok
  if(SPIFFS.exists(pkFileName)) return 0;
  //Tenemos que generar ambos archivos

  //Comenzaremos generando la clave privada

  //Inicializamos el contexto de entropía
	mbedtls_entropy_init(&entropy);
  //Inicializamos el contexto de drbg
	mbedtls_ctr_drbg_init(&ctr_drbg);

  //Creamos la semilla para el contexto drbg
  //Parámetros:
  //  puntero a contexto drbg inicializado
  //  puntero a función de entropía
  //  puntero a contexto de entropía inicializado
  //  puntero a datos personalizados
  //  tamaño de datos personalizados
	errorCode=mbedtls_ctr_drbg_seed(&ctr_drbg,mbedtls_entropy_func,&entropy,NULL,0);
  //Si se ha producido algún error...terminamos con error 2
  if(errorCode!=0) {
    errorCode=2;
    goto freeResources;
  }

  //Creada la semilla del contexto drbg

  //Inicializamos el contexto de la clave privada
	mbedtls_pk_init(&pk);

  //Configuramos el contexto de la clave privada
  //Parámetros:
  //  Puntero a contexto de clave privada
  //  Puntero a información a utilizar
  //    La función mbedtls_pk_info_from_type devuelve la información asociada a un 
  //    tipo de clave privada. Es este caso utilzamos una clave privada de tipo RSA
	errorCode=mbedtls_pk_setup(&pk,mbedtls_pk_info_from_type(MBEDTLS_PK_RSA));
  //Si se ha producido algún error...terminamos con error 3
	if (errorCode!=0) {
    errorCode=3;
    goto freeResources;
  }
  //Configurado el contexto de clave privada

  //Generamos la clave privada
  //Parámetros:
  //  Contexto RSA
  //  Función RNG
  //  Contexto drbg
  //  Longitud de la clave privada: 1024, 2048, 4096
  //  Exponente público (habitualmente 65537)
  errorCode=mbedtls_rsa_gen_key(mbedtls_pk_rsa(pk),mbedtls_ctr_drbg_random,&ctr_drbg,keySize,65537);
  //Si se ha producido algún error...terminamos con error 4
	if(errorCode!=0) {
    errorCode=4;
    goto freeResources;
  }
  //Clave privada generada

  //Ya no necesitamos el contexto de entropía
  //Lo liberamo para consumir menos memoria
	mbedtls_entropy_free(&entropy);

  //Ahora que tenemos generada la clave privada, podemos generar el certificado

  //Inicializamos el contexto del certificado
	mbedtls_x509write_crt_init(&crt);
  //Indicamos la versión del certificado (la última)
	mbedtls_x509write_crt_set_version(&crt,MBEDTLS_X509_CRT_VERSION_3);
  //Indicamos el algoritmo hash para la firma
	mbedtls_x509write_crt_set_md_alg(&crt,MBEDTLS_MD_SHA256);
  //Utilizaremos la clave privada para firmar los distintos campos del certificado
  //Lo hacemos así porque es un certificado autofirmado
  //Firmamos el subject del certificado con la clave privada
	mbedtls_x509write_crt_set_subject_key(&crt,&pk);
  //Firmamos el issuer del certificado con la clave privada
	mbedtls_x509write_crt_set_issuer_key(&crt,&pk);

  //Asignamos el subject al certificado
  //Parámetros
  //  Contexto del certificado
  //  Nombre
  errorCode=mbedtls_x509write_crt_set_subject_name(&crt,name.c_str());
  //Si algo ha salido mal...terminamos con error 5
	if(errorCode!=0) {
    errorCode=5;
    goto freeResources;
  }
  //Hemos podido asignar el subject al certificado

  //Asignamos el issuer al certificado
  //Parámetros
  //  Contexto del certificado
  //  Nombre
  errorCode=mbedtls_x509write_crt_set_issuer_name(&crt,name.c_str());
  //Si algo ha salido mal...terminamos con error 6
	if(errorCode!=0) {
    errorCode=6;
    goto freeResources;
  }
  //Hemos podido asignar el issuer al certificado

  //Asignamos las fechas de validez al certificado
  //Parámetros
  //  Contexto del certificado
  //  Fecha de validez desde en formato "YYYYMMDDhhmmss"
  //  Fecha de validez hasta en formato "YYYYMMDDhhmmss"
	errorCode=mbedtls_x509write_crt_set_validity(&crt,validityFrom.c_str(),validityTo.c_str());
  //Si algo ha salido mal...terminamos con error 7
	if(errorCode!=0) {
    errorCode=7;
    goto freeResources;
  }
  //Hemos podido asignar las fechas de validez

  //Asignamos las restricciones al certificado para que sea autofirmado
  //Parámetros:
  //  Contexto de certificado
  //  Es un certificado autofirmado?
  //  Número máximo de certificados encadenados por debajo?
	errorCode=mbedtls_x509write_crt_set_basic_constraints(&crt,1,0);
  //Si algo ha salido mal...terminamos con error 8
	if(errorCode!=0) {
    errorCode=8;
    goto freeResources;
  }
  //El certificado será autofirmado

	//Inicializamos el número de serie
	mbedtls_mpi_init(&serial);
  //Importamos el número de serie
  //Parámetros:
  //  Variable con el número de serie en el que se guardará
  //  Base numérica
  //  Es un buffer terminado en nulo?
	errorCode=mbedtls_mpi_read_string(&serial,10,"1");
  //Si algo ha salido mal...terminamos con error 9
	if(errorCode!=0) {
    errorCode=9;
    goto freeResources;
  }
  //Hemos podido importar el número de serie

  //Asignamos el número de serie al certificado
  //Parámetros
  //  Entorno del certificado
  //  Puntero de variable con el número de serie
  errorCode=mbedtls_x509write_crt_set_serial(&crt,&serial);
  //Si algo ha salido mal...terminamos con error 10
	if(errorCode!=0) {
    errorCode=10;
    goto freeResources;
  }
  //Hemos podido asignar el número de serie al certificado

  //Ya no necesitamos el número de serie. Lo liberamos para consumir memos memoria
  mbedtls_mpi_free(&serial);

  //Con este paso hemos completado la creación del certificado
  //Sólo queda extraer la clave privada y el certificado y
  //Guardarlos en archivos

  //Creamos el buffer de extracción
	extractBuffer=new byte[4096];
  //Si no se ha podido crear el buffer de extracción...terminamos con error 11
	if(extractBuffer==NULL) {
    errorCode=11;
    goto freeResources;
  }
  //Hemos podido crear el buffer de extracción

  //Escribimos la clave privada en el buffer de extracción y determinamos su tamaño
  //Parámetros
  //  Puntero a contexto de clave privada
  //  Puntero a buffer de memoria donde escribir
  //  Tamaño del buffer de memoria
	extractBufferLength=mbedtls_pk_write_key_der(&pk,extractBuffer,4096);
  //Si ha ocurrido algún error... terminamos con error 12
	if(extractBufferLength<0) {
    errorCode=12;
    goto freeResources;
  }
  //Hemos podido copiar la clave privada al buffer de extracción
  //y sabemos cuanto ocupa la clave privada en bytes

  //Calculamos el puntero de inicio de clave privada
  //Cuando entregamos un buffer (rango de memoria) para obtener la clave privada,
  //ésta se escribe comenzando desde el final!, no desde el principio.
  //Al principio del buffer queda basura
  //Por lo tanto el puntero de inicio del buffer de extracción no coincide con
  //el inicio de la clave privada.
	offset=extractBuffer+4096-extractBufferLength;
  //Ahora tenemos un puntero al inicio de la clave privada en offset
  //y el tamaño de la clave privada en realBufferLength
  //Intentaremos guardarlo en un archivo

  //Creamos el archivo para guardar la clave privada
  f=SPIFFS.open(pkFileName,"w");
  //Si no se ha podido crear el archivo...termnamos con error 13
  if(!f) {
    errorCode=13;
    goto freeResources;
  }
  //Se ha podido crear el archivo de clave privada

  //Guardamos la clave privada en el archivo
  errorCode=f.write(offset,extractBufferLength);
  //Cerramos el archivo
  f.close();
  //Si el número de bytes escritos no es el esperado...terminamos con error 14
  if(errorCode!=extractBufferLength) {
    errorCode=14;
    goto freeResources;
  }
  //Hemos guardado la clave privada en el archivo correctamente

  //Guardaremos el certificado en su archivo

  //Escribimos el certificado en el buffer de extracción y determinamos su tamaño
  //Parámetros
  //  Contexto del certificado
  //  Puntero a buffer
  //  Tamaño del buffer
  //  Función drbg
  //  Contexto drbg
  extractBufferLength=mbedtls_x509write_crt_der(&crt,extractBuffer,4096,mbedtls_ctr_drbg_random,&ctr_drbg);
  //Si ha ocurrido algún error... terminamos con error 15
	if(extractBufferLength<0) {
    errorCode=15;
    goto freeResources;
  }
  //Hemos podido copiar el certificado al buffer de extracción
  //y sabemos cuanto ocupa el certificado en bytes

  //Calculamos el puntero de inicio de certificado
  //Con el certificado ocurre lo mismo que con la clave privada, en la extracción a
  //un buffer, la información se guarda al final.
  //Por lo tanto el puntero de inicio del buffer de extracción no coincide con
  //el inicio del certificado.
	offset=extractBuffer+4096-extractBufferLength;
  //Ahora tenemos un puntero al inicio del certificado en offset
  //y el tamaño del certificado en realBufferLength
  //Intentaremos guardarlo en un archivo

  //Creamos el archivo para guardar el certificado
  f=SPIFFS.open(certFileName,"w");
  //Si no se ha podido crear el archivo...termnamos con error 16
  if(!f) {
    errorCode=16;
    goto freeResources;
  }
  //Se ha podido crear el archivo de certificado

  //Guardamos el certificado en el archivo
  errorCode=f.write(offset,extractBufferLength);
  //Cerramos el archivo
  f.close();
  //Si el número de bytes escritos no es el esperado...terminamos con error 17
  if(errorCode!=extractBufferLength) {
    errorCode=17;
    goto freeResources;
  }
  //Hemos guardado el certificado en el archivo correctamente

  //Todo Ok
  errorCode=0;

  //Sección de liberación de recursos
  freeResources:
  //Liberamos buffer de extracción
  if(extractBuffer!=NULL) delete[] extractBuffer;
  //Liberamos el número de serie
  mbedtls_mpi_free(&serial);
  //Liberamos el contexto del certificado
  mbedtls_x509write_crt_free(&crt);
  //Liberamos el contexto de clave privada
	mbedtls_pk_free(&pk);
  //Liberamos el contexto drbg
	mbedtls_ctr_drbg_free(&ctr_drbg);
  //Liberamos el contexto de entropía
	mbedtls_entropy_free(&entropy);
  //Devolvemos código de error
  return errorCode;
}

#endif