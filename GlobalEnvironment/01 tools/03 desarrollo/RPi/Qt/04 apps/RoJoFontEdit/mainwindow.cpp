#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //Inicialización de variables globales
    modified = false; //Inicialmente la fuente no ha sido modificada
    zoom = 20; //Nivel inicial de zoom
    loadFileName = "newfont.fon"; //Nombre de archivo inicial
    font = new DataFont(); //Creamos el objeto de la fuente
    ui->frameSubMenu->setVisible(false); //Inicialmente no se muestra el submenú
    //Conectamos las signal de QLabelMouse con los slots
    connect(ui->labelImage,SIGNAL(mousePressSignal(QPoint&,int&)),this,SLOT(mousePressSlot(QPoint&,int&)));
    connect(ui->labelImage,SIGNAL(mouseMoveSignal(QPoint&)),this,SLOT(mouseMoveSlot(QPoint&)));
    //Llama a la función que crea una nueva imagen y muestra en ella el carácter actual
    drawPictureChar();
}

MainWindow::~MainWindow()
{
    delete ui;
    //Eliminamos el objeto de la fuente
    font->clear();
    delete font;
}

void MainWindow::drawPictureChar()
{
    //Crea una nueva images vacía del tamaño del carácter actual
    //Tiene en cuenta el zoom

    //Calculamos el tamaño del carácter
    byte charX = font->dataChar[ui->horizontalSliderChar->value() - font->charMin].width;
    byte charY = (byte)(font->pages * 8);
    //Calculamos el tamaño de la imagen
    int picX = zoom * charX;
    int picY = zoom * charY;
    //Borramos el QPixmap
    //Creamos la imagen
    QPixmap *myPixmap=new QPixmap(picX,picY);
    //Creamos el painter
    QPainter *myPainter=new QPainter(myPixmap);
    //Lo pintamos de blanco con un rectángulo relleno
    myPainter->fillRect(0,0,picX,picY,QColor(255,255,255));
    //Asignamos color negro al lápiz por defecto
    myPainter->setPen(QColor(0,0,0)); //Negro
    //Recorreremos todos los pixels del carácter y lo dibujaremos en grande
    for(byte x=0;x<charX;x++)
        for(byte y=0;y<charY;y++)
        {
            //Anotamos si el pixel está activo
            bool status = (font->dataChar[ui->horizontalSliderChar->value() - font->charMin].data[x + (y / 8) * font->dataChar[ui->horizontalSliderChar->value() - font->charMin].width] & 1 << (y % 8)) > 0;
            //Calculamos posición inicial del pixel
            int x0 = x * zoom;
            int y0 = y * zoom;
            //Si hay que dibujar el pixel...pintamos un rectángulo negro dejando
            //un borde blanco para seguir viendo la rejilla
            if(status) myPainter->fillRect(x0+1,y0+1,zoom-2,zoom-2,QColor(0,0,0));
            //Sino, sólo pintamos los bordes del rectángulo
            else myPainter->drawRect(x0,y0,zoom-1,zoom-1);
        }
    //Liberamos el QPainter
    delete myPainter;
    //Asignamos el QPixmap al label
    ui->labelImage->setPixmap(*myPixmap);
    //Liberamos el QPixmap
    delete myPixmap;
}

void MainWindow::fontModified()
{
    //La fuente se ha modificado

    modified = true;
    ui->pushButtonSave->setEnabled(true);
}

void MainWindow::on_pushButtonConfig_clicked(bool checked)
{
    //Se ha pulsado el botón de configuración

    //El submenú se mostrará dependiendo del estado del botón
    ui->frameSubMenu->setVisible(checked);
}

