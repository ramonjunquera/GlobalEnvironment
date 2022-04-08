#include "mainwindow.h"
#include "ui_mainwindow.h"

//Constructor
MainWindow::MainWindow(QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow) {
  ui->setupUi(this);
  mMediaPlayer=new QMediaPlayer; //Nueva instancia de objeto reproductor multimedia
  mVideoWidget=new QVideoWidget; //Nueva instancia Widget de video hijo en la ventana actual
  mMediaPlayer->setVideoOutput(mVideoWidget); //Indicamos al reproductor el Widget de video
  ui->vL1->addWidget(mVideoWidget); //Incluimos el Widget de video en el Layout vertical superior

  //Cuando cambie la duración del audio, actualizaremos el valor máximo de la barra de progreso
  connect(mMediaPlayer,&QMediaPlayer::durationChanged,[&](qint64 duration) {
    ui->progressBar->setMaximum(static_cast<int>(duration));
  });

  //Cuando cambie la posición de la reproducción del audio, actualizaremos el valor de la barra de progreso
  connect(mMediaPlayer,&QMediaPlayer::positionChanged,[&](qint64 position) {
    ui->progressBar->setValue(static_cast<int>(position));
  });

  //Cuando se haga click en el botón Play, se inicia la reproducción del objeto de audio
  connect(ui->pushButtonPlay,&QPushButton::clicked,[&]() {
    mMediaPlayer->play();
  });

  //Cuando se haga click en el botón Pause, se pausa la reproducción del objeto de audio
  connect(ui->pushButtonPause,&QPushButton::clicked,[&]() {
    mMediaPlayer->pause();
  });

  //Cuando se haga click en el botón Stop, se detiene la reproducción del objeto de audio
  connect(ui->pushButtonStop,&QPushButton::clicked,[&]() {
    mMediaPlayer->stop();
  });

  //Cuando el botón Mute cambie de estado, se apagar/encenderá el audio
  connect(ui->pushButtonMute,&QPushButton::toggled,[&](bool checked) {
    mMediaPlayer->setMuted(checked);
  });

  //Cuando cambie el valor del Slider de volumen, se asignará ese valor al volumen de la reproducción de audio
  connect(ui->sliderVolume,&QSlider::valueChanged,[&](int value) {
    mMediaPlayer->setVolume(value);
  });

  //Cuando se pulse el botón Open...
  connect(ui->pushButtonOpenFile,&QPushButton::clicked,[&](){
    QString filename=QFileDialog::getOpenFileName(this,"Select an audio file");
    if(filename.isEmpty()) return; //Si no se ha seleccionado nada...no hacemos nada
    mMediaPlayer->stop(); //Por si está reproduciendo algo
    mMediaPlayer->setMedia(QUrl::fromLocalFile(filename)); //Indicamos el archivo con el audio al objeto de reproducción
    mMediaPlayer->setVolume(ui->sliderVolume->value()); //Asignamos el volumen actual
    mMediaPlayer->play(); //Comienza la reproducción
  });
}

//Destructor
MainWindow::~MainWindow() {
  delete ui;
}
