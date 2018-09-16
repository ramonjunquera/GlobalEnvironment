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
    void on_action1_triggered();

    void on_action2_triggered();

    void on_action3_triggered();

    void on_actionExit_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