void MainWindow::on_spinBoxChar_valueChanged(int arg1)
{
    //Se ha cambiado la anchura del carácter actual

    //Anotamos el índice del carácter editado
    byte charIndex = (byte)(ui->horizontalSliderChar->value() - font->charMin);
    //Anotamos la anchura antigua (la actual)
    byte oldWidth = font->dataChar[charIndex].width;
    //Anotamos la anchura nueva
    byte newWidth = (byte)arg1;
    //Si las anchuras antigua y nueva son iguales...hemos terminado
    if (oldWidth == newWidth) return;
    //La anchura ha cambiado!!

    //Redimensionamos la anchura del carácter editado
    font->dataChar[charIndex].redimWidth(newWidth, font->pages);
    //Redibujamos el carácter completo
    drawPictureChar();
    //La fuente se ha modificado
    fontModified();
}

void MainWindow::on_pushButtonZoomPlus_clicked()
{
    //Pulsado el botón para aumentar el zoom

    //Aumentamos el zoom
    zoom++;
    //Si hemos llegado al máximo zoom...desactivamos el botón
    if (zoom == 255) ui->pushButtonZoomPlus->setEnabled(false);
    //Activamos el botón para reducir el zoom
    ui->pushButtonZoomMinus->setEnabled(true);
    //Redibujamos el carácter
    drawPictureChar();
}

void MainWindow::on_pushButtonZoomMinus_clicked()
{
    //Pulsado el botón para reducir el zoom

    //Reducimos el zoom
    zoom--;
    //Si hemos llegado al mínimo zoom...desactivamos el botón
    if (zoom == 5) ui->pushButtonZoomMinus->setEnabled(false);
    //Activamos el botón para aumentar el zoom
    ui->pushButtonZoomPlus->setEnabled(true);
    //Redibujamos el carácter
    drawPictureChar();
}

void MainWindow::on_horizontalSliderCharMax_valueChanged(int value)
{
    //Ha cambiado el carácter final de la fuente

    //Tomamos nota del nuevo valor
    byte newCharMax = (byte)value;
    //Tomamos nota del antiguo valor (el actual)
    byte oldCharMax = font->charMax;
    //Siempre actualizamos el cuadro que muestra el índice
    ui->labelCharMaxCode->setText(QString::number(value));
    ui->labelCharMaxASCII->setText(QChar(newCharMax));
    //Siempre charMin<=charMax
    if (newCharMax < ui->horizontalSliderCharMin->value())
    {
        //Actualizamos el valor de la barra de charMin
        ui->horizontalSliderCharMin->setValue(newCharMax);
    }
    //Siempre charMax < carácter seleccionado
    if (newCharMax < ui->horizontalSliderChar->value()) ui->horizontalSliderChar->setValue(newCharMax);
    //Si ha cambiado el valor del último carácter...
    if (newCharMax != oldCharMax)
    {
        //Redimensionamos la fuente cambiando el último carácter
        font->redimCharMax(newCharMax);
        //La fuente se ha modificado
        fontModified();
        //Cambiamos el límite del selector de caracteres
        ui->horizontalSliderChar->setMaximum(newCharMax);
        //Asignamos la anchura correcta
        ui->spinBoxChar->setValue(font->dataChar[ui->horizontalSliderChar->value() - font->charMin].width);
        //Se dibuja el carácter seleccionado
        drawPictureChar();
    }
}

void MainWindow::on_horizontalSliderCharMin_valueChanged(int value)
{
    //Ha cambiado el carácter inicial de la fuente

    //Tomamos nota del nuevo valor
    byte newCharMin = (byte)value;
    //Tomamos nota del antiguo valor (el actual)
    byte oldCharMin = font->charMin;
    //Siempre actualizamos el cuadro que muestra el índice
    ui->labelCharMinCode->setText(QString::number(newCharMin));
    ui->labelCharMinASCII->setText(QChar(newCharMin));
    //Siempre charMin<=charMax
    if (newCharMin > ui->horizontalSliderCharMax->value())
    {
        //Actualizamos el valor de la barra de charMax
        ui->horizontalSliderCharMax->setValue(newCharMin);
    }
    //Siempre charMin > carácter seleccionado
    if (newCharMin > ui->horizontalSliderChar->value()) ui->horizontalSliderChar->setValue(newCharMin);
    //Si ha cambiado el valor del primer carácter...
    if (newCharMin != oldCharMin)
    {
        //Redimensionamos la fuente cambiando el primer carácter
        font->redimCharMin(newCharMin);
        //La fuente se ha modificado
        fontModified();
        //Cambiamos el límite del selector de caracteres
        ui->horizontalSliderChar->setMinimum(newCharMin);
        //Asignamos la anchura correcta
        ui->spinBoxChar->setValue(font->dataChar[ui->horizontalSliderChar->value() - font->charMin].width);
        //Se dibuja el carácter seleccionado
        drawPictureChar();
    }
}

