/*
 * Autor: Ramón Junquera
 * Descripción: Simulación de display en un Label de Qt para la representación de sprites
 * Versión: 20220203
 * 
 * Características:
 *   Simula un display en un QLabel.
 *   La memoria de vídeo es un RoJoSprite.
 *   Admite todas las profundidades de color
 */

#ifndef RoJoSpriteQtLabel_h
#define RoJoSpriteQtLabel_h

#include <RoJoSprite.h> //Gestión de sprites
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QCoreApplication>

class RoJoSpriteQtLabel {
private:
  QLabel *_label=nullptr; //Puntero a etiqueta
  byte _zoom;
public:
  RoJoSprite s; //Puntero de sprite de memoria de video

  //Inicialización
  //Devuelve true si todo es correcto
  bool begin(QLabel *label=nullptr,uint16_t width=1,uint16_t height=1,byte bytesPerPixel=2,byte zoom=1) {
    if(!zoom) return false; //El zoom no puede ser cero
    if(!label) return false; //Si no hay puntero de Label...error
    if(!s.setBytesPerPixel(bytesPerPixel)) return false; //Si no podemos aplicar profundidad...error
    if(!s.setSize(width,height)) return false; //Si no podemos dimensionarlo...error
    _label=label;
    _zoom=zoom;
    return true; //Todo Ok
  }

  //Finalización
  void end() {
    s.end(); //Liberamos sprite de memoria de video
  }

  //Destructor
  ~RoJoSpriteQtLabel() {
    end();
  }

  //Anchura de display para compatibilidad con RoJoGraph2
  uint16_t xMax() {
    return s.xMax();
  }

  //Altura de display para compatibilidad con RoJoGraph2
  uint16_t yMax() {
    return s.yMax();
  }

  //Dibuja el sprite en el QLabel
  //Devuelve true si lo consigue
  bool draw() {
    if(!_label) return false; //Si no hay label...terminamos con error
    uint16_t xMax=s.xMax(); //Anotamos anchura
    uint16_t yMax=s.yMax(); //Anotamos altura
    byte R,G,B; //Valores de los canales de color
    QPixmap *myPixmap=new QPixmap(xMax*_zoom,yMax*_zoom); //Creamos la imagen
    QPainter *myPainter=new QPainter(myPixmap); //Creamos el painter
    for(uint16_t y=0;y<yMax;y++) { //Recorremos filas
      for(uint16_t x=0;x<xMax;x++) { //Recorremos columnas
        s.getColor(s.getPixel(x,y),&R,&G,&B); //Obtenemos color descompuesto
        myPainter->fillRect(x*_zoom,y*_zoom,_zoom,_zoom,QColor(R,G,B)); //Dibujamos pixel teniendo en cuenta el zoom
      }
    }
    delete myPainter; //Liberamos el QPainter
    _label->setPixmap(*myPixmap);  //Asignamos el QPixmap al label
    QCoreApplication::processEvents(); //Procesamos los eventos de dibujo pendientes
    delete myPixmap; //Liberamos el QPixmap
    return true; //Todo Ok
  }

};

#endif
