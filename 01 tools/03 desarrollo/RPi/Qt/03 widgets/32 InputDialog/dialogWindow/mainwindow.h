#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QInputDialog>

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
    void on_pushButtonText_clicked();

    void on_pushButtonInt_clicked();

    void on_pushButtonDouble_clicked();

    void on_pushButtonMulti_clicked();

    void on_pushButtonItem_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
