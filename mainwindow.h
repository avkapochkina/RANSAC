#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QtNetwork/QUdpSocket>

extern int threads;

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
    // загрузка файла
    void on_loadButton_clicked();

    // сохранение файла
    void on_saveButton_pressed();

    // запуск алгоритма RANSAC
    void on_calculationButton_pressed();

    // отправка данных
    void on_pushButton_pressed();

    // очистка данных
    void on_clearButton_pressed();

    void on_isParallelBox_toggled(bool checked);

    void on_threadBox_valueChanged(int arg1);

private:

    Ui::MainWindow *ui;

    QGraphicsScene *scene;

    // все точки, находящиеся на графике
    QVector<QPointF> pointVector;

    // inliner точки
    QVector<QPointF> bestInlinerVector;

    // перевод строки файла в QPointF
    QPointF lineToPoint(QString line);

    // перевод QPointF в строку для записи в файл
    QString pointToLine(QPointF point);

    // нанесение точки point на график
    void drawSinglePoint(QPointF point, QColor color);

    // нанесение на график всех точек, хранящихся в vector
    void drawPoints(QVector<QPointF>* vector, QColor color);

    // обработка нажатия кнопки мыши
    void mousePressEvent(QMouseEvent* event) override;

    // очистка графика и pointVector
    void clearData();

    // получаение строки с уравнением прямой вида
    // ax + bx + c = 0
    QString lineEquation();

    // радиус точки на графике
    double radius = 4.0;

    // радиус области вокруг точки,
    // в которой засчитывается клик при удалении
    const double delta = 5.0;

    // коэффициенты уравнения прямой
    double lineA = 0, lineB = 0, lineC = 0;

    QUdpSocket *out;

    QColor gray;
};
#endif // MAINWINDOW_H
