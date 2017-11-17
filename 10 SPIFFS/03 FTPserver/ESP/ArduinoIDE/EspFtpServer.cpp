/*
 Autor: Ramón Junquera
 Descripción: Servidor FTP para ESP v20171028
*/

#include "EspFtpServer.h"

#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h> //Para crear el punto de acceso
#else
  #include <ESP8266WiFi.h>
#endif
#include <WiFiClient.h>
#if defined(ESP32)
  #include <SPIFFS.h> //Librería de gestión de SPIFFS para ESP32
#else
  #include <FS.h> //Librería de gestión de SPIFFS para ESP8266
#endif

WiFiServer ftpServer( FTP_CTRL_PORT );
WiFiServer dataServer( FTP_DATA_PORT_PASV );

void FtpServer::begin(String username, String password)
{
  //Anotamos las credenciales
  _FTP_USER= username;
  _FTP_PASS = password;

  ftpServer.begin();
  delay(10);
  dataServer.begin(); 
  delay(10);
  //Calculamos el tiempo de desconexión por inactividad
  //Lo pasamos de minutos a milisegundos
  millisTimeOut = (uint32_t)FTP_TIME_OUT * 60 * 1000;
  millisDelay = 0;
  cmdStatus = 0;
  iniVariables();
}

void FtpServer::iniVariables()
{
  //Anotamos el puerto de datos para el modo pasivo
  dataPort = FTP_DATA_PORT_PASV;
  
  //Iniciamos con la conexión activa
  dataPassiveConn = true;
  
  //Comenzamos desde raíz
  strcpy(cwdName, "/");

  rnfrCmd = false;
  transferStatus = 0;
}

void FtpServer::handleFTP()
{
  if((int32_t)(millisDelay - millis()) > 0) return;

  if (ftpServer.hasClient())
  {
    client.stop();
    client = ftpServer.available();
  }
  
  if(cmdStatus == 0)
  {
    if( client.connected()) disconnectClient();
    cmdStatus = 1;
  }
  else if(cmdStatus == 1) //Servidor FTP esperando conexión
  {
    abortTransfer();
    iniVariables();
    cmdStatus = 2;
  }
  else if(cmdStatus == 2) //Servidor FTP sin tareas pendientes (idle)
  {
    //Si hay un cliente conectado...
    if(client.connected())
    {
      clientConnected();      
      //Esperaremos 10 segundos a que el cliente termine de conectar
      millisEndConnection = millis() + 10*1000;
      cmdStatus = 3;
    }
  }
  else if(readChar()>0) //Obtenemos respuesta
  {
    if( cmdStatus == 3 ) //Servidor FTP esperando credenciales
      if(userIdentity())
        cmdStatus = 4;
      else
        cmdStatus = 0;
    else if(cmdStatus == 4) //Servidor FTP esperando que el usuario se registre
      if(userPassword())
      {
        cmdStatus = 5;
        millisEndConnection = millis() + millisTimeOut;
      }
      else
        cmdStatus = 0;
    else if(cmdStatus == 5) //Servidor FTP esperando comando de usuario
      if(!processCommand())
        cmdStatus = 0;
      else
        millisEndConnection = millis() + millisTimeOut;
  }
  else if (!client.connected() || !client)
  {
    cmdStatus = 1;
  }

  if(transferStatus == 1) //Obtener datos
  {
    if(!doRetrieve()) transferStatus=0;
  }
  else if(transferStatus == 2) //Guardar datos
  {
    if(!doStore()) transferStatus = 0;
  }
  else if(cmdStatus > 2 && !((int32_t)(millisEndConnection - millis()) > 0))
  {
    client.println("530 Timeout");
    millisDelay = millis() + 200; //Esperamos 200ms
    cmdStatus = 0;
  }
}

void FtpServer::clientConnected()
{
  client.println( "220--- Welcome to FTP for ESP ---");
  client.println( "220---   By RoJo   ---");
  client.println( "220 --   Version "+ String(FTP_SERVER_VERSION) +"   --");
  iCL = 0;
}

