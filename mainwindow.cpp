#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "include/GRANSAC.hpp"
#include "include/LineModel.hpp"
#include <QMouseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QElapsedTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    scene = new QGraphicsScene();
    baseRect = ui->graphicsView->rect();
    ui->graphicsView->setMouseTracking(true);
    //ui->graphicsView->setSceneRect(baseRect);
    ui->graphicsView->setScene(scene);

    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    gray = QColor(127, 127, 127, 255);

    ui->timerLabel->setVisible(false);

    ui->lineTextLabel->setVisible(false);
    ui->lineLabel->setVisible(false);

    ui->threadBox->setEnabled(false);
    ui->threadBox->setMinimum(1);
    ui->threadBox->setMaximum(omp_get_max_threads());

    out = new QUdpSocket(this);
    out->connectToHost(QHostAddress::LocalHost, 50081);

    ui->thresholdEdit->setValidator( new QIntValidator(0, 100, this) );
    ui->thresholdEdit->setText("20");

    ui->iterationsEdit->setValidator( new QIntValidator(0, 10000, this) );
    ui->iterationsEdit->setText("100");
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
    int threshold, iterations;
    if(pointVector.size() == 0)
    {
        QMessageBox::information(nullptr, "Error", "Загрузите файл или отметьте точки на графике");
        return;
    }
    if(ui->thresholdEdit->text() == "" && ui->iterationsEdit->text() == "")
    {
        QMessageBox::information(nullptr, "Error", "Введите пороговое значение и количество итераций");
        return;
    }

    while(!bestInlinerVector.isEmpty())
    {
        bestInlinerVector.clear();
    }
    lineA = 0;

    threshold = ui->thresholdEdit->text().toInt();
    iterations = ui->thresholdEdit->text().toInt();
    scene->clear();

    QElapsedTimer  timer;
    timer.start();
    ui->timerLabel->setVisible(true);

    if(ui->isParallelBox->isChecked())
    {
        threads = ui->threadBox->value();
    }
    else
    {
        threads = 1;
    }

    std::vector<std::shared_ptr<GRANSAC::AbstractParameter>> points;
    for (int i = 0; i < pointVector.size(); ++i)
    {
        std::shared_ptr<GRANSAC::AbstractParameter> pt = std::make_shared<Point2D>(pointVector.at(i).x(), pointVector.at(i).y());
        points.push_back(pt);
    }
    GRANSAC::RANSAC<Line2DModel, 2> estimator;
    estimator.Initialize(threshold, iterations);
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

    // таймер
    ui->lineTextLabel->setVisible(true);
    ui->lineLabel->setVisible(true);
    ui->lineLabel->setText("");
    ui->timerLabel->setText("Время выполнения: "
                            + QString::number(timer.elapsed())
                            + " ms");

    drawPoints(&pointVector, QColor(255, 0, 0, 255));
    drawPoints(&bestInlinerVector, QColor(0, 0, 255, 255));
    auto BestLine = estimator.GetBestModel();
    if (BestLine)
    {
        auto BestLinePt1 = std::dynamic_pointer_cast<Point2D>(BestLine->GetModelParams()[0]);
        auto BestLinePt2 = std::dynamic_pointer_cast<Point2D>(BestLine->GetModelParams()[1]);
        if (BestLinePt1 && BestLinePt2)
        {
            QPointF ptMin(bestInlinerVector.at(0).x(), bestInlinerVector.at(0).y()); // min x
            QPointF ptMax(bestInlinerVector.at(0).x(), bestInlinerVector.at(0).y()); // max x
            for(int iter = 0; iter < bestInlinerVector.size(); iter++)
            {
                if(bestInlinerVector.at(iter).x() < ptMin.x())
                    ptMin = bestInlinerVector.at(iter);
                if(bestInlinerVector.at(iter).x() > ptMax.x())
                    ptMax = bestInlinerVector.at(iter);
            }
            lineA = ptMax.y() - ptMin.y();
            lineB = ptMin.x() - ptMax.x();
            lineC = ptMin.x() * ptMin.y()
                  - ptMin.x() * ptMax.y()
                  - ptMin.y() * ptMin.x()
                  + ptMin.y() * ptMax.x();
            scene->addLine(ptMin.x(),
                           ptMin.y(),
                           ptMax.x(),
                           ptMax.y(),
                           QPen(QColor(0, 0, 255, 255)));
        }
    }

    ui->lineLabel->setText(lineEquation());
}

void MainWindow::on_pushButton_pressed()
{
    out->write(lineEquation().toUtf8().data());
}

QString MainWindow::lineEquation()
{
    QString lineString;
    lineString.append(QString::number(lineA));
    lineString.append("*x + ");
    lineString.append(QString::number(lineB));
    lineString.append("*y + ");
    lineString.append(QString::number(lineC));
    lineString.append(" = 0");
    return lineString;
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
    //ui->graphicsView->fitInView(baseRect);
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

    ui->graphicsView->update();

    ui->timerLabel->setVisible(false);
    ui->lineTextLabel->setVisible(false);
    ui->lineLabel->setVisible(false);

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

void MainWindow::on_isParallelBox_toggled(bool checked)
{
    ui->threadBox->setEnabled(checked);
}

void MainWindow::on_threadBox_valueChanged(int arg1)
{
}
