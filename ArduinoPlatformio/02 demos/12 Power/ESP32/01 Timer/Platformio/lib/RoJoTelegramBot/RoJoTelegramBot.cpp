/*
  Nombre de la librería: RoJoTelegramBot.h
  Versión: 20211107
  Autor: Ramón Junquera
  Descripción:
    Librería de gestión de bots de Telegram para ESP

  Notas:
    Puesto que necesitamos conectividad WiFi, estas librerías sólo funcionarán en
    dispositivos ESP8266 y ESP32.

    Cuando solicitamos la recuperación de mensajes a Telegram podemos añadir como
    parámetro optativo el número máximo de mensajes que queremos recuperar.
    Debido a las grandes limitaciones de memoria que tenemos en estos dispositivos
    tenemos un límite físico que a duras penas supera los 3 mensajes en un ESP8266.
    Por esta razón, el desarrollo siempre limita el número de mensajes obtenidos a
    uno. Es algo más lento, pero consume mucha menos memoria.

    Todo el proceso de respuestas recibidas se hace directamente en tiempo de recepción.
    No se almacena primero y se analiza después.
    Esta técnica permite un mínimo consumo de memoria, sin limitación de la longitud de
    la respuesta recibida.

    Los métodos se han simplificado y adaptado para que sean intuitivos y sencillos de
    utilizar, con los mínimos parámetros necesarios.

    No se utilizan librerías externas para el proceso de los mensajes JSON. Esto mejora
    la velocidad de proceso y consume mucha menos memoria.

    La librería implementa las funcionalidades básicas que ofrece el API de Telegram.
    Se puede consultar el API en: https://core.telegram.org/bots/api

    Los nuevos mensajes siempre se obtienen a través del método getUpdates que consiste en
    conectarse al servidor de Telegram con una conexión segura, preguntar por los mensajes
    y descargarlos. Esto obliga a contínuas llamadas.
    Existe un segundo método llamado webHook, en el que se le indica al servidor Telegram una
    URL segura (https) a la que debe enviar los nuevos mensajes.
    Este sistema evita las llamadas periódicas.
    Las librerías para la creación de un servidor seguro sólo están disponibles para ESP32.
    Por lo tanto, estas funcionalidades no estarán disponibles para ESP8266.
    
    Si estamos usando un ESP8266 no se deben copiar a la carpeta libs las librerías de
    RoJoWiFiServerSecure32.
*/

#include <RoJoTelegramBot.h> //Gestión de bot de Telegram

//Inicialización
void RoJoTelegramBot::begin(String botToken) {
  _botToken = botToken; //Guardamos el parámetro en la variable privada
  SPIFFS.begin(); //Inicializamos el sistema de archivos en ESP32
}

