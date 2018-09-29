#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QtCore>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public slots:
    void Mouse_Pressed();
    void showMousePosition(QPoint& pos);
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_show_axes_clicked();

    void on_Draw_clicked();

    void on_set_point1_clicked();

    void on_set_point2_clicked();

    void on_pushButton_clicked();

    void on_set_grid_size_clicked();

    int getx(int);
    int gety(int);

    void on_DDA_clicked();

    void on_midp_center_clicked();

    void on_midpoint_circle_clicked();

    void mysetpixel(int,int,int,int,int);
    void circleplotpoint(int,int,int,int);


    void on_bresenham_clicked();
    void drawbresenham(int ,int ,int ,int);

    void on_polygon_clicked();

    void on_transform_clicked();

private:
    Ui::MainWindow *ui;
    QPoint p1,p2,p3;
    void point(int,int);
};

#endif // MAINWINDOW_H
