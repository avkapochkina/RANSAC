#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMouseEvent>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    scene = new QGraphicsScene();
    ui->graphicsView->setMouseTracking(true);
    //ui->graphicsView->setSceneRect(ui->graphicsView->rect());
    ui->graphicsView->setScene(scene);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void  MainWindow::mousePressEvent(QMouseEvent* event)
{
    QPoint remapped = ui->graphicsView->mapFromParent(event->pos());
    if(ui->graphicsView->rect().contains(remapped))
    {
        QPointF mousePoint = ui->graphicsView->mapToScene(remapped);
        if(pointList.contains(mousePoint))
        {
            //удаление
        }
        else
        {
            drawSinglePoint(mousePoint);
        }
    }
    else
    {
        ui->testLabel->setText("miss");
    }
}

void MainWindow::on_loadButton_clicked()
{
    QString loadFileName = QFileDialog::getOpenFileName(
                nullptr,
                QObject::tr("Open Document"),
                QDir::currentPath(),
                QObject::tr("Text files (*.txt)"));
    QFile loadFile(loadFileName);
    if(loadFile.open(QIODevice::ReadOnly))
    {
        // вытащи в отдельную функцию
        QTextStream load(&loadFile);
        QString line;
        while(!load.atEnd())
        {
            // тут зациклилось
            line = load.readLine();
            line.remove('(');
            line.remove(')');
            line.remove(' ');
            QStringList coordinates = line.split(",");
            QPointF point;
            point.setX(coordinates.begin()->toDouble());
            point.setY(coordinates.end()->toDouble());
            pointList.append(&point);
        }
        loadFile.close();
        drawPoints();
    }
    else
    {
        ui->testLabel->setText("loadFile fail");
        //QMessageBox::information
    }
}


void MainWindow::on_saveButton_pressed()
{
    QString saveFileName = QFileDialog::getOpenFileName(
                nullptr,
                QObject::tr("Open Document"),
                QDir::currentPath(),
                QObject::tr("Text files (*.txt)"));

    QFile saveFile(saveFileName);
    if(saveFile.open(QIODevice::WriteOnly))
    {
        ui->testLabel->setText("loadFile success");
        //
        saveFile.close();
    }
    else
    {
        ui->testLabel->setText("loadFile fail");
    }
}


void MainWindow::on_calculationButton_pressed()
{

}


void MainWindow::on_pushButton_pressed()
{

}

void MainWindow::drawSinglePoint(QPointF point)
{
   scene->addEllipse(point.x() - radius, point.y() - radius, radius * 2.0, radius * 2.0, QPen(), QBrush(Qt::SolidPattern));
}

void MainWindow::drawPoints()
{
    for(auto iter : pointList)
    {
        drawSinglePoint(*iter);
    }
}