//Conecta y envía HTML
//Parámetros:
//  *client = Puntero a conexión cliente
//  headers = Cabeceras sin Host, ni Content-Length, ni línea final vacía
//  body = incio de cuerpo del mensaje (opcional)
//  filename = nombre del archivo a enviar (opcional)
//Devuelve el código de error:
//  0 = No hay errores. Todo OK
//  1 = Error. No se puede abrir el archivo local
//  2 = Error. No se puede conectar con Telegram
byte RoJoTelegramBot::_sendHTML(WiFiClientSecure *client,String headers,String body,String filename) {
  File f; //Archivo local
  uint32_t fileSize=0; //Tamaño del archivo
  if(filename.length()>0) { //Si tenemos nombre de archivo...
    f=SPIFFS.open(filename,"r"); //Intentamos abrir archivo local como sólo lectura
    if(!f) return 1; //No se ha podido abrir el archivo local
    //Hemos podido abrir el archivo local
    fileSize=f.size(); //Obtenemos el tamaño del archivo
  }

  client->setInsecure(); //Podemos conectar con servidores con certificado no validado
  //Si no conseguimos abrir conexión segura con Telegram...
  if(!client->connect(_telegramHost.c_str(),_telegramPort)) {
    if(f) f.close(); //Si tenemos archivo abierto...lo cerramos
    return 2; //Error. No se ha podido conectar con Telegram
  } 
  //Hemos conseguido conectar!
  headers+="Host:"+_telegramHost+"\r\n"; //Añadimos la cabecera de Host
  if(body.length()>0) { //Si tenemos cuerpo...
    //Añadiremos la cabecera de Content-Length, con y sin archivo local
    if(fileSize>0) headers+="Content-Length:"+String(body.length()+fileSize+bodyEnd.length())+"\r\n";
    else headers+="Content-Length:"+String(body.length())+"\r\n";
  }
  headers+="\r\n"; //Fin de cabeceras
  client->print(headers); client->flush(); //Enviamos cabeceras
  if(body.length()>0) {
    client->print(body); client->flush(); //Enviamos cuerpo
    if(fileSize>0) {
      //Debemos enviar el contenido completo del archivo
      //No es conveniente enviar los bytes de uno en uno porque es muy muy lento
      //Tampoco es una buena idea cargar en memoria el archivo completo, porque podría ser 
      //demasiado grande.
      //Como mejor solución crearemos un buffer de memoria dinámico de un tamaño que sea
      //fácilmente ajustable, que llenaremos y enviaremos tantas veces como sea 
      //necesario para que se envía el total del archivo

      //Creamos la variable del buffer
      byte *buffer;
      //Constante con el tamaño del buffer
      const uint32_t bufferMax=512;
      //Reservamos memoria para el buffer
      buffer = new byte[bufferMax];
      //Cuántos bytes nos faltan por leer?. Inicialmente todos
      uint32_t pending=fileSize;
      //Cuántos bytes contiene el buffer?. Inicialmente estará lleno
      uint32_t bufferSize=bufferMax;
      //Mientras queden bytes pendientes por leer...
      while(pending) {
        //Si podemos llenar el buffer...
        if(pending>=bufferMax) {
          //Lo leemos entero desde el archivo
          f.readBytes((char *)buffer,bufferMax);
          //Reducimos el número de bytes pendientes por leer en el tamaño del buffer
          pending-=bufferMax;
        } else { //No tenemos suficiente como para llenar el buffer...
          //Leemos lo que quede
          f.readBytes((char *)buffer,pending);
          //El buffer no está lleno
          bufferSize=pending;
          //Ya no queda nada pendiente
          pending=0;
        }
        //Enviamos el buffer
        client->write(buffer,bufferSize);
        client->flush();
      }
      //liberamos la memoria del buffer
      delete[] buffer;
      //Hemos terminado de enviar el contenido del archivo
      
      client->print(bodyEnd); client->flush(); //Enviamos final de cuerpo
    }
  }

  if(f) f.close(); //Si tenemos archivo abierto...lo cerramos

  return 0; //Todo Ok
}

