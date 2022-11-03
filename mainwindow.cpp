#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMouseEvent>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene();
    ui->graphicsView->setMouseTracking(true);
    //ui->graphicsView->setSceneRect(ui->graphicsView->rect());
    ui->graphicsView->setScene(scene);
    gray = QColor(127, 127, 127, 255);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void  MainWindow::mousePressEvent(QMouseEvent* event)
{
    // проверка попадания клика в существующую точку
    // i - индекс точки в pointList
    // при i = -1 осуществляется нанесение точки на график
    // при i != -1 точка удаляется
    QPoint remapped = ui->graphicsView->mapFromParent(event->pos());
    if(ui->graphicsView->rect().contains(remapped))
    {
        QPointF mousePoint = ui->graphicsView->mapToScene(remapped);
        long long int i = -1;
        if(pointList.size() > 0)
        {
            for(int iter = 0; iter < pointList.size(); iter++)
            {
                if(abs(mousePoint.x() - pointList.at(iter).x()) < delta
                        && abs(mousePoint.y() - pointList.at(iter).y()) < delta)
                {
                    i = iter;
                }
            }
        }
        if(i != -1)
        {
            drawSinglePoint(pointList.at(i), QColor(255, 255, 255, 255));
            pointList.remove(i);
        }
        else
        {
            pointList.append(mousePoint);
            drawSinglePoint(mousePoint, gray);
        }
    }
}

void MainWindow::on_loadButton_clicked()
{
    QString loadFileName = QFileDialog::getOpenFileName(
                nullptr,
                QObject::tr("Save Document"),
                QDir::currentPath(),
                QObject::tr("Text file (*.txt)"));
    QFile loadFile(loadFileName);
    if(loadFile.open(QIODevice::ReadOnly))
    {
        clearData();
        QTextStream load(&loadFile);
        QString line;
        while(!load.atEnd())
        {
            line = load.readLine();
            pointList.append(lineToPoint(line));
        }
        loadFile.close();
        drawPoints(gray);
        // resize?
    }
    else
    {
        QMessageBox::information(nullptr, "Error", "Файл не был выбран");
    }
}

void MainWindow::on_saveButton_pressed()
{
    QString saveFileName = QFileDialog::getSaveFileName(
                nullptr,
                QObject::tr("Open Document"),
                QDir::currentPath(),
                QObject::tr("Text files (*.txt)"));

    QFile saveFile(saveFileName);
    if(saveFile.open(QIODevice::ReadWrite))
    {
        QTextStream save(&saveFile);
        for(auto iter : pointList)
        {
            save << pointToLine(iter);
            save << "\n";
        }
        saveFile.close();
    }
    else
    {
        QMessageBox::information(nullptr, "Error", "Файл не был создан");
    }
}


void MainWindow::on_calculationButton_pressed()
{

}


void MainWindow::on_pushButton_pressed()
{

}

void MainWindow::drawSinglePoint(QPointF point, QColor color)
{
    scene->addEllipse(
               point.x() - radius,
               point.y() - radius,
               radius,
               radius,
               QPen(color),
               QBrush(color));
}

void MainWindow::drawPoints(QColor color)
{
    for(auto iter : pointList)
    {
        scene->addEllipse(
                   iter.x() - radius,
                   iter.y() - radius,
                   radius,
                   radius,
                   QPen(color),
                   QBrush(color));
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

QString MainWindow::pointToLine(QPointF point)
{
    QString line;
    line.append('(');
    line.append(QString::number(point.x()));
    line.append(", ");
    line.append(QString::number(point.y()));
    line.append(')');
    return line;
}

void MainWindow::on_clearButton_pressed()
{
    clearData();
}

void MainWindow::clearData()
{
    scene->clear();
    while(!pointList.isEmpty())
    {
        pointList.clear();
    }
}
