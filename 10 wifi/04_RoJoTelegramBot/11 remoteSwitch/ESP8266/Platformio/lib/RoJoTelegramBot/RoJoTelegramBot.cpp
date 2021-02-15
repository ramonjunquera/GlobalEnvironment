/*
  Nombre de la librería: RoJoTelegramBot.h
  Versión: 20190910
  Autor: Ramón Junquera
  Descripción:
    Librería de gestión de bots de Telegram para ESP

  Notas:
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

    Los nuevos mensajes siempre se obtiene a través del método getUpdates que consiste en
    conectarse al servidor de Telegram con una conexión segura, preguntar por los mensajes
    y descargarlos. Esto obliga a contínuas llamadas.
    Existe un segundo método llamado webHook, en el que se le indica al servidor Telegram una
    URL segura (https) a la que debe enviar los nuevos mensajes.
    Este sistema evita las llamadas periódicas, pero no está contemplado en este desarrollo.
*/

#include <RoJoTelegramBot.h>

//Inicialización
void RoJoTelegramBot::begin(String botToken) {
  //Guardamos el parámetro en la variable privada
  _botToken = botToken;
  //Inicializamos el sistema de archivos
  SPIFFS.begin();
}

//Lee caracteres del cliente hasta encontrar una cadena, detectar la cadena endText o alcanzar el tiempo de time out
//El parámetro endText es optativo
//La longitud de los parámetros findText y endText puede ser distinta
//Devuelve true si ha encontrado la cadena
bool RoJoTelegramBot::_clientFindString(WiFiClientSecure *client,uint16_t timeOut,String findText,String endText) {
  //Calculamos el máximo tiempo de espera
  uint32_t waitTime=millis()+timeOut;  
  //Buffer de texto recibido
  String buffer="";
  //Calculamos el tamaño del buffer
  byte bufferLen=findText.length();
  if(endText.length()>bufferLen) bufferLen=endText.length();

  //Inicializamos el buffer a su tamaño correcto
  for(byte c=0;c<bufferLen;c++) buffer+="@";
  //Mientras no hayamos consumido todo el tiempo permitido...
  while(millis()<waitTime) {
    //Mientras haya caracteres pendientes por recibir...
    while(client->available()>0) {
      //Leemos el siguiente carácter y lo añadimos al buffer
      buffer=buffer.substring(1)+(char)(*client).read();
      //Si el final del buffer coincide con la cadena que estamos buscando...lo hemos encontrado!
      if(buffer.substring(bufferLen-findText.length())==findText) return true;
      //Si tenemos cadena de fin...
      if(endText.length()) {
        //Si el final del buffer coincide con la cadena de fin...no lo hemos encontrado
        if(buffer.substring(bufferLen-endText.length())==endText) return false;
      }
    }
  }
  //TimeOut. No lo hemos encontrado
  return false;
}

//Parsea caracteres numéricos
//Se detiene cuando encuentra un carácter no numérico o cuando se supera el tiempo de timeOut
String RoJoTelegramBot::_clienParseInt(WiFiClientSecure *client,uint16_t timeOut) {
  //Calculamos el máximo tiempo de espera
  uint32_t waitTime=millis()+timeOut;
  //Variable en la que guardaremos la respuesta
  String answer="";
  //Carácter leido
  char c;

  //Mientras no hayamos consumido todo el tiempo permitido...
  while(millis()<waitTime) {
    //Mientras haya caracteres pendientes por recibir...
    while(client->available()>0) {
      //Leemos el siguiente carácter
      c=(*client).read();
      //Si es un carácter numérico...
      if(c>='0' && c<='9') {
        //...lo tomamos como correcto lo añadimos a la respuesta
        //El código ASCII del '0' es el 48
        answer+=c;
      }
      else { //No es un carácter numérico
        //Devolvemos el resultado
        return answer;
      }
    }
  }
  //TimeOut. Devolvemos lo que hayamos podido encontrar
  return answer;
}

