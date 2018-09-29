#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QImage>
#include <iostream>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintDevice>
#include <QPoint>

QImage img;
int grsz=1;
int polygon[100][2];
int polygonindex=0;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->x_axis->hide();
    ui->y_axis->hide();
    img=QImage(ui->frame->width(),ui->frame->height(),QImage::Format_RGB888);
    connect(ui->frame,SIGNAL(Mouse_Pos()),this,SLOT(Mouse_Pressed()));
    connect(ui->frame,SIGNAL(sendMousePosition(QPoint&)),this,SLOT(showMousePosition(QPoint&)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::getx(int x){
    int temp=x-(ui->frame->width()/2);
    int neg=0;
    if(temp<0) neg=1;
    temp=abs(temp);
    temp+=(grsz/2);
    temp/=grsz;
    if(neg) temp=-temp;
    return temp;
}

int MainWindow::gety(int y){
    int height=ui->frame->height();
    int temp=y-(height/2);
    temp=-temp;
    int neg=0;
    if(temp<0) neg=1;
    temp=abs(temp);
    temp+=(grsz/2);
    temp/=grsz;
    if(neg) temp=-temp;
    return temp;
}

void MainWindow::point(int x,int y)
{

    /*int lowx=(x/grsz)*grsz;
    int highx=(x/grsz+1)*grsz;
    int lowy=(y/grsz)*grsz;
    int highy=(y/grsz+1)*grsz;
    int i,j;
    for(i=lowx;i<=highx;i++){
        for(j=lowy;j<=highy;j++){
            img.setPixel(i,j,qRgb(100,200,100));
        }
    }*/
    int i,j;
    int a=getx(x);
    int b=gety(y);
    for(i=0;i<img.width();i++){
        for(j=0;j<img.height();j++){
            if(getx(i)==a&&gety(j)==b)
                img.setPixel(i,j,qRgb(100,200,100));
        }
    }
    ui->frame->setPixmap(QPixmap::fromImage(img));
}


void MainWindow::showMousePosition(QPoint &pos)
{
    int x=pos.x();
    int y=pos.y();
    ui->mouse_movement->setText(" X : "
    +QString::number(x-ui->frame->width()/2)+"- "+QString::number(getx(x)) +
           ", Y : "+QString::number(-y+ui->frame->height()/2)+"- "+QString::number(gety(y)));
}
void MainWindow::Mouse_Pressed()
{
    /*int x=ui->frame->x-(ui->frame->width()/2);
    int y=ui->frame->y-(ui->frame->height()/2);
    y=-y;
    x/=grsz;
    y/=grsz;*/
    int x=ui->frame->x;
    int y=ui->frame->y;
    ui->mouse_pressed->setText(" X : "+QString::number(getx(x))+", Y : "+QString::number(gety(y)));
    point(ui->frame->x,ui->frame->y);
    ui->x_axis->move(0,ui->frame->y);
    ui->y_axis->move(ui->frame->x,0);
}

void MainWindow::on_show_axes_clicked()
{
    /*if(ui->show_axes->isChecked())
    {
        ui->x_axis->show();
        ui->y_axis->show();
    }
    else{
        ui->x_axis->hide();
        ui->y_axis->hide();
    }*/
    int i,j;
    for(i=1;i<=ui->frame->width();i++){
        for(j=1;j<=ui->frame->height();j++){
            if(getx(i)==0||gety(j)==0)
                img.setPixel(i,j,qRgb(120,140,160));
        }
    }
    ui->frame->setPixmap(QPixmap::fromImage(img));
}
void MainWindow::on_set_point1_clicked()
{
    if(ui->draw_line->isChecked()){
        p1.setX(ui->frame->x);
        p1.setY(ui->frame->y);
    }
}

void MainWindow::on_set_point2_clicked()
{
    if(ui->draw_line->isChecked()){
        p2.setX(ui->frame->x);
        p2.setY(ui->frame->y);
    }
}

void MainWindow::on_Draw_clicked()
{
    int r0=ui->circle_radius->value();
    QPainter painter(&img);
    QPen pen;
    pen.setWidth(1);
    pen.setColor(Qt::red);
    if(ui->draw_circle->isChecked()){
        p1.setX(ui->frame->x);
        p1.setY(ui->frame->y);
        painter.setPen(pen);
        painter.drawEllipse(p1,r0,r0);
    }
    if(ui->draw_line->isChecked()){
        painter.setPen(pen);
        painter.drawLine(p1,p2);
    }
    ui->frame->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::on_pushButton_clicked()
{
    for(int j=0;j<img.height();j++)
    {
        for(int i=0;i<img.width();i++)
        {
                img.setPixel(i,j,qRgb(0,0,0));
        }
    }
    ui->frame->setPixmap(QPixmap::fromImage(img));
    polygonindex=0;
}


void MainWindow::on_set_grid_size_clicked()
{
    grsz=ui->gridsize->value();
    std::cerr<<grsz;
    int i,j;
    for(i=0;i<img.width();i++){
        for(j=0;j<img.height();j++){
            if((i-img.width()/2+(grsz/2))%grsz==0||(-j+img.height()/2+(grsz/2))%grsz==0)
                img.setPixel(i,j,qRgb(25,155,250));
        }
    }
    ui->frame->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::on_DDA_clicked()
{
    if(!ui->draw_line->isChecked())
        return;
    int i,j;
    int x1=p1.x();
    int y1=p1.y();
    int x2=p2.x();
    int y2=p2.y();
    x1=getx(x1);
    x2=getx(x2);
    y1=gety(y1);
    y2=gety(y2);
    float m =(y2-y1)/(float)(x2-x1);
    //if(m<0) m=-m;
    int x=x1,y=y1;
    int minx,miny,maxx,maxy;
    qDebug()<<"m = "+QString::number(m);
    if(fabs(m)<=1){
         if(x1<=x2){
             minx=x1;
             miny=y1;
             maxx=x2;
             maxy=y2;
         }
         else{
             minx=x2;
             miny=y2;
             maxx=x1;
             maxy=y1;
         }
         x=minx;
         while(x<=maxx){
            y=miny+m*(x-minx);
            for(i=0;i<img.width();i++){
                for(j=0;j<img.height();j++)
                    if(x==getx(i)&&y==gety(j))
                        img.setPixel(i,j,qRgb(170,110,30));
            }
            x++;
        }
    }
    else{
        if(y1<=y2){
            minx=x1;
            miny=y1;
            maxx=x2;
            maxy=y2;
        }
        else{
            minx=x2;
            miny=y2;
            maxx=x1;
            maxy=y1;
        }
        y=miny;
        float m1=(x2-x1)/(float)(y2-y1);

        //qDebug()<<QString::number(minx)+" "+QString::number(miny)+" "+QString::number(maxx)+" "+QString::number(maxy)+" ";
        while(y<=maxy){
            x=minx+(y-miny)*m1;
            for(i=0;i<img.width();i++){
                for(j=0;j<img.height();j++)
                    if(x==getx(i)&&y==gety(j))
                        img.setPixel(i,j,qRgb(110,100,110));
            }
            y++;
        }

    }

    ui->frame->setPixmap(QPixmap::fromImage(img));

}

void MainWindow::on_midp_center_clicked()
{
    p3.setX(ui->frame->x);
    p3.setY(ui->frame->y);

}

void MainWindow::mysetpixel(int hor, int ver, int a=150, int b=150, int c=150){
    int i,j;
    for(i=0;i<img.width();i++){
        for(j=0;j<img.height();j++)
            if(hor==getx(i)&&ver==gety(j))
                img.setPixel(i,j,qRgb(a,b,c));
    }
    ui->frame->setPixmap(QPixmap::fromImage(img));

}

void MainWindow::circleplotpoint(int xcenter,int ycenter,int x,int y){
    mysetpixel(xcenter+x,ycenter+y);
    mysetpixel(xcenter-x,ycenter+y);
    mysetpixel(xcenter+x,ycenter-y);
    mysetpixel(xcenter-x,ycenter-y);

    mysetpixel(xcenter+y,ycenter+x);
    mysetpixel(xcenter-y,ycenter+x);
    mysetpixel(xcenter+y,ycenter-x);
    mysetpixel(xcenter-y,ycenter-x);
}

void MainWindow::on_midpoint_circle_clicked()
{
    int radius=ui->midp_radius->value();
    int xcenter=getx(p3.x());
    int ycenter=gety(p3.y());

    int x=0;
    int y=radius;
    int p=1-radius;

    circleplotpoint(xcenter,ycenter,x,y);

    while(x<y){
        x++;
        if(p<0)
            p+=2*x+1;
        else{
            y--;
            p+= 2*(x-y)+1;
        }
        circleplotpoint(xcenter,ycenter,x,y);
    }

}

void MainWindow::on_bresenham_clicked()
{

    int x1=p1.x();
    int y1=p1.y();
    int x2=p2.x();
    int y2=p2.y();
    int xa=getx(x1);
    int xb=getx(x2);
    int ya=gety(y1);
    int yb=gety(y2);
    drawbresenham(xa,ya,xb,yb);

}
void MainWindow::drawbresenham (int xa,int ya,int xb,int yb){
    //qDebug()<<QString::number(x)+" "+QString::number(y)+" "+QString::number(xend)+" "+QString::number(yb)+" ";
    int dx=abs(xa-xb);
    int dy=abs(ya-yb);

    if(dy<=dx){
        int flag=0;
        int p=2*dy-dx;
        int x,y,xend;
        if(ya>yb){
            x=xb;
            y=yb;
            xend=xa;
        }
        else{
            x=xa;
            y=ya;
            xend=xb;
        }

        if(x<xend) flag=1;
        else flag=-1;

        //qDebug()<<QString::number(x)+" "+QString::number(y)+" "+QString::number(xend)+" "+QString::number(flag)+" ";
        mysetpixel(x,y);
        while((flag==1&&x<xend)||(flag==-1&&x>xend)){
            x+=flag;
            if(p<0){
                p+=(2*dy);
            }
            else{
                p+=(2*dy-2*dx);
                y++;
            }
            mysetpixel(x,y);
        }
    }
    else{
        int p=2*dx-dy;
        int x,y,yend;
        int flag=0;
        if(xa>xb){
            y=yb;
            x=xb;
            yend=ya;
        }
        else{
            y=ya;
            x=xa;
            yend=yb;
        }
        if(y<yend) flag=1;
        else flag=-1;
        mysetpixel(x,y);
        while((flag==1&&y<yend)||(flag==-1&&y>yend)){
            y+=flag;
            if(p<0)
                p+=(2*dx);
            else{
                p+=(2*dx-2*dy);
                x++;
            }
            mysetpixel(x,y);
        }
    }

}
void MainWindow::on_polygon_clicked()
{
    int x1=ui->frame->x;
    int y1=ui->frame->y;
    x1=getx(x1);
    y1=gety(y1);
    polygon[polygonindex][0]=x1;
    polygon[polygonindex][1]=y1;
    //qDebug()<<QString::number(x1)+" "+QString::number(y1)+" ";
    if(polygonindex>0){
        drawbresenham(x1,y1,polygon[polygonindex-1][0],polygon[polygonindex-1][1]);
    }
    polygonindex++;
}

void MainWindow::on_transform_clicked()
{
    int sx=ui->displace_x->value();
    int sy=ui->displace_y->value();
    int i=0;
    polygon[0][0]+=sx;
    polygon[0][1]+=sy;

    for(i=1;i<polygonindex;i++){
        polygon[i][0]+=sx;
        polygon[i][1]+=sy;
        drawbresenham(polygon[i][0],polygon[i][1],polygon[i-1][0],polygon[i-1][1]);
    }
}

void MainWindow::on_set_pivot_clicked()
{
    p3.setX(ui->frame->x);
    p3.setY(ui->frame->y);
}
void MainWindow::on_rotation_clicked()
{
    qreal theta=ui->rotate_angle->value();
    theta=(theta*M_PI/180);
    int i=0;
    int tx,ty;

    int xr=getx(p3.x());
    int yr=gety(p3.y());

    tx=polygon[i][0];
    ty=polygon[i][1];
    polygon[i][0]=tx*cos(theta)-ty*sin(theta)+xr*(1-cos(theta))+yr*sin(theta);
    polygon[i][1]=ty*cos(theta)+tx*sin(theta)+yr*(1-cos(theta))-xr*sin(theta);

    for(i=1;i<polygonindex;i++){
        tx=polygon[i][0];
        ty=polygon[i][1];
        polygon[i][0]=tx*cos(theta)-ty*sin(theta)+xr*(1-cos(theta))+yr*sin(theta);
        polygon[i][1]=ty*cos(theta)+tx*sin(theta)+yr*(1-cos(theta))-xr*sin(theta);
        drawbresenham(polygon[i][0],polygon[i][1],polygon[i-1][0],polygon[i-1][1]);
    }
}

