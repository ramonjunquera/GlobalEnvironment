#include "qlabelmouse.h"

QLabelMouse::QLabelMouse(QWidget *parent):QLabel(parent)
{
    //Nada especial que hacer aquí
}

void QLabelMouse::mousePressEvent(QMouseEvent *ev)
{
    //Se ejecuta cuando se pulsa el botón del mouse

    //Creamos el objeto QPoint y guardamos el él las coordenadas del click
    QPoint xy = ev->pos();
    //Anotamos el botón que lo ha pulsado
    //Qt::LeftButton = 1
    //Qt::RightButton = 2
    //Qt::MidButton = 4
    int button=ev->button(); //Qt::
    //Emitimos una señal del tipo mousePressSignal
    emit mousePressSignal(xy,button);
}

void QLabelMouse::mouseReleaseEvent(QMouseEvent *ev)
{
    //Se ejecuta cuando se suelta el botón del mouse

    //Creamos el objeto QPoint y guardamos el él las coordenadas del click
    QPoint xy = ev->pos();
    //Emitimos una señal del tipo mouseReleaseSignal
    emit mouseReleaseSignal(xy);
}

void QLabelMouse::mouseMoveEvent(QMouseEvent *ev)
{
    //Se ejecuta cuando el mouse se ha movido

    //Creamos el objeto QPoint y guardamos el él las coordenadas del click
    QPoint xy = ev->pos();
    //Emitimos una señal del tipo mouseMoveSignal
    emit mouseMoveSignal(xy);
}