void FtpServer::disconnectClient()
{
  abortTransfer();
  client.println("221 Goodbye");
  client.stop();
}

bool FtpServer::userIdentity()
{  
  if(strcmp(command,"USER")) client.println("500 Syntax error");
  if(strcmp(parameters,_FTP_USER.c_str()))
  {
    client.println("530 user not found");
  }
  else
  {
    client.println("331 OK. Password required");
    strcpy(cwdName,"/");
    return true;
  }
  millisDelay = millis() + 100;  //Esperamos 100ms
  return false;
}

bool FtpServer::userPassword()
{
  if(strcmp(command,"PASS"))
    client.println("500 Syntax error");
  else if(strcmp(parameters,_FTP_PASS.c_str()))
    client.println("530 ");
  else
  {
    client.println("230 OK.");
    return true;
  }
  millisDelay = millis() + 100;  //Esperamos 100ms
  return false;
}

bool FtpServer::processCommand()
{
  //CDUP - Change to Parent Directory 
  if(!strcmp(command,"CDUP"))
    client.println("250 Ok. Current directory is " + String(cwdName));

  //CWD - Change Working Directory
  else if(!strcmp(command,"CWD"))
  {
    char path[ FTP_CWD_SIZE ];
    if(strcmp(parameters,".")==0) // 'CWD .' es lo mismo que que comando PWD
      client.println("257 \"" + String(cwdName) + "\" is your current directory");
    else 
      client.println("250 Ok. Current directory is " + String(cwdName));
  }
  
  //PWD - Print Directory
  else if(!strcmp(command,"PWD"))
    client.println("257 \"" + String(cwdName) + "\" is your current directory");
    
  //QUIT
  else if(!strcmp(command,"QUIT"))
  {
    disconnectClient();
    return false;
  }

  //MODE - Transfer Mode 
  else if(!strcmp(command,"MODE"))
  {
    if(!strcmp(parameters,"S"))
      client.println("200 S Ok");
    else
      client.println("504 Only S(tream) is suported");
  }
  
  //PASV - Passive Connection management
  else if(!strcmp(command,"PASV"))
  {
    if (data.connected()) data.stop();
    
    dataIp = WiFi.localIP();
    dataIp = client.localIP();
    
    dataPort = FTP_DATA_PORT_PASV;
    client.println("227 Entering Passive Mode ("+ String(dataIp[0]) + "," + String(dataIp[1])+","+ String(dataIp[2])+","+ String(dataIp[3])+","+String( dataPort >> 8 ) +","+String ( dataPort & 255 )+").");
    dataPassiveConn = true;
  }
  
  //PORT - Data Port
  else if(!strcmp(command,"PORT"))
  {
    if(data) data.stop();
    //Obtenemos la ip de datos del cliente
    dataIp[0]=atoi(parameters);
    char *p = strchr(parameters,',');
    for(byte i=1;i<4;i++)
    {
      dataIp[i] = atoi(++p);
      p = strchr(p,',');
    }
    //Obtenemos el puerto de datos del cliente
    dataPort = 256 * atoi(++p);
    p = strchr(p,',');
    dataPort += atoi(++p);
    if(p == NULL)
      client.println( "501 Can't interpret parameters");
    else
    {
      client.println("200 PORT command successful");
      dataPassiveConn = false;
    }
  }
  
  //STRU - File Structure
  else if(!strcmp(command,"STRU"))
  {
    if(!strcmp(parameters,"F")) client.println("200 F Ok");
    else client.println("504 Only F(ile) is suported");
  }
  
  //TYPE - Data Type
  else if(!strcmp(command,"TYPE"))
  {
    if(!strcmp(parameters,"A"))
      client.println("200 TYPE is now ASII");
    else if(!strcmp(parameters,"I"))
      client.println("200 TYPE is now 8-bit binary");
    else
      client.println("504 Unknow TYPE");
  }

  //ABOR - Abort
  else if(!strcmp(command,"ABOR"))
  {
    abortTransfer();
    client.println("226 Data connection closed");
  }
  
  //DELE - Delete a File 
  else if(!strcmp(command,"DELE"))
  {
    char path[ FTP_CWD_SIZE ];
    if(strlen(parameters)==0)
      client.println("501 No file name");
    else if(makePath(path))
    {
      if(!SPIFFS.exists(path))
        client.println("550 File " + String(parameters) + " not found");
      else
      {
        if(SPIFFS.remove(path))
          client.println("250 Deleted " + String(parameters) );
        else
          client.println("450 Can't delete " + String(parameters));
      }
    }
  }
  
  //LIST - List 
  else if(!strcmp(command,"LIST"))
  {
    if(!dataConnect())
      client.println("425 No data connection");
    else
    {
      client.println("150 Accepted data connection");
      #if defined(ESP32)
        uint16_t itemsCount = 0; //Número de entradas en la carpeta actual
        if(!SPIFFS.exists(cwdName))
          client.println("550 Can't open directory " + String(cwdName) );
        else
        {
          File dir=SPIFFS.open(cwdName); 
          file = dir.openNextFile();
          while (file)
          {
            String fileName,fileSize;
            fileName = file.name();
            fileName.remove(0,1);
            fileSize = String(file.size());
            data.println("+r,s" + fileSize);
            data.println(",\t" + fileName );
            itemsCount++;
            file = dir.openNextFile();
          }
          file.close();
          dir.close();
          client.println("226 " + String(itemsCount) + " matches total");
        }
      #else
        uint16_t nm = 0;
        Dir dir=SPIFFS.openDir(cwdName);
        if( !SPIFFS.exists(cwdName))
          client.println( "550 Can't open directory " + String(cwdName) );
        else
        {
          while( dir.next())
          {
            String fn, fs;
            fn = dir.fileName();
            fn.remove(0, 1);
            fs = String(dir.fileSize());
            data.println( "+r,s" + fs);
            data.println( ",\t" + fn );
            nm ++;
          }
          client.println( "226 " + String(nm) + " matches total");
        }
      #endif
      data.stop();
    }
  }
  
  //MLSD - Listing for Machine Processing (see RFC 3659)
  else if(!strcmp(command,"MLSD"))
  {
    if(!dataConnect())
      client.println("425 No data connection MLSD");
    else
    {
      client.println("150 Accepted data connection");
      uint16_t nm = 0;
      #if defined(ESP32)
        File dir=SPIFFS.open(cwdName);
        char dtStr[15];
        file = dir.openNextFile();
        while (file)
        {
          String fn,fs;
          fn = file.name();
          fn.remove(0,1);
          fs = String(file.size());
          data.println("Type=file;Size=" + fs + ";"+"modify=19670527163000;" +" " + fn);
          nm ++;
          file = dir.openNextFile();
        }
        file.close();
        client.println("226-options: -a -l");
        client.println("226 " + String(nm) + " matches total");
        dir.close();
      #else
        Dir dir= SPIFFS.openDir(cwdName);
        char dtStr[ 15 ];
        while( dir.next())
        {
          String fn,fs;
          fn = dir.fileName();
          fn.remove(0, 1);
          fs = String(dir.fileSize());
          data.println( "Type=file;Size=" + fs + ";"+"modify=19670527163000;" +" " + fn);
          nm ++;
        }
        client.println( "226-options: -a -l");
        client.println( "226 " + String(nm) + " matches total");
      #endif
      data.stop();
    }
  }
  
  //NLST - Name List 
  else if(!strcmp(command,"NLST"))
  {
    if(!dataConnect())
      client.println("425 No data connection");
    else
    {
      client.println("150 Accepted data connection");
      uint16_t nm = 0;
      #if defined(ESP32)
        File dir=SPIFFS.open(cwdName);
        if(!dir)
          client.println("550 Can't open directory " + String(parameters));
        else
        {
          if(!dir.isDirectory())
            client.println("550 It's not a directory " + String(parameters));
          else
          {
            file = dir.openNextFile();
            while(file)
            {
              data.println(file.name());
              nm ++;
              file = dir.openNextFile();
            }
            file.close();
            client.println("226 " + String(nm) + " matches total");
          }
        }
        dir.close();
      #else
        Dir dir=SPIFFS.openDir(cwdName);
        if( !SPIFFS.exists( cwdName ))
          client.println( "550 Can't open directory " + String(parameters));
        else
        {
          while( dir.next())
          {
            data.println( dir.fileName());
            nm ++;
          }
          client.println( "226 " + String(nm) + " matches total");
        }
      #endif
      data.stop();
    }
  }
  
  //NOOP
  else if(!strcmp(command,"NOOP"))
  {
    client.println( "200 Zzz...");
  }
  
  //RETR - Retrieve
  else if(!strcmp(command,"RETR"))
  {
    char path[ FTP_CWD_SIZE ];
    if(strlen(parameters)==0)
      client.println("501 No file name");
    else if(makePath(path))
    {
      file = SPIFFS.open(path,"r");
      if(!file)
        client.println("550 File " +String(parameters)+ " not found");
      else if(!file)
        client.println("450 Can't open " +String(parameters));
      else if(!dataConnect())
        client.println("425 No data connection");
      else
      {
        client.println("150-Connected to port "+ String(dataPort));
        client.println("150 " + String(file.size()) + " bytes to download");
        millisBeginTrans = millis();
        bytesTransfered = 0;
        transferStatus = 1;
      }
    }
  }
  
  //STOR - Store
  else if(!strcmp(command,"STOR"))
  {
    char path[ FTP_CWD_SIZE ];
    if(strlen(parameters)==0)
      client.println("501 No file name");
    else if(makePath(path))
    {
      file = SPIFFS.open(path,"w");
      if(!file)
        client.println("451 Can't open/create " +String(parameters) );
      else if(!dataConnect())
      {
        client.println("425 No data connection");
        file.close();
      }
      else
      {
        client.println( "150 Connected to port " + String(dataPort));
        millisBeginTrans = millis();
        bytesTransfered = 0;
        transferStatus = 2;
      }
    }
  }
  
  //MKD - Make Directory
  else if(!strcmp(command,"MKD"))
    client.println("550 Can't create \"" + String(parameters)); //No soportado en SPIFFS
   
  //RMD - Remove a Directory 
  else if(!strcmp(command,"RMD"))
    client.println("501 Can't delete \"" +String(parameters));
    
  //RNFR - Rename From 
  else if(!strcmp(command,"RNFR"))
  {
    buf[0] = 0;
    if(strlen(parameters)==0)
      client.println("501 No file name");
    else if(makePath(buf))
    {
      if(!SPIFFS.exists(buf))
        client.println("550 File " +String(parameters)+ " not found");
      else
      {
        client.println("350 RNFR accepted - file exists, ready for destination");     
        rnfrCmd = true;
      }
    }
  }

  //RNTO - Rename To 
  else if(!strcmp(command,"RNTO"))
  {  
    char path[ FTP_CWD_SIZE ];
    char dir[ FTP_FIL_SIZE ];
    if(strlen(buf)==0 || !rnfrCmd)
      client.println("503 Need RNFR before RNTO");
    else if(strlen(parameters)==0)
      client.println("501 No file name");
    else if(makePath(path))
    {
      if(SPIFFS.exists(path))
        client.println("553 " +String(parameters)+ " already exists");
      else
      {   
        if(SPIFFS.rename(buf,path))
          client.println("250 File successfully renamed or moved");
        else
          client.println("451 Rename/move failure");
      }
    }
    rnfrCmd = false;
  }

  //FEAT - New Features
  else if(!strcmp(command,"FEAT"))
  {
    client.println( "211-Extensions suported:");
    client.println( " MLSD");
    client.println( "211 End.");
  }
  
  //MDTM - File Modification Time (see RFC 3659)
  else if (!strcmp(command,"MDTM"))
    client.println("550 Unable to retrieve time");

  //SIZE - Size of the file
  else if(!strcmp(command,"SIZE"))
  {
    char path[ FTP_CWD_SIZE ];
    if(strlen(parameters)==0)
      client.println("501 No file name");
    else if(makePath(path))
    {
      file = SPIFFS.open(path,"r");
      if(!file)
         client.println("450 Can't open " +String(parameters) );
      else
      {
        client.println("213 " + String(file.size()));
        file.close();
      }
    }
  }

  //SITE - System command
  else if(!strcmp(command,"SITE"))
    client.println("500 Unknow SITE command " +String(parameters) );
  else
    client.println("500 Unknow command");
  
  return true;
}