//Comprueba si existe un nuevo mensaje en el servidor de Telegram.
//Devuelve la estructura del mensaje en el parámetro msg que
//ya debería estar creado.
//Si el mensaje tiene texto vacío, es que no hay nuevo mensaje.
void RoJoTelegramBot::getNextMessage(TelegramMessage* msg) {
  //Ejemplo de respuesta sin mensaje:
  //{"ok":true,"result":[]}
  //Ejemplo de respuesta con mensaje:
  //{"ok":true,"result":[{"update_id":313033373,"message":{"message_id":578,"from":{"id":3396975,"is_bot":false,
  //"first_name":"Ramon","last_name":"Junquera","username":"ramonjunquera","language_code":"es"},
  //"chat":{"id":3396975,"first_name":"Ramon","last_name":"Junquera","username":"ramonjunquera","type":"private"},
  //"date":1511178795,"text":"Hola","entities":[{"offset":0,"length":3,"type":"bot_command"}]}}]}

  msg->text=""; //Inicializamos el mensaje asignando un nombre vacío

  #ifdef ESP32
    //Si tenemos activado webhook...hemos terminado
    if(_webhookServer) return;
  #endif

  //Si no hemos inicializado el bot...hemos terminado
  if(!_botToken.length()) return;
  //El bot ha sido inicializado

  RoJoWiFiClientSecure client; //Conexión de cliente seguro (https)
  String headers="GET /bot"+_botToken+"/getUpdates?offset="+String(_lastUpdateId.toInt()+1)+"&limit=1 HTTP/1.1\r\n";
  if(_sendHTML(&client,headers)==0) { //Si no hay errores...
    if(client.readFindString("\"ok\":true")) { //Si se ha enviado correctamente...
      msg->update_id=client.readFindFieldInt("update_id"); //Leemos el campo update_id
      if(msg->update_id.length()) { //Si tiene algún valor...
        //...leemos el resto de campos
        msg->message_id=client.readFindFieldInt("message_id");
        msg->from_id=client.readFindFieldInt("id");
        msg->from_name=client.readFindFieldString("first_name");
        msg->chat_id=client.readFindFieldInt("id");
        msg->date=client.readFindFieldInt("date");
        msg->text=client.readFindFieldString("text");
        //El identificador del último mensaje recibido será el actual
        _lastUpdateId = msg->update_id;
      }
    }
    client.stop(); //Cerramos la conexión
  }
}

//Envía un archivo/foto local a un chat.
//Devuelve el file_id del archivo si se ha enviado correctamente. Si no, devuelve una cadena vacía
//Si la extensión (ext) contiene algo, supondremos que es una foto
String RoJoTelegramBot::_sendFileLocal(String chat_id,String fileName,String ext) {
  //Ejemplo de respuesta (sin cabeceras):
  //{"ok":true,"result":{"message_id":1567,"from":{"id":464959828,"is_bot":true,"first_name":"RoJoTest","username":"RoJoTestBot"},
  //"chat":{"id":3396975,"first_name":"Ramon","last_name":"Junquera","username":"ramonjunquera","type":"private"},"date":1512023259,
  //"photo":[{"file_id":"AgADBAADKKsxG9aI-VBHE3dFfFVGyYjn-RkABBAUyNyncheJmuwDAAEC","file_size":1549,"width":90,"height":60},
  //{"file_id":"AgADBAADKKsxG9aI-VBHE3dFfFVGyYjn-RkABJJ41C_H2cJJmewDAAEC","file_size":2819,"width":96,"height":64}]}}

  //Respuesta. Inicialmente vacía.
  String answer="";
  //Si no hemos inicializado el bot...hemos terminado
  if(!_botToken.length()) return answer;

  RoJoWiFiClientSecure client; //Conexión de cliente seguro (https)

  String headers="POST /bot"+_botToken+"/send"+(ext.length()>0?"Photo":"Document")+" HTTP/1.1\r\n";
  headers+="Content-Type: multipart/form-data;boundary="+boundary+"\r\n";

  String body="--"+boundary+"\r\n";
  body+="content-disposition: form-data; name=\"chat_id\"\r\n\r\n";
  body+=chat_id+"\r\n";
  body+="--"+boundary+"\r\n";
  //Indicamos el nombre dependiendo de su es foto o documento
  body+="content-disposition: form-data; name=\"";
  body+=(ext.length()>0?"photo":"document");
  body+="\"; filename=\""+fileName+"\"\r\n";
  //Si es una foto, debemos indicar su formato
  if(ext.length()>0) body+="Content-Type: "+ext+"\r\n";
  body+="\r\n"; //Fin del inicio del cuerpo

  //Si hubo algún error enviando...terminamos con error
  if(_sendHTML(&client,headers,body,fileName)>0) return "";
  //Se ha enviado correctamente

  String file_id; //Identificador de imagen
  //Esperamos confirmación de recepción
  //Si encontramos el ok en la respuesta...
  if(client.readFindString("\"ok\":true","",8000)) {
    do {
      //Tomamos nota del campo file_id
      //Incluimos }} como cadena de fin para no tener que esperar al TimeOut
      file_id=client.readFindFieldString("file_id","}}");
      //Si contiene algo, lo anotamos como respuesta válida
      if(file_id.length()) answer=file_id;
    } while(file_id.length()); //Repetiremos mientras encontremos un file_id. Así localizaremos siempre el último
  }
  return answer; //Devolvemos respuesta
}

