#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <datachar.h>
#include <datafont.h>
#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>

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
    void on_pushButtonConfig_clicked(bool checked);
    void on_spinBoxChar_valueChanged(int arg1);
    void on_pushButtonZoomPlus_clicked();
    void on_pushButtonZoomMinus_clicked();
    void on_horizontalSliderCharMax_valueChanged(int value);
    void on_horizontalSliderCharMin_valueChanged(int value);
    void on_horizontalSliderChar_valueChanged(int value);
    void on_spinBoxPages_valueChanged(int arg1);
    void mousePressSlot(QPoint& xy,int& button);
    void mouseMoveSlot(QPoint& xy);
    void on_pushButtonOpen_clicked();
    void on_pushButtonSave_clicked();
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    DataFont *font; //Fuente en edición
    bool modified; //La fuente ha sido modificada?
    byte zoom; //Nivel de zoom
    QString loadFileName; //Nombre del último archivo cargado
    void drawPictureChar(); //Crea y muestra la imagen del carácter actual
    void fontModified(); //La fuente se ha modificado
    bool lastColor; //Último color seleccionado dependiendo del botón del mouse
    QString openFileFON(DataFont *newFont,QString filename); //Lee archivo de fuentes
};

#endif // MAINWINDOW_H
