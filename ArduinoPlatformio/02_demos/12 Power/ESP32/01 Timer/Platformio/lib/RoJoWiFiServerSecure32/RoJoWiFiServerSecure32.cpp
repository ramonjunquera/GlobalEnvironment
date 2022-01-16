/*
  Nombre de la librería: RoJoWiFiServerSecure32.h
  Autor: Ramón Junquera
  Fecha: 20211026
  Descripción:
    Servidor seguro (SSL)
*/

//Impedimos que se defina los métodos más de una vez
#ifndef RoJoWiFiServerSecure32_CPP
#define RoJoWiFiServerSecure32_CPP

#include <RoJoWiFiServerSecure32.h>

//Constructor
//Parámetros:
//  port: puerto de escucha (por defecto 443)
//  maxClients: número máximo de clientes concurrentes (por defecto 1)
//  timeoutClient: tiempo máximo de espera en milisegundos para una conexión cliente (por defecto 1ms)
//    0 = Espera infinita hasta recibir nuevo dato o que corten la conexión.
RoJoWiFiServerSecure32::RoJoWiFiServerSecure32(uint16_t port,byte maxClients,uint16_t timeoutClient) {
  //Nota: No es conveniente que el número máximo de conexiones cliente concurrentes sea superior
  //a 4, porque podríamos quedarnos cortos de memoria para procesarlas.

  //Guardamos los parámetros en las variables privadas
  _socket=-1; //Identificador del socket. Inicialmente no tenemos el socket abierto
  _port=port; //Puerto de escucha
  _maxClients=maxClients; //Máximo número de clientes
  _listening=false; //El socket está escuchando?. Inicialmente no

  //Convertimos el timeout del cliente a formato timeval
  _timeoutClient.tv_sec=timeoutClient/1000; //Calculamos los segundos
  _timeoutClient.tv_usec=(timeoutClient % 1000)*1000; //Calculamos los microsegundos
}

//Carga un archivo de SPIFFS en memoria y devuelve el puntero (*pMem)
//También devuelve el tamaño del archivo (fileLength)
//El método gestiona la reserva de memoria dinámica necesaria
//Devuelve:
//  0 = Ok
//  1 = Archivo no encontrado
byte RoJoWiFiServerSecure32::_loadFile2Mem(const char *path,byte **pMem,uint16_t *fileLength) {
  //Abrimos el archivo como sólo lectura
  File f=SPIFFS.open(path);
  //Si no hemos podido abrir el archivo...error 1
  if(!f) return 1;

  //Obtenemos el tamaño del archivo
  *fileLength=f.size();
  //Reservamos memoria para almacenar el archivo completo 
  *pMem=new byte[*fileLength];
  //Cargamos el archivo en memoria
  f.readBytes((char *)*pMem,*fileLength);
  //Cerramos el archivo
  f.close();
  //Todo ok
	return 0;
}

//Detiene el servidor, cerrando el socket
void RoJoWiFiServerSecure32::end() {
  _listening=false; //El socket deja de escuchar
  lwip_close_r(_socket); //Cerramos el socket
  _socket=-1; //Ya no tenemos socket abierto
}

//Destructor
RoJoWiFiServerSecure32::~RoJoWiFiServerSecure32() {
  end(); //Cierra el servidor
}

