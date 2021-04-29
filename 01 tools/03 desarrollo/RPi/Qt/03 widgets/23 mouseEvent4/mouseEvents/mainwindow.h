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

private:
    Ui::MainWindow *ui;

private slots:
    void mousePressSlot(QPoint& xy,int& button);
    void mouseReleaseSlot(QPoint& xy);
    void mouseMoveSlot(QPoint& xy);
};

#endif // MAINWINDOW_H