//Localiza un campo numérico
//En parámetro endText es optativo
String RoJoTelegramBot::_clientFieldInt(WiFiClientSecure *client,uint16_t timeOut,String fieldName,String endText) {
  String answer="";
  //Si hemos encontrado el campo...
  if(_clientFindString(client,timeOut,"\""+fieldName+"\":",endText)) {
    //...leemos el valor entero
    answer=_clienParseInt(client,timeOut);
  }
  return answer;
}

//Devuelve el texto recibido hasta encontrar la cadena de marca de fin
//Si no encuentra la marca de fin o se alcanza el tiempo límite devuelve una cadena vacía
String RoJoTelegramBot::_clientReadString(WiFiClientSecure *client,uint16_t timeOut,String border) {
  //Calculamos el máximo tiempo de espera
  uint32_t waitTime=millis()+timeOut;
  //Variable en la que guardaremos la respuesta
  String answer="";

  //Mientras no hayamos consumido todo el tiempo permitido...
  while(millis()<waitTime) {
    //Mientras haya caracteres pendientes por recibir...
    while(client->available()>0) {
      //Leemos el siguiente carácter y lo añadimos a la respuesta
      answer+=(char)(*client).read();
      //Si el final de la cadena coincide con la marca de fin...
      if(answer.substring(answer.length()-border.length())==border) {
         //...devolvemos la respuesta sin incluir la marca de fin
        return answer.substring(0,answer.length()-border.length());
      }
    }
  }
  //No hemos encontrado la marca de fin
  return "";
}

//Localiza un campo y devuelve su texto
//En parámetro endText es optativo
String RoJoTelegramBot::_clientFieldString(WiFiClientSecure *client,uint16_t timeOut,String fieldName,String endText) {
  String answer="";

  //Si hemos encontrado el campo...
  if(_clientFindString(client,timeOut,"\""+fieldName+"\":\"",endText)) {
    //...leemos el texto hasta las siguientes comillas
    answer=_clientReadString(client,timeOut,"\"");
  }
  return answer;
}