//Arrancamos el servidor
//Devuelve:
//  0 = Ok
//  1 = Error. No se ha podido inicializar el sistema de archivos
//  2 = Error. No se ha podido crear el contexto del servidor seguro
//  3 = Error. Archivo de certificado no encontrado
//  4 = Error. Archivo de certificado corrupto
//  5 = Error. Archivo de clave privada no encontrado
//  6 = Error. Archivo de clave privada corrupto
//  7 = Error. No se ha podido crear el socket
//  8 = Error. No se ha podido crear el bind
//  9 = Error. No se ha podido activar la escucha del socket
byte RoJoWiFiServerSecure32::begin(const char* secureServerCertificateFile,const char* secureServerPrivateKeyFile) {
  //Si no podemos inicializar el sistema de archivos...error 1
  if(!SPIFFS.begin()) return 1;

  byte *pFileInMem=nullptr; //Puntero a memoria que contendrá el archivo cargado
  uint16_t fileLength=0;
  byte errorCode; 

  //Creamos un nuevo contexto para el servidor seguro
  _SSLcontext=SSL_CTX_new(TLS_server_method());
  //Si el contexto no se ha podido crear...error 2
  if(!_SSLcontext) return 2;

  //Cargamos el archivo de certificado en memoria
  errorCode=_loadFile2Mem(secureServerCertificateFile,&pFileInMem,&fileLength);
  //Si tenemos algún error (no se ha encontrado el archivo)...error 3
  if(errorCode) return 3;
  //Asignamos el certificado al contexto SSL
  errorCode=SSL_CTX_use_certificate_ASN1(_SSLcontext,fileLength,pFileInMem);
  //Ya no necesitaremos el certificado. Liberamos la memoria
  delete[] pFileInMem;
  //Si hemos tenido algún error asignando el certificado (0=error,1=ok)...
  if(!errorCode) {
    //...liberamos el contexto asignado
    SSL_CTX_free(_SSLcontext);
    //Informamos del error 4
    return 4;
  }
  //Hemos podido asignar el certificado al contexto SLL

  //Cargamos el archivo de clave privada en memoria
  errorCode=_loadFile2Mem(secureServerPrivateKeyFile,&pFileInMem,&fileLength);
  //Si tenemos algún error (no se ha encontrado el archivo)...error 5
  if(errorCode) return 5;
  //Asignamos el archivo de clave privada a la primera clave privada del contexto SSL
  errorCode=SSL_CTX_use_PrivateKey_ASN1(0,_SSLcontext,pFileInMem,fileLength);
  //Ya no necesitaremos la clave privada. Liberamos la memoria
  delete[] pFileInMem;
  //Si hemos tenido algún error asignando la clave privada (0=error,1=ok)...
  if(!errorCode) {
    //...liberamos el contexto asignado
    SSL_CTX_free(_SSLcontext);
    //Informamos del error 6
    return 6;
  }
  //Hemos podido asignar la clave privada al contexto SSL

  //Solicitamos la creación del socket: int socket(int domain,int type,int protocol)
  //  AF_INET = 2
  //  SOCK_STREAM = 1
  _socket=lwip_socket(AF_INET,SOCK_STREAM,0);
  //Si no se ha podido crear...
  if (_socket<0) {
    //...liberamos el contexto asignado
    SSL_CTX_free(_SSLcontext);
    //Informamos del error 7
    return 7;
  }
  //Hemos podido crear el socket
  
  //Creamos una variable con la estructura de configuración del tráfico de entrada del socket
  struct sockaddr_in client;
  //Inicializamos el contenido de la variable
  //Puesto que la estructura incluye algún array lo más fácil es llenarla de ceros
  memset(&client,0,sizeof(client));
  //Procedencia del tráfico del socket = Internet
  client.sin_family=AF_INET;
  //Rango de direcciones que atenderá el socket. Todas = 0.0.0.0
  client.sin_addr.s_addr=INADDR_ANY;
  //Definimos el puerto de escucha
  client.sin_port=htons(_port);
  //Solicitamos creación del bind
  errorCode=bind(_socket,(struct sockaddr*)&client,sizeof(client));
  if(errorCode) { //Si hemos tenido algún error creando el bind...
    end(); //...cerramos el socket
    return 8; //Informamos del error 8
  }
  //Hemos podido crear el bind

  //Activamos la escucha del socket
  errorCode=listen(_socket,_maxClients);
  if(errorCode) { //Si hubo algún problema...
    end(); //...cerramos el socket
    return 9; //Informamos del error 9
  }
  //Hemos podido activar la escucha del socket

  //Indicamos que el identificador del socket no bloquee las comunicaciones
  // F_SETFL = Set File Flags = 4
  // O_NONBLOCK = non blocking I/O = 0x4000
  //Si no lo hacemos, el sistema esperará a que el socket reciba los datos antes
  //de continuar
  fcntl(_socket,F_SETFL,O_NONBLOCK);

  //El socket comienza a escuchar
  _listening=true; 
  
  //Todo ok
  return 0;
}

//Hay nuevas conexiones cliente?
//Devuelve un puntero a un objeto de conexión cliente o NULL si no hay nuevos clientes
RoJoWiFiClientSecure32* RoJoWiFiServerSecure32::available() {
  //Si el servidor no está escuchando...devolvemos un puntero nulo
  if(!_listening) return NULL;
  //Creamos la variable que guardará el socket de la conexión cliente
  int client_sock;
  //Creamos una variable con la estructura de un cliente
  struct sockaddr_in _client;
  //Calculamos el tamaño de una estructura de cliente
  int cs = sizeof(struct sockaddr_in);
  //Obtenemos la siguiente conexión cliente aceptada
  client_sock = lwip_accept_r(_socket, (struct sockaddr *)&_client, (socklen_t*)&cs);
  //Si el socket es válido...
  if(client_sock>=0) {
    //Aplicamos una opción al socket del cliente detectado
    //Fijamos un tiempo límite de recepción (timeout)
    //Si solicitamos una lectura y se supera este tiempo de espera, no devolverá lectura
    //Si se ha podido aplicar la opción...
    if(setsockopt(client_sock,SOL_SOCKET,SO_RCVTIMEO,(char*)&_timeoutClient,sizeof(struct timeval))==ESP_OK) {
      int val = 0; //Valor de la opción. 0 = desactivar
      //Desactivamos una opción para el cliente detectado (TCP_NODELAY)
      //Los datos se acumularán en el buffer (no se enviarán al instante)
      //Si se ha podido aplicar...
      if(setsockopt(client_sock,IPPROTO_TCP,TCP_NODELAY,(char*)&val,sizeof(int))==ESP_OK) {
        //...tenemos una nueva conexión cliente y además tenemos un cliente conectado!

        //Creamos un objeto de conexión cliente con el nuevo socket
        RoJoWiFiClientSecure32 *client=new RoJoWiFiClientSecure32(client_sock,_SSLcontext);
        //Si estamos conectados...devolvemos el puntero del cliente
        if(client->connected()) return client;
        //No hemos conseguido una conexión. Borramos el objeto de conexión cliente
        delete client;
      }
    }
    //No se han podido verificar que tenemos conexión con el cliente
  }
  //No hemos detectado ninguna nueva conexión cliente
  //Devolvemos un cliente vacío
  return NULL;
}

#endif
