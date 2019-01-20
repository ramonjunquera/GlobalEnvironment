#ifndef QLABELMOUSE_H
#define QLABELMOUSE_H

#include <QLabel>
#include <QMouseEvent>

class QLabelMouse : public QLabel
{
    Q_OBJECT
public:
    explicit QLabelMouse(QWidget *parent = 0);
    void mousePressEvent(QMouseEvent *ev); //Botón pulsado
    void mouseMoveEvent(QMouseEvent *ev); //Mouse en movimiento
signals:
    void mousePressSignal(QPoint&,int&);
    void mouseMoveSignal(QPoint&);

public slots:

};

#endif // QLABELMOUSE_H
