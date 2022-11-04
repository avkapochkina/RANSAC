#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "include/GRANSAC.hpp"
#include "include/LineModel.hpp"
#include <QMouseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    scene = new QGraphicsScene();
    ui->graphicsView->setMouseTracking(true);
    ui->graphicsView->setSceneRect(ui->graphicsView->rect());
    ui->graphicsView->setScene(scene);
    gray = QColor(127, 127, 127, 255);

    ui->timerLabel->setVisible(false);
    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, SLOT(updateTimer()));

    ui->lineTextLabel->setVisible(false);
    ui->lineLabel->setVisible(false);

    out = new QUdpSocket(this);
    out->connectToHost(QHostAddress::LocalHost, 50081);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void  MainWindow::mousePressEvent(QMouseEvent* event)
{
    // проверка попадания клика в существующую точку
    // i - индекс точки в pointVector
    // при i = -1 осуществляется нанесение точки на график
    // при i != -1 точка удаляется
    QPoint remapped = ui->graphicsView->mapFromParent(event->pos());
    if(ui->graphicsView->rect().contains(remapped))
    {
        ui->timerLabel->setVisible(false);
        QPointF mousePoint = ui->graphicsView->mapToScene(remapped);
        long long int i = -1;
        if(pointVector.size() > 0)
        {
            for(int iter = 0; iter < pointVector.size(); iter++)
            {
                if(abs(mousePoint.x() - pointVector.at(iter).x()) < delta
                        && abs(mousePoint.y() - pointVector.at(iter).y()) < delta)
                {
                    i = iter;
                }
            }
        }
        if(i != -1)
        {
            drawSinglePoint(pointVector.at(i), QColor(255, 255, 255, 255));
            pointVector.remove(i);
        }
        else
        {
            pointVector.append(mousePoint);
            drawSinglePoint(mousePoint, gray);
        }
    }
}

void MainWindow::on_loadButton_clicked()
{
    ui->timerLabel->setVisible(false);
    QString loadFileName = QFileDialog::getOpenFileName(
                nullptr,
                QObject::tr("Save Document"),
                QDir::currentPath(),
                QObject::tr("Text file (*.txt)"));
    QFile loadFile(loadFileName);
    if(!loadFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(nullptr, "Error", "Файл не был выбран");
        return;
    }
    clearData();
    QTextStream load(&loadFile);
    QString line;
    while(!load.atEnd())
    {
        line = load.readLine();
        pointVector.append(lineToPoint(line));
    }
    loadFile.close();
    drawPoints(&pointVector, gray);
    // resize?
}

void MainWindow::on_saveButton_pressed()
{
    ui->timerLabel->setVisible(false);
    QString saveFileName = QFileDialog::getSaveFileName(
                nullptr,
                QObject::tr("Open Document"),
                QDir::currentPath(),
                QObject::tr("Text files (*.txt)"));

    QFile saveFile(saveFileName);
    if(!saveFile.open(QIODevice::ReadWrite))
    {
        QMessageBox::information(nullptr, "Error", "Файл не был создан");
        return;
    }
    QTextStream save(&saveFile);
    for(auto iter : pointVector)
    {
        save << pointToLine(iter);
        save << "\n";
    }
    saveFile.close();
}


void MainWindow::on_calculationButton_pressed()
{
    if(pointVector.size() == 0)
    {
        QMessageBox::information(nullptr, "Error", "Загрузите файл или отметьте точки на графике");
        return;
    }
    scene->clear();

    timerMS = 0;
    timer->start(1);
    ui->timerLabel->setVisible(true);

    // parallel
    if(ui->isParallelBox->isChecked())
    {
    }
    else
    {
    }

    int Perturb = 25;
    std::normal_distribution<GRANSAC::VPFloat> PerturbDist(0, Perturb);
    std::vector<std::shared_ptr<GRANSAC::AbstractParameter>> points;
    for (int i = 0; i < pointVector.size(); ++i)
    {
        std::shared_ptr<GRANSAC::AbstractParameter> pt = std::make_shared<Point2D>(pointVector.at(i).x(), pointVector.at(i).y());
        points.push_back(pt);
    }
    GRANSAC::RANSAC<Line2DModel, 2> estimator;
    estimator.Initialize(20, 100); // Threshold, iterations
    estimator.Estimate(points);

    auto bestInliers = estimator.GetBestInliers();
    if (bestInliers.size() > 0)
    {
        for (auto& Inlier : bestInliers)
        {
            auto RPt = std::dynamic_pointer_cast<Point2D>(Inlier);
            QPointF point;
            point.setX(RPt->m_Point2D[0]);
            point.setY(RPt->m_Point2D[1]);
            bestInlinerVector.append(point);
        }
    }

    drawPoints(&pointVector, QColor(255, 0, 0, 255));
    drawPoints(&bestInlinerVector, QColor(0, 0, 255, 255));
    auto BestLine = estimator.GetBestModel();
    if (BestLine)
    {
        auto BestLinePt1 = std::dynamic_pointer_cast<Point2D>(BestLine->GetModelParams()[0]);
        auto BestLinePt2 = std::dynamic_pointer_cast<Point2D>(BestLine->GetModelParams()[1]);
        if (BestLinePt1 && BestLinePt2)
        {
            QPointF pt1(BestLinePt1->m_Point2D[0], BestLinePt1->m_Point2D[1]);
            QPointF pt2(BestLinePt2->m_Point2D[0], BestLinePt2->m_Point2D[1]);
            scene->addLine(pt1.x(),
                           pt1.y(),
                           pt2.x(),
                           pt2.y(),
                           QPen(QColor(0, 0, 255, 255)));
            lineA = pt2.y() - pt1.y();
            lineB = pt1.x() - pt2.x();
            lineC = pt1.x() * pt1.x() - pt1.x() * pt2.y() - pt1.y() * pt1.x() + pt1.y() * pt2.x();
        }
    }

    ui->lineTextLabel->setVisible(true);
    ui->lineLabel->setVisible(true);
    ui->lineLabel->setText("");
    QString lineString;
    lineString.append(QString::number(lineA));
    lineString.append("*x + ");
    lineString.append(QString::number(lineB));
    lineString.append("*y + ");
    lineString.append(QString::number(lineC));
    lineString.append(" = 0");
    ui->lineLabel->setText(lineString);

    timer->stop();
    out->write(lineString.toUtf8());
}

void MainWindow::updateTimer()
{
    timerMS++;
    QString timerText = QString::number(timerMS);
    ui->timerLabel->setText("Время выполнения: " + timerText);
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

void MainWindow::drawPoints(QVector<QPointF> *vector, QColor color)
{
    for(int iter = 0; iter < vector->size(); iter++)
    {
        scene->addEllipse(
                   vector->at(iter).x() - radius,
                   vector->at(iter).y() - radius,
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

    ui->timerLabel->setVisible(false);
    ui->lineTextLabel->setVisible(false);
    ui->lineLabel->setVisible(false);

    if(timer->isActive())
        timer->stop();
    while(!pointVector.isEmpty())
    {
        pointVector.clear();
    }

    while(!bestInlinerVector.isEmpty())
    {
        bestInlinerVector.clear();
    }

    lineA = 0;
    lineB = 0;
    lineC = 0;
}
