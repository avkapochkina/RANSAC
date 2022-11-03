#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_loadButton_clicked();

    void on_saveButton_pressed();

    void on_calculationButton_pressed();

    void on_pushButton_pressed();

private:

    Ui::MainWindow *ui;

    QGraphicsScene *scene;

    QList<QPointF> pointList;

    QPointF lineToPoint(QString line);

    void drawSinglePoint(QPointF point);

    void drawPoints();

    void mousePressEvent(QMouseEvent* event) override;

    double radius = 1.0;
};
#endif // MAINWINDOW_H
