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
    void circleplotpoint(int,int,int,int,int,int,int);


    void on_bresenham_clicked();
    void drawbresenham(int ,int ,int ,int,int,int,int);

    void on_polygon_clicked();

    void on_transform_clicked();

    void on_rotation_clicked();

    void on_set_pivot_clicked();

    void on_reflection_clicked();

    void on_scale_clicked();

    void on_minwin_clicked();

    void on_maxwin_clicked();

    void on_draw_window_clicked();

    void on_clipline_clicked();

    void on_clear_polygon_clicked();

    void on_clip_pol_clicked();

    void on_beizer_clicked();

    void on_scan_fill_clicked();
    void ScanlineFill();
    void drawPolyDino();


    void on_boundary_fill_clicked();
    void boundary_fill_util(int,int,QRgb,int, int,int);

    void on_flood_fill_clicked();
    void flood_fill_util(int,int,QRgb,int, int,int);

    void on_bresenham_circle_clicked();

    void on_draw_ellipse_clicked();
    void ellipseplotpoint(int ,int,int,int,int,int,int);
    void on_ellipse_center_clicked();

    void on_shear_clicked();

private:
    Ui::MainWindow *ui;
    QPoint p1,p2,p3,p4,wmin,wmax;
    void point(int,int);
};

#endif // MAINWINDOW_H
