#ifndef QLABELMOUSE_H
#define QLABELMOUSE_H

#include <QLabel>
#include <QMouseEvent>

class QLabelMouse : public QLabel
{
    //Obligatorio incluir la macro Q_OBJECT aquí
    Q_OBJECT
public:
    explicit QLabelMouse(QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *ev); //Botón pulsado
    void mouseReleaseEvent(QMouseEvent *ev); //Botón soltado
signals:
    void mousePressSignal(QPoint&,int&);
    void mouseReleaseSignal(QPoint&);
};

#endif // QLABELMOUSE_H
