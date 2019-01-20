#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void mousePressSlot(QPoint& xy,int& button);
    void mouseMoveSlot(QPoint& xy);

private:
    Ui::MainWindow *ui;

    //Puntero de QPixmap que utilizaremos para editar y asignar al QLabel
    QPixmap *myPixmap;
    //Puntero de QPainter que nos permitirá editar el QPixmap
    QPainter *myPainter;
    //Borra la imágen
    void clear();

};

#endif // MAINWINDOW_H