//Envía un archivo local a un chat.
//Devuelve el file_id del archivo si se ha enviado correctamente. Si no, devuelve una cadena vacía
String RoJoTelegramBot::sendFileLocal(String chat_id,String fileName) {
  //Llamamos al método privado. La extensión está vacía para indicar que es un archivo
  return _sendFileLocal(chat_id,fileName,"");
}

//Envía una foto local a un chat.
//Devuelve el file_id del archivo si se ha enviado correctamente. Si no, devuelve una cadena vacía
//Detectará como archivos de foto aquellos que tengan extensión: jpg o bmp
String RoJoTelegramBot::sendPhotoLocal(String chat_id,String fileName) {
  //Debemos identificar si es un archivo de foto y de qué tipo
  //Obtenemos como extensión los 3 últimos caracteres del nombre de archivo
  String ext=fileName.substring(fileName.length()-3);
  //Tipo de archivo
  if(ext=="jpg") ext="jpeg";
  else if(ext!="bmp") return "";
  //Si es una foto llamamos al método privado. Enviamos la extensión para indicar que es una foto
  return _sendFileLocal(chat_id,fileName,ext);
}

//Envía un archivo/foto a un chat. Ya sea por su URL o por su file_id
//Si se ha enviado correctamente devuelve el file_id. Si no, devuelve una cadena vacía.
//Si se trata de una foto, el file_id devuelto siempre es el de mayor resolución.
String RoJoTelegramBot::_sendFileRemote(bool isPhoto, String chat_id,String file_id,String caption,bool disable_notification,String reply_to_message_id) {
  //Ejemplo de respuesta (sin cabeceras):
  //{"ok":true,"result":{"message_id":1564,"from":{"id":464959828,"is_bot":true,"first_name":"RoJoTest","username":"RoJoTestBot"},
  //"chat":{"id":3396975,"first_name":"Ramon","last_name":"Junquera","username":"ramonjunquera","type":"private"},"date":1512023047,
  //"photo":[{"file_id":"AgADBAADTPY4G3EeZAdRfHiiD7n7KeJv4BkABN8qbr6qcFPthowBAAEC","file_size":2091,"width":90,"height":90},
  //{"file_id":"AgADBAADTPY4G3EeZAdRfHiiD7n7KeJv4BkABCVIEEM8hSmdh4wBAAEC","file_size":15169,"width":250,"height":250}],
  //"caption":"imagen original"}}

  String answer=""; //Respuesta. Inicialmente vacía.
  
  //Si no hemos inicializado el bot...hemos terminado
  if(!_botToken.length()) return answer;
  //El bot está inicializado

  String headers="POST /bot"+_botToken+"/send"+(isPhoto?"Photo":"Document")+" HTTP/1.1\r\n";
  headers+="Content-Type: application/json\r\n";
  
  String body="{\"chat_id\":\""+chat_id+"\",\"";
  body+=(isPhoto?"photo":"document");
  body+="\":\""+file_id+"\"";
  //Si tiene el parámetro caption...lo añadimos
  if(caption.length()) body+=",\"caption\":\""+caption+"\"";
  //Si tiene el parámetro disable_notification=true...lo añadimos
  if(disable_notification) body+=",\"disable_notification\":\"true\"";
  //Si tiene el parámetro reply_to_message_id...lo añadimos
  if(reply_to_message_id!="0") body+=",\"reply_to_message_id\":\""+reply_to_message_id+"\"";
  body+="}\r\n"; //Cerramos la cadena

  RoJoWiFiClientSecure client; //Conexión de cliente seguro (https)
  if(_sendHTML(&client,headers,body)==0) { //Se se ha podido conectar...
    //Dependiendo del tamaño de la imagen se pueden devolver varios id distintos
    //Cada uno con su tamaño
    //Están ordenados por tamaño de menor a mayor
    //Nosotros siempre devolveremos el mayor (último)
    //El campo a localizar es file_id. Es de texto.

    String file_id; //Identificador de imagen
    if(client.readFindString("\"ok\":true","",8000)) { //Si se ha enviado correctamente...
      do {
        //Tomamos nota del campo file_id
        //Enviamos como cadena de fin }} para que no tengamos que esperar al TimeOut
        file_id=client.readFindFieldString("file_id","}}");
        //Si contiene algo, lo anotamos como respuesta válida
        if(file_id.length()) answer=file_id;
      }
      //Repetiremos mientras encontremos un file_id. Así localizaremos siempre el último
      while(file_id.length());
    }
  }
  return answer; //Devolvemos respuesta
}

