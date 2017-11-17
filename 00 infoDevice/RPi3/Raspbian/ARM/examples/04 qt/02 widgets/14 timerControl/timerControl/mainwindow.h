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
    QTimer *myTimer;

private slots:
    void BulbChangeStatus();
    void TimerLauncherOnce();
    void on_pushButton_Start_clicked();
    void on_pushButton_End_clicked();
    void on_pushButton_Once_clicked();
};

#endif // MAINWINDOW_H
