#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMouseEvent>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void  MainWindow::mousePressedEvent(QMouseEvent* event)
{
    // doesnt work lol
    QPoint remapped = ui->graphicsView->mapFromParent(event->pos());
    if(ui->graphicsView->rect().contains(remapped))
    {
        QPointF mousePoint = ui->graphicsView->mapToScene(remapped);
        QString testLabelText;
        testLabelText.append(QString::number(mousePoint.rx()));
        testLabelText.append(QString::number(mousePoint.ry()));
        ui->testLabel->setText(testLabelText);
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
        ui->testLabel->setText("loadFile success");
    }
    else
    {
        ui->testLabel->setText("loadFile fail");
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