//Envía una foto a un chat. Ya sea por su URL o por su file_id
//Devuelve el file_id de la foto si se ha enviado correctamente. Si no, devuelve una cadena vacía
//El file_id devuelto siempre es el de mayor resolución
String RoJoTelegramBot::sendPhotoRemote(String chat_id,String file_id,String caption,bool disable_notification,String reply_to_message_id) {
  //Llamamos al método privado indicando que es una foto
  return _sendFileRemote(true,chat_id,file_id,caption,disable_notification,reply_to_message_id);
}

//Envía un archivo a un chat. Ya sea por su URL o por su file_id
//Devuelve el file_id del archivo si se ha enviado correctamente. Si no, devuelve una cadena vacía
//Si se referencia al archivo con una URL sólo puede ser del tipo gif, pdf o zip.
String RoJoTelegramBot::sendFileRemote(String chat_id,String file_id,String caption,bool disable_notification,String reply_to_message_id) {
  //Llamamos al método privado indicando que es una foto
  return _sendFileRemote(false,chat_id,file_id,caption,disable_notification,reply_to_message_id);
}

//Envía un mensaje de acción
//Devuelve true si se ha podido enviar
bool RoJoTelegramBot::sendChatAction(String chat_id,byte actionCode) {
  //Códigos de los mensajes de acción
  //  0 - escribiendo / typing
  //  1 = enviando foto / upload_photo
  //  2 = enviando vídeo / record_video
  //  3 = grabando un mensaje de voz / record_audio
  //  4 = enviando audio / upload_audio
  //  5 = enviando archivo / upload_document
  //  6 = escribiendo / find_location
  //  7 = grabando un videomensaje / record_video_note
  //  8 = grabando un videomensaje / upload_video_note

  //Ejemplo de respuesta:
  //{"ok":true,"result":true}

  //Respuesta. Inicialmente no se ha enviado correctamente
  bool answer=false;
  //Si no hemos inicializado el bot...hemos terminado
  if(!_botToken.length()) return answer;

  if(actionCode<9) { //Si el código de acción está permitido...
    RoJoWiFiClientSecure client; //Conexión de cliente seguro (https)
    String headers="GET /bot"+_botToken+"/sendChatAction?chat_id="+chat_id+"&action="+_actionCodes[actionCode]+" HTTP/1.1\r\n";
    if(!_sendHTML(&client,headers)) return false; //Si no hemos podido conectar...error
    //Hemos podido conectar!
    answer=client.readFindString("\"ok\":true","}",8000); //Anotamos la respuesta de si se ha enviado correctamente
    client.stop(); //Cerramos la conexión
  }
  return answer; //Respondemos si se ha enviado correctamente
}

