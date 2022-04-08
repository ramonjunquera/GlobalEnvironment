#include <QApplication>
#include <QDir> //Para obtener la carpeta del ejecutable
#include <QMediaPlayer> //Reproductor multimedia
#include <QVideoWidget> //Widget que muestra el video
#include <QDesktopWidget> //Para obtener el tamaño de pantalla
#include <QMediaPlaylist> //Para listas de reproducción

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  QString currentPath=(new QDir)->absolutePath(); //Carpeta que contiene la aplicación
  QMediaPlaylist playList; //Cremos una lista de reproducción
  playList.addMedia(QUrl::fromLocalFile(currentPath+"/v2.m4v")); //Añadimos el primer archivo
  playList.addMedia(QUrl::fromLocalFile(currentPath+"/v3.m4v")); //Añadimos el segundo archivo
  playList.setPlaybackMode(QMediaPlaylist::Loop); //Reproducción en bucle

  QMediaPlayer* mPlayer=new QMediaPlayer; //Instanciamos reproductor multimedia
  QVideoWidget* vWidget=new QVideoWidget; //Instanciamos Widget de video
  mPlayer->setVideoOutput(vWidget); //El reproductor sacará el video por el Widget
  mPlayer->setPlaylist(&playList); //Asignamos lista de reproducción
  //Si quisieramos reproducir un único archivo no necesitamos crear una lista de reproducción
  //Simplemente indicamos el path del archivo de video de esta manera:
  //mPlayer->setMedia(QUrl::fromLocalFile(currentPath+"/v2.m4v"));

  QRect desktopSize = QApplication::desktop()->rect(); //Obtenemos el tamaño de pantalla
  //Definimos la posición y tamaño del rectángulo que mostrará el video
  //0,0 : desde el primer pixel de arriba a la izquierda
  //desktopSize.width(),desktopSize.height() : ocupando toda la pantalla
  //El video se reproducirá a pantalla completa
  //El Widget tiene bordes de ventana y controles para maximizar, cerrar, etc. Como cualquier otra ventana
  //Pero en este caso NO se verán los bordes de la ventana porque quedan fuera por fuera de la zona visible
  //Para cerrar la ventana podemos utilizar Alt+F4 o Alt y arrastrar la ventana con el ratón para moverla y cerrarla.
  vWidget->setGeometry(0,0,desktopSize.width(),desktopSize.height());
  vWidget->show(); //Mostramos el Widget de video
  mPlayer->play(); //Reproducimos. En este caso la lista de reproducción asignada

  //El programa finaliza, pero la ventana con el Widget de video continua con la reproducción
  return a.exec();
}