bool FtpServer::dataConnect()
{
  unsigned long startTime = millis();
  //Esperamos 5 segundos para la conexión de datos
  if (!data.connected())
  {
    while(!dataServer.hasClient() && millis() - startTime < 10000)
    {
      yield();
    }
    if (dataServer.hasClient())
    {
      data.stop();
      data = dataServer.available();
    }
  }
  return data.connected();
}

bool FtpServer::doRetrieve()
{
  int16_t nb = file.readBytes(buf,FTP_BUF_SIZE);
  if(nb>0)
  { 
    data.write((uint8_t*) buf,nb);
    bytesTransfered += nb;
    return true;
  }
  closeTransfer();
  return false;
}

bool FtpServer::doStore()
{
  if(data.connected())
  {
    int16_t nb = data.readBytes((uint8_t*) buf,FTP_BUF_SIZE);
    if(nb>0)
    {
      file.write((uint8_t*) buf,nb);
      bytesTransfered += nb;
    }
    return true;
  }
  closeTransfer();
  return false;
}

void FtpServer::closeTransfer()
{
  uint32_t deltaT = (int32_t) (millis() - millisBeginTrans);
  if(deltaT>0 && bytesTransfered>0)
  {
    client.println("226-File successfully transferred");
    client.println("226 " + String(deltaT) + " ms, "+ String(bytesTransfered / deltaT) + " kbytes/s");
  }
  else
    client.println("226 File successfully transferred");
  
  file.close();
  data.stop();
}

