#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void timerLauncher();
  void on_pushButtonStartStop_toggled(bool checked);

private:
  Ui::MainWindow *ui;
  QTimer *myTimer;
};

#endif // MAINWINDOW_H