void RoJoTelegramBot::getNextMessage(TelegramMessage* msg) {
  //Obtiene el siguiente mensaje
  //Devuelve la estructura del mensaje en el parámetro msg que ya debería estar creado
  //Si el mensage tiene texto vacío, es que no hay nuevo mensaje
  //Ejemplo de respuesta sin mensaje:
  //{"ok":true,"result":[]}
  //Ejemplo de respuesta con mensaje:
  //{"ok":true,"result":[{"update_id":313033373,"message":{"message_id":578,"from":{"id":3396975,"is_bot":false,
  //"first_name":"Ramon","last_name":"Junquera","username":"ramonjunquera","language_code":"es"},
  //"chat":{"id":3396975,"first_name":"Ramon","last_name":"Junquera","username":"ramonjunquera","type":"private"},
  //"date":1511178795,"text":"Hola","entities":[{"offset":0,"length":3,"type":"bot_command"}]}}]}

  Serial.println("getNextMessage start"); //DEBUG
  //Conexión de cliente seguro (https)
  WiFiClientSecure client;
  //Con la nueva versión de BearSSL en vez de axSSL, es necesario añadir la siguiente
  //línea para que no se comprueben credenciales ni fingerprints
  client.setInsecure();
  //inicializamos el mensaje asignando un nombre vacío
  msg->text="";
  //Si no hemos inicializado el bot...hemos terminado
  if(!_botToken.length()) return;
  Serial.println("getNextMessage bot started"); //DEBUG
  //Si conseguimos abrir conexión segura con Telegram...
  if(client.connect(_telegramHost,_telegramPort)) {
    Serial.println("getNextMessage https opened"); //DEBUG
    //Enviamos el comando
    client.println("GET /bot"+_botToken+"/getUpdates?offset="+String(_lastUpdateId.toInt()+1)+"&limit=1");
    //Si se ha enviado correctamente...
    if(_clientFindString(&client,1500,"\"ok\":true")) {
      Serial.println("getNextMessage answer ok"); //DEBUG
      //Leemos el campo update_id
      msg->update_id=_clientFieldInt(&client,1500,"update_id");
      //Si tiene algún valor...
      if(msg->update_id.length()) {
        //...leemos el resto de campos
        msg->message_id=_clientFieldInt(&client,1500,"message_id");
        msg->from_id=_clientFieldInt(&client,1500,"id");
        msg->from_name=_clientFieldString(&client,1500,"first_name");
        msg->chat_id=_clientFieldInt(&client,1500,"id");
        msg->date=_clientFieldInt(&client,1500,"date");
        msg->text=_clientFieldString(&client,1500,"text");
        //El identificador del último mensaje recibido será el actual
        _lastUpdateId = msg->update_id;
        Serial.println("getNextMessage message detected: "+msg->text); //DEBUG
      }
    }
  }
  //Cerramos la conexión
  client.stop();
  //Hemos terminado
  return;
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

  //Dependiendo de la placa definidos el tiempo de espera
  //Las placas ESP32 son bastante más rápidas que las ESP8266
  //Cuando se envía información a través de una conexión cliente, en ESP32 debemos
  //esperar un poco para que no se sature el buffer de salida.
  //En las ESP8266 no es necesario
  #if defined(ESP32)
    const uint32_t clientWait=6; //6ms en ESP32
  #else
    const uint32_t clientWait=0; //0ms en ESP8266
  #endif 

  //Respuesta. Inicialmente vacía.
  String answer="";
  //Si no hemos inicializado el bot...hemos terminado
  if(!_botToken.length()) return answer;

  const String boundary = "-RoJo2017-Smile!";
  //Tamaño del archivo
  uint32_t fileSize;

  //Conexión de cliente seguro (https)
  WiFiClientSecure client;
  //Con la nueva versión de BearSSL en vez de axSSL, es necesario añadir la siguiente
  //línea para que no se comprueben credenciales ni fingerprints
  client.setInsecure();
  //Si conseguimos abrir conexión segura con Telegram...
  if(client.connect(_telegramHost.c_str(),_telegramPort)) {
    //Abrimos el archivo indicado del SPIFFS como sólo lectura
    File f=SPIFFS.open(fileName,"r");
    //Si se pudo abrir...
    if(f) {
      //Componemos el inicio de la solicitud
      String start_request = "--" + boundary + "\r\n";
      start_request+= "content-disposition: form-data; name=\"chat_id\"\r\n";
      start_request+= "\r\n";
      start_request+= chat_id + "\r\n";
      start_request+= "--" + boundary + "\r\n";
      //Indicamos el nombre dependiendo de su es foto o documento
      start_request+= "content-disposition: form-data; name=\"";
      start_request+=(ext.length()>0?"photo":"document");
      start_request+="\"; filename=\""+fileName+"\"\r\n";
      //Si es una foto, debemos indicar su formato
      if(ext.length()>0) start_request+= "Content-Type: " + ext + "\r\n";
      start_request+= "\r\n"; //Terminada la cadena de inicio
      
      //Componemos el final de la solicitud
      String end_request = "\r\n";
      end_request+= "--" + boundary + "--\r\n";

      //Obtenemos el tamaño del archivo
      fileSize=f.size();
      //Calculamos el tamaño total a enviar
      uint32_t contentLength = fileSize + start_request.length() + end_request.length();

      //Enviamos el texto HTML con las cabeceras al servidor Telegram
      //Enviamos URI
      //Distinguimos entre foto y documento. Los comandos son distintos: sendPhoto o sendDocument
      client.println("POST /bot"+_botToken+"/send"+(ext.length()>0?"Photo":"Document")+" HTTP/1.1");
      //Enviamos el Host
      delay(clientWait);
      client.println("Host:" + _telegramHost);
      //User-Agent
      delay(clientWait);
      client.println("User-Agent: arduino/1.0");
      //Accept
      delay(clientWait);
      client.println("Accept: */*");
      //Tamaño del envío
      delay(clientWait);
      client.println("Content-Length: " + String(contentLength));
      //tipo de envío
      delay(clientWait);
      client.println("Content-Type: multipart/form-data; boundary=" + boundary);
      //Se acabaron las cabeceras
      delay(clientWait);
      client.println("");
      delay(clientWait);
      //Enviamos el inicio de la solicitud
      client.print(start_request);

      //Debemos enviar el contenido completo del archivo
      //No es conveniente enviar los bytes de uno en uno porque es muy muy lento
      //Tampoco es una buena idea cargar en memoria el archivo completo, porque podría ser 
      //demasiado grande.
      //Como mejor solución crearemos un buffer de memoria dinámico de un tamaña que sea
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
      while(pending>0) {
        //Si podemos llenar el buffer...
        if(pending>=bufferMax) {
          //Lo leemos entero desde el archivo
          f.readBytes((char *)buffer,bufferMax);
          //Reducimos el número de bytes pendientes por leer en el tamaño del buffer
          pending-=bufferMax;
        }
        else { //No tenemos suficiente como para llenar el buffer...
          //Leemos lo que quede
          f.readBytes((char *)buffer,pending);
          //El buffer no está lleno
          bufferSize=pending;
          //Ya no queda nada pendiente
          pending=0;
        }
        //Enviamos el buffer
        delay(clientWait*4);
        client.write(buffer,bufferSize);
      }
      //liberamos la memoria del buffer
      delete[] buffer;
      //Hemos terminado de enviar el contenido del archivo

      //Hemos terminado de trabajar con el archivo. Lo cerramos
      f.close();

      //Enviamos el final de la solicitud
      client.print(end_request);

      //Identificador de imagen
      String file_id;
      //Si se ha enviado correctamente...
      if(_clientFindString(&client,8000,"\"ok\":true")) {
        do {
          //Tomamos nota del campo file_id
          //Incluimos }} como cadena de fin para no tener que esperar al TimeOut
          file_id=_clientFieldString(&client,1000,"file_id","}}");
          //Si contiene algo, lo anotamos como respuesta válida
          if(file_id.length()) answer=file_id;
        }
        //Repetiremos mientras encontremos un file_id. Así localizaremos siempre el último
        while(file_id.length());
      }
    }
  }
  //Devolvemos respuesta
  return answer;
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

  //Respuesta. Inicialmente vacía.
  String answer="";
  
  //Si no hemos inicializado el bot...hemos terminado
  if(!_botToken.length()) return answer;
  
  //Componemos la cadena de parámetros
  String params="{\"chat_id\":\""+chat_id+"\",\"";
  params+=(isPhoto?"photo":"document");
  params+="\":\""+file_id+"\"";
  //Si tiene el parámetro caption...lo añadimos
  if(caption.length()) params+=",\"caption\":\""+caption+"\"";
  //Si tiene el parámetro disable_notification=true...lo añadimos
  if(disable_notification) params+=",\"disable_notification\":\"true\"";
  //Si tiene el parámetro reply_to_message_id...lo añadimos
  if(reply_to_message_id!="0") params+=",\"reply_to_message_id\":\""+reply_to_message_id+"\"";
  //Cerramos la cadena
  params+="}";

  //Conexión de cliente seguro (https)
  WiFiClientSecure client;
  //Con la nueva versión de BearSSL en vez de axSSL, es necesario añadir la siguiente
  //línea para que no se comprueben credenciales ni fingerprints
  client.setInsecure();
  //Si conseguimos abrir conexión segura con Telegram...
  if(client.connect(_telegramHost.c_str(),_telegramPort)) {
    //Enviamos el texto HTML con las cabeceras al servidor Telegram
    //Enviamos URI
    //Distinguimos entre foto y documento. Los comandos son distintos: sendPhoto o sendDocument
    client.println("POST /bot"+_botToken+"/send"+(isPhoto?"Photo":"Document")+" HTTP/1.1");
    //Enviamos el Host
    client.println("Host:" + _telegramHost);
    //Enviamos el content type = json
    client.println("Content-Type: application/json");
    //Enviamos la longitud del contenido
    client.println("Content-Length:"+String(params.length()));
    //Hemos terminado con las cabeceras
    client.println();
    //Hacemos el POST con el cuerpo del mensaje
    client.println(params);

    //Anotar la respuesta como ejemplo en la cabecera de este método
    //Dependiendo del tamaño de la imagen se pueden devolver varios id distintos
    //Cada uno con su tamaño
    //Están ordenados por tamaño de menor a mayor
    //Nosotros siempre devolveremos el mayor (último)
    //El campo a localizar es file_id. Es de texto.

    //Identificador de imagen
    String file_id;
    //Si se ha enviado correctamente...
    if(_clientFindString(&client,8000,"\"ok\":true")) {
      do {
        //Tomamos nota del campo file_id
        //Enviamos como cadena de fin }} para que no tengamos que esperar al TimeOut
        file_id=_clientFieldString(&client,1000,"file_id","}}");
        //Si contiene algo, lo anotamos como respuesta válida
        if(file_id.length()) answer=file_id;
      }
      //Repetiremos mientras encontremos un file_id. Así localizaremos siempre el último
      while(file_id.length());
    }
  }
  //Devolvemos respuesta
  return answer;
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

  //Si el código de acción está permitido...
  if(actionCode<9) {
    //Conexión de cliente seguro (https)
    WiFiClientSecure client;
    //Con la nueva versión de BearSSL en vez de axSSL, es necesario añadir la siguiente
    //línea para que no se comprueben credenciales ni fingerprints
    client.setInsecure();
    //Si conseguimos abrir conexión segura con Telegram...
    if(client.connect(_telegramHost.c_str(),_telegramPort)) {
      //Enviamos el comando
      client.println("GET /bot"+_botToken+"/sendChatAction?chat_id="+chat_id+"&action="+_actionCodes[actionCode]);
      //Anotamos la respuesta de si se ha enviado correctamente
      answer=_clientFindString(&client,8000,"\"ok\":true","}");
    }
    //Cerramos la conexión
    client.stop();
  }
  //Respondemos si se ha enviado correctamente
  return answer;
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

  //Respuesta. Inicialmente vacía.
  String answer="";
  //Si no hemos inicializado el bot...hemos terminado
  if(!_botToken.length()) return answer;
  
  //Componemos la cadena básica de parámetros
  String params="{\"chat_id\":\""+chat_id+"\",\"text\":\""+text+"\"";
  //Si tenemos el parámetro de teclado...
  if(keyboard.length()) {
    //Añadimos los parámetros del teclado
    params+=",\"reply_markup\":{\"keyboard\":"+keyboard;
    //Si tiene el parámetro resize...lo añadimos
    if(resize) params+=",\"resize_keyboard\":true";
    //Si tiene el parámetro oneTime...lo añadimos
    if(oneTime) params+=",\"one_time_keyboard\":true";
    //Cerramos la sección de teclado
    params+="}";
  }
  //Cerramos la cadena de parámetros
  params+="}";
  
  //Conexión de cliente seguro (https)
  WiFiClientSecure client;
  //Con la nueva versión de BearSSL en vez de axSSL, es necesario añadir la siguiente
  //línea para que no se comprueben credenciales ni fingerprints
  client.setInsecure();
  //Si conseguimos abrir conexión segura con Telegram...
  if(client.connect(_telegramHost.c_str(),_telegramPort)) {
    //Enviamos el texto HTML con las cabeceras al servidor Telegram
    //Enviamos URI
    client.println("POST /bot"+_botToken+"/sendMessage HTTP/1.1");
    //Enviamos el Host
    client.println("Host:" + _telegramHost);
    //Enviamos el content type = json
    client.println("Content-Type: application/json");
    //Enviamos la longitud del contenido
    client.println("Content-Length:"+String(params.length()));
    //Hemos terminado con las cabeceras
    client.println();
    //Hacemos el POST con el cuerpo del mensaje
    client.println(params);
    //Respondemos si se ha encontrado la marca de envío correcto
    return _clientFindString(&client,8000,"\"ok\":true");
  }
  //No hemos podido conectar
  return false;
}