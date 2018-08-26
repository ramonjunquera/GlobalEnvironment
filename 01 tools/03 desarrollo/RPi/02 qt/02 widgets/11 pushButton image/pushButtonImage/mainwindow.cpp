/*
 * Ejemplo de botón con imágen
 *
 * Creamos un archivo de recursos y deé l seleccionamos la imagen que pondremos como
 * icono.
 * Es importante que tras crear el archivo de recursos guardemos todo el proyecto para
 * que quede reflejado y se pueda referenciar desde cualquier sitio.
 *
 * Para signar una imagen a un pushButton, seleccionamos la propiedad
 *   QAbstractButton/icon/Normal Off
 * Este es el icono de un botón no pulsado.
 * Se puede cargar una imagen distinta para cada uno de sus estados.
 *
 * Todas la imágenes cargadas son reducidas por defecto a un tamaño de 16x16.
 * Si queremos que conserve su tamaño original, tendremos que indicarlo manualmente.
 *
 * Un botón puede contener imagen y texto al mismo tiempo.
 * En el ejemplo, para que sólo tengan icono, el texto se ha eliminado.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_1_clicked()
{
    //Extraemos la imagen del archivo de recursos
    QPixmap image = QPixmap(":/images/icons/Button-Log-Off-icon.png");
    //La asignamos al label
    ui->label->setPixmap(image);
}


void MainWindow::on_pushButton_2_clicked()
{
    //En este caso obtendremos la imagen del icono del propio botón
    //No podemos convertir directamente un QIcon en un QPixmap
    //Aunque el objeto QIcon tenga el método .pixmap(), éste requiere
    //como parámetro un tamaño.
    //La razón es que QIcon puede contener más de una imagen. Incluso de
    //distintos tamaños y es obligatorio seleccionar cual.
    //Primero tomaremos nota de la lista de todos los tamaños de los iconos
    //que almacena el botón
    //Los guardaremos en una lista de objetos QSize
    QList<QSize> sizes=ui->pushButton_2->icon().availableSizes();
    //La intención es tomar el tamaño más grande de todos los iconos para que
    //no se recorte la imagen
    //Sabemos que los iconos utilizados son cuadrados (misma altura que anchura).
    //Creamos una variable entera en la que guardaremos la anchura máxima de todos
    //las imágenes que guarda el objeto QIcon y la inicializamos con la anchura
    //de la primera imagen
    int maxWidth=sizes[0].width();
    //Ahora recorremos todas las imágenes del icono comenzando por la segunda...
    for(int i=1;i<sizes.size();i++)
        //...y anotamos el mayor valor entre la anchura máxima actual y la del
        //icono procesado
        //(si la anchura del icono procesado es mayor que la máxima, la máxima será
        //la anchura actual)
        maxWidth=qMax(maxWidth,sizes[i].width());
    //Ya sabemos la anchura (y altura) máximas de todas las imágenes del icono.
    //Ahora podemos utilizar el métod pixmap de QIcon
    QPixmap image = ui->pushButton_2->icon().pixmap(QSize(maxWidth,maxWidth));
    //Asignamos la imagen al label
    ui->label->setPixmap(image);
}

void MainWindow::on_pushButton_3_clicked()
{
    //En este caso también utilizaremos el icono del botón para extraer la imagen a
    //aplicar al label. Pero para simplificar el proceso asumiremos que todas las
    //imágenes del icono (si es que tuviese más de una) son del mismo tamaño.
    //Ya no necesitamos calcular el tamaño máximo. Nos quedaremos con el tamaño de
    //la primera imágen.
    QPixmap image= ui->pushButton_3->icon().pixmap(ui->pushButton_3->icon().availableSizes().first());
    //Asignamos la imagen al label
    ui->label->setPixmap(image);
}
