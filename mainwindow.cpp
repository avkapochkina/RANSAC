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
    ui->graphicsView->setSceneRect(ui->graphicsView->rect());
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
        long long int i = -1;
        // delta - радиус области вокруг точки, в которой засчитывается клик
        const double delta = 0.1;
        if(pointList.size() > 0)
        {
            for(int iter = 0; iter < pointList.size(); iter++)
            {
                const double pointListX = pointList.at(iter).x();
                const double pointListY = pointList.at(iter).y();

                const double dx = abs(mousePoint.x() - pointList.at(iter).x());
                const double dy = abs(mousePoint.y() - pointList.at(iter).y());

                if(dx < delta && dy < delta)
                {
                    i = iter;
                }
            }
        }
        if(i != -1)
        {
            pointList.remove(i);
        }
        else
        {
            pointList.append(mousePoint);
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
        while(!pointList.isEmpty())
        {
            pointList.clear();
        }

        QTextStream load(&loadFile);
        QString line; QPointF point;
        while(!load.atEnd())
        {
            line = load.readLine();
            point = lineToPoint(line);
            // append меняет весь вектор ???
            pointList.append(point);
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
   scene->addEllipse(
               point.x() - radius,
               point.y() - radius,
               radius * 2.0,
               radius * 2.0,
               QPen(),
               QBrush(Qt::SolidPattern));
}

void MainWindow::drawPoints()
{
    for(auto iter : pointList)
    {
        drawSinglePoint(iter);
    }
}

QPointF MainWindow::lineToPoint(QString line)
{
    line.remove('(');
    line.remove(')');
    line.remove(' ');
    QStringList coordinates = line.split(",");
    QPointF point;
    point.setX(coordinates.begin()->toDouble());
    point.setY(coordinates.back().toDouble());
    coordinates.clear();
    return point;
}