//Envía un mensaje de texto a un chat
//Devuelve true si se ha enviado correctamente
bool RoJoTelegramBot::sendMessage(String chat_id,String text,String keyboard,bool resize,bool oneTime) {
  //Sólo son obligatorios los dos primeros parámetros
  // chat_id : identificador del chat al que se envía el mensaje
  // text : texto del mensaje
  // keyboard : array de arrays en formato JSON
  //   Cada elemento primario es una fila, y dentro de él, cada elemento es una columna
  // resize : intenta reducir la altura de los botones ajustándola a su contenido
  // oneTime : sólo muestra el teclado hasta que se selecciona una opción, después desaparece

  //Ejemplo de parámetros de envío:
  //{"chat_id":"3396975","text":"Choose one option","reply_markup":{"keyboard":[["/on","/off"],["/status"]],"resize_keyboard":true}}
  //En este ejemplo se muestran dos filas de botones. La primera tiene 2 y la segunda sólo uno.

  //Ejemplo de respuesta (sin cabeceras):
  //{"ok":true,"result":{"message_id":1845,"from":{"id":464959828,"is_bot":true,"first_name":"RoJoTest","username":"RoJoTestBot"},
  //"chat":{"id":3396975,"first_name":"Ramon","last_name":"Junquera","username":"ramonjunquera","type":"private"},
  //"date":1512151697,"text":"Choose from one of the following options"}}

  //Si no hemos inicializado el bot...hemos terminado
  if(!_botToken.length()) return false;
  //Se bot ya estaba inicializado

  //Componemos cabeceras
  String headers="POST /bot"+_botToken+"/sendMessage HTTP/1.1\r\n";
  headers+="Content-Type: application/json\r\n";

  //Componemos la cadena básica de parámetros
  String body="{\"chat_id\":\""+chat_id+"\",\"text\":\""+text+"\"";
  //Si tenemos el parámetro de teclado...
  if(keyboard.length()) {
    //Añadimos los parámetros del teclado
    body+=",\"reply_markup\":{\"keyboard\":"+keyboard;
    //Si tiene el parámetro resize...lo añadimos
    if(resize) body+=",\"resize_keyboard\":true";
    //Si tiene el parámetro oneTime...lo añadimos
    if(oneTime) body+=",\"one_time_keyboard\":true";
    //Cerramos la sección de teclado
    body+="}";
  }
  //Cerramos la cadena de parámetros
  body+="}";
  
  bool answer=false; //Inicialmente no lo hemos conseguido
  RoJoWiFiClientSecure client; //Conexión de cliente seguro (https)
  if(_sendHTML(&client,headers,body)==0) { //Si hemos podido conectar...
    //Comprobamos si se ha encontrado la marca de envío correcto
    answer=client.readFindString("\"ok\":true","{",8000);
    client.stop(); //Cerramos la conexión
  }
  return answer;
}

//Activa webhook en Telegram
//Respuestas:
//  0 = Todo Ok
//  1 = Error al abrir archivo de certificado en SPIFFS
//  2 = Error al conectar con Telegram
//  3 = Error al activar el webhook en Telegram
byte RoJoTelegramBot::webhookEnableTelegram(const char *secureServerCertificateFile,String localHostname,uint16_t port,byte maxClients) {
  String headers="POST /bot"+_botToken+"/setWebhook HTTP/1.1\r\n";
  headers+="Content-Type: multipart/form-data; boundary="+boundary+"\r\n";

  String body="--"+boundary+"\r\n";
  body+="content-disposition: form-data; name=\"url\"\r\n\r\n";
  body+="https://"+localHostname+":"+String(port)+"/"+"\r\n--"+boundary+"\r\n";
  //Indicamos el número máximo de conexiones
  body+="content-disposition: form-data; name=\"max_connections\"\r\n\r\n";
  body+=String((int)maxClients)+"\r\n";
  body+="--"+boundary+"\r\n";
  //Indicamos el archivo de certificado que enviamos
  body+="content-disposition: form-data; name=\"certificate\"; filename=\"";
  body+=secureServerCertificateFile;
  body+="\"\r\n";
  body+="Content-Type: application/octect-stream\r\n\r\n";

  RoJoWiFiClientSecure client; //Conexión de cliente seguro (https)
  byte errorCode=_sendHTML(&client,headers,body,secureServerCertificateFile);
  if(errorCode) return errorCode;
  //Se ha podido conectar

  //Esperamos confirmación de recepción
  //La respuesta correcta debería tener el siguiente formato:
  //{"ok":true,"result":true,"description":"Webhook was set"}
  bool found=client.readFindString("\"result\":true","}",8000);
  client.stop();
  return (found==true)?0:3;
}

