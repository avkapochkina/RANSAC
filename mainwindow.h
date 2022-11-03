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

private:

    Ui::MainWindow *ui;

    QGraphicsScene *scene;

    // все точки, находящиеся на графике
    QList<QPointF> pointList;

    // перевод строки файла в QPointF
    QPointF lineToPoint(QString line);

    // перевод QPointF в строку для записи в файл
    QString pointToLine(QPointF point);

    // нанесение точки point на график
    void drawSinglePoint(QPointF point, QColor color);

    // нанесение на график всех точек, хранящихся в pointList
    void drawPoints(QColor color);

    // обработка нажатия кнопки мыши
    void mousePressEvent(QMouseEvent* event) override;

    // очистка графика и pointList
    void clearData();

    // радиус точки на графике
    double radius = 4.0;

    // радиус области вокруг точки,
    // в которой засчитывается клик при удалении
    const double delta = 5.0;

    QColor gray;
};
#endif // MAINWINDOW_H
