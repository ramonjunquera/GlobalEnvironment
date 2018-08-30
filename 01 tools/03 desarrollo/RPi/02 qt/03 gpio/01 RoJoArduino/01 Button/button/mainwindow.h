#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void on_pushButton_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    const uint8_t _pinLed=16; //Pin con el led
};

#endif // MAINWINDOW_H