void MainWindow::on_horizontalSliderChar_valueChanged(int value)
{
    //Se ha cambiado el carácter a editar

    //Actualizamos el código ASCII del carácter
    ui->labelCharCode->setText(QString::number(value));
    //Actualizamos el carácter
    ui->labelCharASCII->setText(QChar(value));
    //Actualizamos la anchura correcta
    ui->spinBoxChar->setValue(font->dataChar[value-font->charMin].width);
    //Redibujamos el carácter actual
    drawPictureChar();
}

void MainWindow::on_spinBoxPages_valueChanged(int arg1)
{
    //Ha cambiado el valor del número de páginas de la fuente

    //Nuevo número de páginas
    byte newPages = (byte)arg1;
    //Si el número de páginas ha cambiado...
    if (font->pages != newPages)
    {
        //Redimensionamos todos los caracteres de la fuente al nuevo número de páginas
        font->redimPages(newPages);
        //Asignamos el nuevo número de páginas a la fuente
        font->pages = newPages;
        //Redibujamos el carácter actual
        drawPictureChar();
        //La fuente se ha modificado
        fontModified();
    }
}

void MainWindow::mousePressSlot(QPoint &xy, int &button)
{
    //Se ha recibido una signal de que se ha pulsado el botón del mouse

    //Si el botón es uno de los que nos interesa...
    //Qt::LeftButton = 1
    //Qt::RightButton = 2
    //Qt::MidButton = 4
    if(button<3) //Botón 1 o botón 2
    {
        //Anotamos cuál será el último color:
        //button==1 -> lastColor=true : pinta de negro
        //button==2 -> lastColor=false : pinta de blanco
        lastColor=button==1;
        //Llamamos al slot de movimiento que es el que verifica
        //rangos, edita y refresca la imágen
        mouseMoveSlot(xy);
    }
}

void MainWindow::mouseMoveSlot(QPoint &xy)
{
    //Se ha recibido una signal de que se ha movido el mouse

    //Teniendo en cuenta el zoom actual, calculamos las coordenadas
    //del carácter
    int x=xy.x()/zoom;
    int y=xy.y()/zoom;
    //Si las coordenadas están dentro de los límites del carácter...
    if(x<ui->spinBoxChar->value() && y<ui->spinBoxPages->value()*8)
    {
        //Calculamos la página
        byte page=y/8;
        //Calculamos la fila de la página
        byte dy=y % 8;
        //Calculamos el byte gráfico afectado
        int dataIndex=x+page*ui->spinBoxChar->value();
        //Calculamos el color actual del pixel
        bool currentColor = (font->dataChar[ui->horizontalSliderChar->value() - font->charMin].data[dataIndex] & (byte)(1 << dy)) > 0;
        //Si el color actual del pixel no coincide con el último color seleccionado...
        if(currentColor != lastColor)
        {
            //Cambiamos el estado del pixel en la fuente
            font->dataChar[ui->horizontalSliderChar->value() - font->charMin].data[dataIndex] ^= (byte)(1 << dy);
            //Redibujamos el carácter actual
            drawPictureChar();
            //La fuente se ha modificado
            fontModified();
        }
    }
}

