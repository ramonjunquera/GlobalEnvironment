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
    void on_pushButton0_clicked();

    void on_pushButton2_clicked();

    void on_pushButtonDelete_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
