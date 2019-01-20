#include "qlabelmouse.h"

QLabelMouse::QLabelMouse(QWidget *parent):QLabel(parent)
{
    //Nada especial que hacer aquí
}

void QLabelMouse::mousePressEvent(QMouseEvent *ev)
{
    //Creamos el objeto QPoint y guardamos el él las coordenadas del click
    QPoint xy = ev->pos();
    //Emitimos una señal del tipo mousePressSignal
    emit mousePressSignal(xy);
}