void FtpServer::abortTransfer()
{
  if(transferStatus>0)
  {
    file.close();
    data.stop(); 
    client.println("426 Transfer aborted");
  }
  transferStatus = 0;
}

//Lee un carácter de la conexión cliente al servidor FTP
//Actualiza la línea de comandos (cmdLine) y los buffers
//de comandos, iCL y punteros de parámetros.
//Devuelve:
//  -2 si el buffer cmdLine está lleno
//  -1 si la línea no está completa
//   0 si se recibe una línea vacía
//  longitud de cmdLine (positivo) if no empty line received 
int8_t FtpServer::readChar()
{
  int8_t rc = -1;

  if(client.available())
  {
    char c = client.read();
    if(c == '\\') c = '/';
    if(c != '\r')
      if(c != '\n')
      {
        if(iCL < FTP_CMD_SIZE)
          cmdLine[iCL++] = c;
        else
          rc = -2; //Error. Línea demasiado larga
      }
      else
      {
        cmdLine[iCL] = 0;
        command[0] = 0;
        parameters = NULL;
        //Si es una línea vacía...
        if(iCL==0)
          //...devolvemos el código de línea vacía
          rc = 0;
        else
        {
          rc = iCL;
          //Buscaremos espacios entre el comando y sus parámetros
          parameters = strchr(cmdLine,' ');
          if(parameters != NULL)
          {
            if(parameters - cmdLine > 4)
              rc = -2; //Error. Línea demasiado larga
            else
            {
              strncpy(command,cmdLine,parameters - cmdLine);
              command[parameters - cmdLine]=0;

              //Aumentamos el puntero de los parámetros mientras encontremos espacios
              while(*(++parameters) == ' ');
            }
          }
          else if(strlen(cmdLine)>4)
            rc = -2; //Error. Línea demasiado larga
          else
            strcpy(command,cmdLine);
          iCL=0;
        }
      }
    if(rc>0)
      for(byte i=0;i<strlen(command);i++)
        command[i]=toupper(command[i]);
    if(rc==-2)
    {
      iCL = 0;
      client.println("500 Syntax error");
    }
  }
  return rc;
}

