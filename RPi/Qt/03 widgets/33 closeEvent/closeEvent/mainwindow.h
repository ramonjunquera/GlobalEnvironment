#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>

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
    void on_actionSave_triggered();
    void on_plainTextEdit_textChanged();
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    bool modified; //Se ha modificado?
};

#endif // MAINWINDOW_H
