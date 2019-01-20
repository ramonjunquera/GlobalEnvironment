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
    void mousePressEvent(QMouseEvent *ev);
signals:
    void mousePressSignal(QPoint&);
};

#endif // QLABELMOUSE_H
