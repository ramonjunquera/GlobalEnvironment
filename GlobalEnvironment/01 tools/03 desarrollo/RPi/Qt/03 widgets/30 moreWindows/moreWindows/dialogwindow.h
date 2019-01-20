#ifndef DIALOGWINDOW_H
#define DIALOGWINDOW_H

#include <QDialog>

namespace Ui {
class DialogWindow;
}

class DialogWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DialogWindow(QWidget *parent = 0);
    ~DialogWindow();
    bool open(QString *answer);

private slots:
    void on_pushButtonOk_clicked();

private:
    Ui::DialogWindow *ui;
    bool _ok; //Se ha acptado la ventana?
};

#endif // DIALOGWINDOW_H