//Consulta en Telegram si el bot tiene activado el webhook
//Respuesta:
//  0 = Webhook desactivado
//  1 = Webhook activado
//  2 = Error. No puedo conectar con Telegram
byte RoJoTelegramBot::webhookCheckTelegram() {
  RoJoWiFiClientSecure client; //Conexión de cliente seguro (https)

  String headers="GET /bot"+_botToken+"/getWebhookInfo HTTP/1.1\nHost:"+_telegramHost+"\r\n";
  if(_sendHTML(&client,headers)>0) return 2; //Si no se puede coenctar...error
  //Hemos podido conectar

  //Ejemplo de mensajes de respuesta
  //Webhook desactivado:
  //{"ok":true,"result":{"url":"","has_custom_certificate":false,"pending_update_count":2}}
  //Webhook activado:
  //{"ok":true,"result":{"url":"https://myhostname.com:8443/","has_custom_certificate":true,"pending_update_count":2,"last_error_date":1636098955,"last_error_message":"Connection timed out","max_connections":1,"ip_address":"79.13.17.25"}}
  String url=client.readFindFieldString("url","}}",8000); //Anotamos el valor del campo url
  client.stop();
  return url.length()>0;
}

//Desactiva webhook en Telegram
//Devuelve true si lo consigue
bool RoJoTelegramBot::webhookDisableTelegram() {
  RoJoWiFiClientSecure client; //Conexión de cliente seguro (https)
  String headers="GET /bot"+_botToken+"/setWebhook HTTP/1.1\nHost:"+_telegramHost+"\r\n";
  if(_sendHTML(&client,headers)>0) return false; //Si no podemos conectar...error
  //Hemos podido conectar
  
  //Ejemplo de mensajes de respuesta
  //Desactivación correcta:
  //{"ok":true,"result":true,"description":"Webhook was deleted"}
  //Ya estaba desactivado de antes:
  //{"ok":true,"result":true,"description":"Webhook is already deleted"}
  //En cualquier caso, nos fijaremos sólo en "result":true
  bool found=client.readFindString("\"result\":true","}",8000);
  client.stop();
  return found;
}

#ifdef ESP32 //Si es un ESP32 tiene disponibles las funciones de webhook

//Activa webhook, tanto en local como en Telegram
//Parámetros:
//  secureServerCertificateFile : archivo de certificado en SPIFFS
//  secureServerPrivateKeyFile : archiivo de clave privada en SPIFFS
//  customFuncion : función de usuario a la que se llama para entregar los mensajes recibidos por webhook
//  port : puerto de escucha del servidor web seguro
//  maxClients : número máximo de conexiones cliente concurrentes
//  timeoutClient : timeout de recepción de datos de cliente en ms
byte RoJoTelegramBot::webhookEnable(const char *secureServerCertificateFile,const char *secureServerPrivateKeyFile,void (*customFunction)(TelegramMessage *msg),uint16_t port,byte maxClients,uint16_t timeoutClient) {
  //Devuelve el codigo de error
  //Los primeros son los mismos que devuelve RoJoWiFiServerSecure32.begin
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
  // 10 = Error. No se ha podido activar webhook en Telegram

  //Si ya estaba activo...hemos terminado sin errores
  if(_webhookServer) return 0;
  //El servidor web seguro no está activo

  //Creamos un objeto de servidor seguro
  _webhookServer=new RoJoWiFiServerSecure32(port,maxClients,timeoutClient);
  //Lo arrancamos
  byte errorCode=_webhookServer->begin(secureServerCertificateFile,secureServerPrivateKeyFile);
  //Si se ha producido algún error...desactivamos webhook
  if(errorCode>0) webhookDisable(); 
  //Si se ha creado el servidor...anotamos la función notificación de mensajes recibidos por webhook
  else _customFunction=customFunction;
  return errorCode; //Devolvemos el código de error
}

