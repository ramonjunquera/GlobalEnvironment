#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug> //Mensajes de debug
#include <RoJoQtPlt.h> //Representación gráfica de funciones
#include <RoJoMakeCirclesQt.h> //Creación de anillos
#include <RoJoNeuralNetworkQt.h> //Redes neuronales
#include <RoJoFloatMatrixQt.h> //Gestión de matrices

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  private slots:
    void on_pushButton_start_clicked();

  private:
    Ui::MainWindow *ui;
    RoJoQtPlt *plt; //Puntero a objeto para salida gráfica
};

#endif // MAINWINDOW_H
