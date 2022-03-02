#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMessageBox>
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

    void on_action4_triggered();

    void on_action5_triggered();

    void on_action6_triggered();

    void on_action7_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