//Desactiva webhook, tanto en local como en Telegram
bool RoJoTelegramBot::webhookDisable() {
  bool isOk=false; //Hemos podido deshabilitar webhooks?. Inicialemente no
  if(_webhookServer) { //Si está activo...
    RoJoWiFiClientSecure client; //Conexión de cliente seguro (https)

    String headers="GET /bot"+_botToken+"/setWebhook HTTP/1.1\nHost:"+_telegramHost+"\r\n";
    if(_sendHTML(&client,headers)==0) { //Si hemos podido conectar...
      //La respuesta correcta debería tener el siguiente formato:
      //{"ok":true,"result":true,"description":"Webhook was deleted"}
      //Si encontramos la marca de envío correcto...
      if (client.readFindString("\"ok\":true","}",8000)) {
        client.stop();
        _webhookServer->end(); //Detenemos el servidor
        delete _webhookServer; //Liberamos el objeto del servidor seguro
        _webhookServer=NULL; //Limpiamos su puntero
        isOk=true; //Todo Ok
      }
    }
  }
  return isOk;
}

//Detecta nuevos mensajes recibidos por webhook
void RoJoTelegramBot::webhookRefresh() {
  //Si webhook no está activado...hemos terminado
  if(!_webhookServer) return;
  //webhook está activo

  //Anotamos si hay algún nuevo cliente
  RoJoWiFiClientSecure32 *client=_webhookServer->available();
  if(client) { //Si tenemos alguno...
    /*
    Ejemplo de formato de mensaje recibido:

    POST / HTTP/1.1
    Host: myserver.mydomain
    Content-Type: application/json
    Content-Length: 326
    Connection: keep-alive
    Accept-Encoding: gzip, deflate

    {"update_id":500150329,
    "message":{"message_id":10606,"from":{"id":3396975,"is_bot":false,"first_name":"Ramon","last_name":"Junquera","username":"ramonjunquera","language_code":"es"},"chat":{"id":3396975,"first_name":"Ramon","last_name":"Junquera","username":"ramonjunquera","type":"private"},"date":1557012314,"text":"Hola"}}

    La estructura de un mensaje de Telegram es:
      String update_id;
      String message_id;
      String from_id;
      String from_name;
      String chat_id;
      String date;
      String text;
    */

    TelegramMessage *msg=new TelegramMessage; //Creamos una estructura de mensaje
    msg->update_id=client->readFindFieldInt("update_id"); //Leemos el campo update_id
    if(msg->update_id.length()) { //Si tiene algún valor...
      //...leemos el resto de campos
      msg->message_id=client->readFindFieldInt("message_id");
      msg->from_id=client->readFindFieldInt("id");
      msg->from_name=client->readFindFieldString("first_name");
      msg->chat_id=client->readFindFieldInt("id");
      msg->date=client->readFindFieldInt("date");
      msg->text=client->readFindFieldString("text");
      //Hemos terminado de recuperar los datos del mensaje
      //Devolvemos confirmación de recepción correcta
      client->write("HTTP/1.1 200 OK\r\n\r\n");
    }
    client->stop(); //Cortamos la conexión
    delete client; //Liberamos el objeto de conexión cliente
    //Si tenemos un mensaje válido...se lo pasamos a la función de usuario
    if(msg->update_id.length()) _customFunction(msg);
    delete msg; //Ya lo han procesado. Liberamos memoria del mensaje
  }
}

#endif //ESP32. Funciones de webhook
