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
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonFile_clicked();

    void on_pushButtonFiles_clicked();

    void on_pushButtonFolder_clicked();

    void on_pushButtonFileSave_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