QString MainWindow::openFileFON(DataFont *newFont, QString fileName)
{
    //Abre un archivo .fon
    //Devuelve el mensaje de error (si existe)

    //Mensaje de error. Inicialmente ninguno
    QString errorMessage = "";
    //Creamos un objeto QFile
    QFile f(fileName);
    //Abrimos el archivo para sólo escritura
    //Si no se consigue...tenemos un error
    if(!f.open(QIODevice::ReadOnly)) errorMessage="No se puede abrir el archivo.";
    else //Se ha podido abrir el archivo
    {
        try
        {
            //Leemos el código del primer carácter de la fuente
            f.read((char*)&(newFont->charMin),1);
            //Leemos el código del último carácter de la fuente
            f.read((char*)&(newFont->charMax),1);
            //Leemos el número de páginas de altura
            f.read((char*)&(newFont->pages),1);
            //Calculamos el número de caracteres de la fuente
            byte charCount = newFont->charCount();
            //Creamos el array de caracteres
            newFont->dataChar = new DataChar[charCount];
            //Recorremos todos los caracteres
            for (byte c = 0; c < charCount; c++)
            {
                //Posicionamos la lectura del archivo en la anchura del carácter
                f.seek(3 + charCount * 2 + c);
                //Leemos la anchura
                f.read((char*)&(newFont->dataChar[c].width),1);
                //Creamos el array de datos del carácter
                newFont->dataChar[c].data = new byte[newFont->dataChar[c].width * newFont->pages];
                //Posicionamos la lectura del archivo en el índice de los datos del carácter
                f.seek(3 + 2 * c);
                //Leemos la posición de inicio de datos del carácter
                quint16 charIndex;
                f.read((char*)&charIndex,2);
                //Posicionamos la lectura del archivo al inicio de datos del carácter
                f.seek(5 + charCount * 3 + charIndex);
                //Leemos los datos del carácter al array
                f.read((char*)newFont->dataChar[c].data,newFont->dataChar[c].width * newFont->pages);
            }
        }
        catch (...) //Si ocurre cualquier excepción...
        {
            //Algo ha salido mal mientras procesábamos su contenido
            //Anotamos el mensaje de error
            errorMessage = "Error procesando el archivo de fuentes";
        }
        //Cerramos el archivo
        f.close();
    }
    //Hemos terminado de leer el archivo
    //Devolvemos el mensaje de error
    return errorMessage;
}

void MainWindow::on_pushButtonOpen_clicked()
{
    //Pulsado el botón Open

    //Si la fuente cargada actualmente ha sido modificada...
    if (modified)
    {
        //Creamos variable para recoger el resultado de una ventana
        QMessageBox::StandardButton answer;
        //Mostramos una ventana de pregunta con botones estándar: Yes & No
        //Recogemos su respuesta
        answer=QMessageBox::question(this,"Fuente modificada","La fuente actual tiene cambios no guardados.\nDesea continuar?");
        //Si no ha respondido afirmativamente...hemos terminado
        if(answer!=QMessageBox::Yes) return;
    }
    //Calculamos el path actual del ejecutable
    QDir dir;
    QString currentPath=dir.absolutePath();
    //Pedimos que se seleccione un único archivo
    QString fileName = QFileDialog::getOpenFileName(this,tr("Selecciona archivo de fuente"),currentPath,tr("Fuentes (*.fon)"));
    //Si se cancela el formulario de selección de archivos...hemos terminado
    if(fileName.isNull()) return;
    //Si el archivo es .fon...
    if(fileName.endsWith(".fon"))
    {
        //Creamos una nueva clase para cargar la fuente
        DataFont *newFont= new DataFont();
        //Intentamos leer el archivo
        QString errorMessage = openFileFON(newFont,fileName);
        //Si se ha leido correctamente....
        if (errorMessage.length()==0)
        {
            //Todo correcto, podemos sustituir la fuente actual
            //Borramos la fuente actual
            font->clear();
            delete font;
            //Asignamos la nueva fuente al puntero
            font=newFont;
            //Por ahora no se ha modificado
            modified = false;
            ui->pushButtonSave->setEnabled(false);
            //Aplicamos los límites del rango de caracteres de la fuente
            ui->horizontalSliderChar->setMinimum(font->charMin);
            ui->horizontalSliderChar->setMaximum(font->charMax);
            //Y a los controles de configuración
            ui->horizontalSliderCharMin->setValue(font->charMin);
            ui->horizontalSliderCharMax->setValue(font->charMax);
            //Aplicamos el número de páginas al control de configuración
            ui->spinBoxPages->setValue(font->pages);
            //Inicialmente comenzaremos editando el primer carácter del rango
            ui->horizontalSliderChar->setValue(font->charMin);
            //Asignamos la anchura correcta
            ui->spinBoxChar->setValue(font->dataChar[0].width);
            //Se dibuja el carácter seleccionado
            drawPictureChar();
            //Anotamos el nombre de archivo como último cargado
            loadFileName=fileName;
        }
        //Error al leer archivo...mostramos mensaje
        else QMessageBox::critical(this,"Error",errorMessage);
    }
    else //El archivo no tiene extensión .fon
    {
        //No reconocemos la extensión del archivo de entrada
        QMessageBox::information(this,"Info","El archivo seleccionado no tiene extensión .fon");
    }

}

