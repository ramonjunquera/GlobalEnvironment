#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = 0);
    ~LoginWindow();
    bool begin();

private slots:


    void on_pushButtonOk_clicked();

private:
    Ui::LoginWindow *ui;
    bool _ok; //Las credenciales se han validado correctamente?
};

#endif // LOGINWINDOW_H