//Cremos el path/nombre completo de cwdName y sus parámetros
//Tenemos 3 posibles casos. Los parámetros pueden ser:
// - El path absoluto
// - El path relativo
// - Sólo el nombre
//Devolvemos true si se pudo completar con correctamente
bool FtpServer::makePath(char *fullName)
{
  return makePath(fullName,parameters);
}

bool FtpServer::makePath(char *fullName,char *param)
{
  if(param == NULL) param = parameters;
    
  //Si es raíz o está vacío...
  if(strcmp(param,"/")==0 || strlen(param)==0)
  {
    //...el resultado será la carpeta raíz
    strcpy(fullName,"/");
    return true;
  }
  //Si es un path relativo...
  if(param[0]!='/') 
  {
    //...le añadimos como prefijo la carpeta actual
    strcpy(fullName,cwdName);
    if(fullName[strlen(fullName)-1] != '/') strncat(fullName,"/",FTP_CWD_SIZE);
    strncat(fullName,param,FTP_CWD_SIZE );
  }
  else
    strcpy(fullName,param);
  //Calculamos la longitud del path
  uint16_t strl=strlen(fullName)-1;
  //Si el path finaliza con '/'...se lo quitamos
  if(fullName[strl] == '/' && strl>1) fullName[strl]=0;
  //Si la longitud total es menor que la máxima permitida...todo correcto
  if(strlen(fullName)<FTP_CWD_SIZE) return true;
  
  client.println("500 Command line too long");
  return false;
}