void MainWindow::on_pushButtonSave_clicked()
{
    //Se ha pulsado el botón de salvar fuente

    //Creamos un objeto QFile con el mismo nombre que el último archivo cargado
    QFile f(loadFileName);
    //Abrimos el archivo para sólo escritura
    //Si no se consigue...
    if(!f.open(QIODevice::WriteOnly))
    {
        //...mostramos el error
        QMessageBox::critical(this,"Error","Error al abrir el archivo para escritura.");
        //Hemos terminado
        return;
    }
    //Hemos podido abrir el archivo para escritura
    //Escribimos el código del primer carácter de la fuente
    f.write((char*)&(font->charMin),1);
    //Escribimos el código del último carácter de la fuente
    f.write((char*)&(font->charMax),1);
    //Escribimos el número de páginas de altura
    f.write((char*)&(font->pages),1);
    //Calculamos el número de caracteres de la fuente
    byte charCount = (byte)(font->charMax - font->charMin + 1);
    //Definimos variable que guardará la posición de los datos gráficos
    quint16 dataChar = 0;
    //Definimos variable con el sumatorio de anchuras
    quint16 totalWidth = 0;
    //Escribimos la tabla de direcciones
    for (byte c = 0; c < charCount; c++)
    {
        //Anchura del carácter
        byte width = font->dataChar[c].width;
        //La añadimos al sumatorio de anchuras
        totalWidth += width;
        //Escribimos la dirección de datos
        f.write((char*)&dataChar,2);
        //Incrementamos la dirección en tantos bytes como contenga el carácter
        dataChar += (quint16)(width * font->pages);
    }
    //Escribimos la tabla de anchuras
    for (byte c = 0; c < charCount; c++)
    {
        f.write((char*)&(font->dataChar[c].width),1);
    }
    //El tamaño de los datos gráficos es el sumatorio de anchuras por el
    //número de páginas
    totalWidth *= font->pages;
    //Escribimos el tamaño de la tabla de datos gráficos
    f.write((char*)&totalWidth,2);
    //Escribimos los datos gráficos de cada carácter
    for (byte c = 0; c < charCount; c++)
    {
        //Guardamos todos sus datos gráficos
        //El total de bytes es el producto de la anchura por las páginas
        f.write((char*)font->dataChar[c].data,font->dataChar[c].width*font->pages);
    }
    //Cerramos el archivo
    f.close();
    //El archivo se ha guardado correctamente
    //Ya no hay modificaciones
    modified = false;
    //Desactivamos el botón de guardar
    ui->pushButtonSave->setEnabled(false);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //Se ha solicitado cerrar la ventana

    //Si la fuente actual se ha modificado...
    if(modified)
    {
        //Creamos variable para recoger el resultado de una ventana
        QMessageBox::StandardButton answer;
        //Mostramos una ventana de pregunta con botones estándar: Yes & No
        //Recogemos su respuesta
        answer=QMessageBox::question(this,"Fuente modificada","La fuente actual tiene cambios no guardados.\nDesea continuar?");
        //Si se ha respondido afirmativamente...aceptamos el evento
        if(answer==QMessageBox::Yes) event->accept();
        //Sino...lo ignoramos
        else event->ignore();
    }
}
