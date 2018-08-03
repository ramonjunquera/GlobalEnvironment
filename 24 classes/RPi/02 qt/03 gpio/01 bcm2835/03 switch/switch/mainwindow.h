#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    const int pinSwitch=21;
    QTimer *myTimer;
    void ChangeBulbStatus();

private slots:
    void TimerLauncher();
};

#endif // MAINWINDOW_H
