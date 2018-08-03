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
    void on_pushButton_Image1_clicked();

    void on_pushButton_Text_clicked();

    void on_pushButton_Image2_clicked();

    void on_pushButton_Image3_clicked();

    void on_pushButton_empty_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