//Calcula año, mes, día, hora, minuto y segundo
//del primer parámetro enviado por el comando MDTM
//que tiene el formato YYYYMMDDHHMMSS
//Devuelve:
//  0 si el parámetro no tiene el formato correcto
//  longitud del parámetro + un espacio
byte FtpServer::getDateTime(uint16_t *pyear,byte *pmonth,byte *pday,byte *phour,byte *pminute,byte *psecond)
{
  char dt[15];

  //La longitud del formato de fecha es 14
  //que termina con un espacio seguido del nombre del archivo
  
  //Si la longitud total de los parámetros es menor que la mínima
  //o el carácter 15 (posición 14) no es un espacio...
  //...no tiene el formato correcto
  if(strlen(parameters)<15 || parameters[14]!=' ') return 0;
  //Recorremos los 14 primeros caracteres...
  for(byte i=0;i<14;i++)
    //Si el carácter procesado no es un dígito...el formato no es correcto
    if(!isdigit(parameters[i])) return 0;

  strncpy(dt,parameters,14);
  dt[14]=0;
  *psecond = atoi(dt+12); 
  dt[12]=0;
  *pminute = atoi(dt+10);
  dt[10]=0;
  *phour = atoi(dt+8);
  dt[8] = 0;
  *pday = atoi(dt+6);
  dt[6] = 0;
  *pmonth = atoi(dt+4);
  dt[4] = 0;
  *pyear = atoi(dt);
  return 15;
}

//Creamos la cadena YYYYMMDDHHMMSS con la fecha y la hora
//Devuelve el puntero a la cadena
char* FtpServer::makeDateTimeStr(char *tstr,uint16_t date,uint16_t time)
{
  sprintf(tstr,"%04u%02u%02u%02u%02u%02u",
           ((date & 0xFE00) >> 9 ) + 1980, (date & 0x01E0) >> 5, date & 0x001F,
           (time & 0xF800) >> 11, (time & 0x07E0) >> 5, (time & 0x001F ) << 1 );            
  return tstr;
}

