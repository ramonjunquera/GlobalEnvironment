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
    void test01();
    void test02();
    void test03();
    void test04();
    void test05();
    void test06();
    void test07();
    void test08();
    void test09();
    void test10();
    void test11();
    void test12();
    void test13();
    void test14();
    void test15();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
