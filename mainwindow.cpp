#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QImage>
#include <iostream>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintDevice>
#include <QPoint>
#define ROUND(a) ((int)(a+0.5))

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
    //if(ui->draw_line->isChecked())
    {
        p1.setX(ui->frame->x);
        p1.setY(ui->frame->y);
    }
}

void MainWindow::on_set_point2_clicked()
{
    //if(ui->draw_line->isChecked())
    {
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
    //std::cerr<<grsz;
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

void MainWindow::circleplotpoint(int xcenter,int ycenter,int x,int y,int a=100,int b=100,int c=100){
    mysetpixel(xcenter+x,ycenter+y,a,b,c);
    mysetpixel(xcenter-x,ycenter+y,a,b,c);
    mysetpixel(xcenter+x,ycenter-y,a,b,c);
    mysetpixel(xcenter-x,ycenter-y,a,b,c);

    mysetpixel(xcenter+y,ycenter+x,a,b,c);
    mysetpixel(xcenter-y,ycenter+x,a,b,c);
    mysetpixel(xcenter+y,ycenter-x,a,b,c);
    mysetpixel(xcenter-y,ycenter-x,a,b,c);
}


void MainWindow::on_midpoint_circle_clicked()
{
    int radius=ui->midp_radius->value();
    int xcenter=getx(p3.x());
    int ycenter=gety(p3.y());

    int x=0;
    int y=radius;
    int p=1-radius;

    circleplotpoint(xcenter,ycenter,x,y,180,180,180);

    while(x<y){
        x++;
        if(p<0)
            p+=2*x+1;
        else{
            y--;
            p+= 2*(x-y)+1;
        }
        circleplotpoint(xcenter,ycenter,x,y,180,180,180);
    }
}

void MainWindow::on_bresenham_circle_clicked()
{
    int radius=ui->midp_radius->value();
    int xcenter=getx(p3.x());
    int ycenter=gety(p3.y());

    int x=0,y=radius;
    int D=3-2*radius;
    for(;x<=y;x++){
        circleplotpoint(xcenter,ycenter,x,y,160,120,210);
        if(D>0){
            y--;
            D+=(4*(x-y)+10);
        }
        else
            D+=(4*(x)+6);
    }
}

void MainWindow::drawbresenham (int xa,int ya,int xb,int yb,int red=150,int green=150,int blue=150){
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
        mysetpixel(x,y,red,green,blue);
        while((flag==1&&x<xend)||(flag==-1&&x>xend)){
            x+=flag;
            if(p<0){
                p+=(2*dy);
            }
            else{
                p+=(2*dy-2*dx);
                y++;
            }
            mysetpixel(x,y,red,green,blue);
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
        mysetpixel(x,y,red,green,blue);
        while((flag==1&&y<yend)||(flag==-1&&y>yend)){
            y+=flag;
            if(p<0)
                p+=(2*dx);
            else{
                p+=(2*dx-2*dy);
                x++;
            }
            mysetpixel(x,y,red,green,blue);
        }
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
//====================================ellipse=====================================
void MainWindow::ellipseplotpoint(int xcenter,int ycenter,int x,int y,int a=150,int b=150,int c=150){
    mysetpixel(xcenter+x,ycenter+y,a,b,c);
    mysetpixel(xcenter+x,ycenter-y,a,b,c);
    mysetpixel(xcenter-x,ycenter+y,a,b,c);
    mysetpixel(xcenter-x,ycenter-y,a,b,c);
}

void MainWindow::on_draw_ellipse_clicked()
{
    int xrad= ui->xradius->value();
    int yrad= ui->yradius->value();
    int xcenter=getx(p3.x());
    int ycenter=gety(p3.y());

    int p;
    int x=0;
    int y=yrad;
    int px=0;
    int rx2=xrad*xrad;
    int ry2=yrad*yrad;
    int py=2*rx2*y;

    ellipseplotpoint(xcenter,ycenter,x,y);

    p=ROUND(ry2-(rx2*yrad)+(.25*rx2));
    while(px<py){
        x++;
        px+=2*ry2;
        if(p<0)
            p+=(ry2+px);
        else{
            y--;
            py-=(2*rx2);
            p+=(ry2+px-py);
        }
        ellipseplotpoint(xcenter,ycenter,x,y,190,65,140);
    }

    p=ROUND(ry2*(x+0.5)*(x+0.5)+rx2*(y-1)*(y-1)-rx2*ry2);
    while(y>0){
        y--;
        py-=2*rx2;
        if(p>0)
            p+=(rx2-py);
        else{
            x++;
            px+=2*ry2;
            p+=rx2-py+px;
        }
        ellipseplotpoint(xcenter,ycenter,x,y,65,140,190);
    }
}

void MainWindow::on_ellipse_center_clicked()
{
    p3.setX(ui->frame->x);
    p3.setY(ui->frame->y);
}

//======================2D viewing==========================================================
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
void MainWindow::on_reflection_clicked()
{
    //if(x2==x1)//
    //float m=(float)
    int px=getx(p1.x());
    int py=gety(p1.y());
    int qx=getx(p2.x());
    int qy=gety(p2.y());

    int i=0;
    int a=polygon[i][0];
    int b=polygon[i][1];

    int A=py-qy;
    int B=qx-px;
    int C= -py*(qx-px)-px*(py-qy);

    float a1=(a*B*B-a*A*A-2*b*A*B-2*A*C)/(float)(A*A+B*B);
    float b1=(b*A*A-b*B*B-2*a*A*B-2*B*C)/(float)(A*A+B*B);


    polygon[i][0]=(a1);
    polygon[i][1]=(b1);
    //qDebug()<<QString::number(a)+" "+QString::number(b)+" "+QString::number(a1)+" "+QString::number(b1)+" ";
    for(i=1;i<polygonindex;i++){
        a=polygon[i][0];
        b=polygon[i][1];
        A=py-qy;
        B=qx-px;
        C= -py*(qx-px)-px*(py-qy);
        a1=(a*B*B-a*A*A-2*b*A*B-2*A*C)/(float)(A*A+B*B);
        b1=(b*A*A-b*B*B-2*a*A*B-2*B*C)/(float)(A*A+B*B);
        polygon[i][0]=(a1);
        polygon[i][1]=(b1);
        drawbresenham(polygon[i][0],polygon[i][1],polygon[i-1][0],polygon[i-1][1]);
    }
}


void MainWindow::on_scale_clicked()
{
    int xr=getx(p3.x());
    int yr=gety(p3.y());

    float rx=ui->scalex->value();
    float ry=ui->scaley->value();

    int i=0;
    int a,b;
    a=polygon[i][0];
    b=polygon[i][1];
    a-=xr;
    b-=yr;
    a*=rx;
    b*=ry;
    a+=xr;
    b+=yr;
    qDebug()<<QString::number(polygon[i][0])+" "+QString::number(polygon[i][1])+" "+QString::number(a)+" "+QString::number(b)+" ";
    polygon[i][0]=a;
    polygon[i][1]=b;
    for(i=1;i<polygonindex;i++){
        a=polygon[i][0];
        b=polygon[i][1];
        a-=xr;
        b-=yr;
        a*=rx;
        b*=ry;
        a+=xr;
        b+=yr;
        qDebug()<<QString::number(polygon[i][0])+" "+QString::number(polygon[i][1])+" "+QString::number(a)+" "+QString::number(b)+" ";
        polygon[i][0]=a;
        polygon[i][1]=b;
        drawbresenham(polygon[i][0],polygon[i][1],polygon[i-1][0],polygon[i-1][1]);
    }
}

void MainWindow::on_shear_clicked()
{
    int xr=getx(p3.x());
    int yr=gety(p3.y());

    float shx=ui->scalex->value();
    float shy=ui->scaley->value();
    int i=0;
    int a,b,a1,b1;
    a=polygon[i][0];
    b=polygon[i][1];
    a1=a+shx*(b-yr);
    b1=b+shy*(a-xr);
    //qDebug()<<QString::number(polygon[i][0])+" "+QString::number(polygon[i][1])+" "+QString::number(a)+" "+QString::number(b)+" ";
    polygon[i][0]=a1;
    polygon[i][1]=b1;
    for(i=1;i<polygonindex;i++){
        a=polygon[i][0];
        b=polygon[i][1];
        a1=a+shx*(b-yr);
        b1=b+shy*(a-xr);
        //qDebug()<<QString::number(polygon[i][0])+" "+QString::number(polygon[i][1])+" "+QString::number(a)+" "+QString::number(b)+" ";
        polygon[i][0]=a1;
        polygon[i][1]=b1;
        drawbresenham(polygon[i][0],polygon[i][1],polygon[i-1][0],polygon[i-1][1]);
    }

}

void MainWindow::on_clear_polygon_clicked()
{
    polygonindex=0;
}


//============CLIPPING===========================================================
#define LEFTEDGE 0x1
#define RIGHTEDGE 0x2
#define BOTTOMEDGE 0x4
#define TOPEDGE 0x8
#define INSIDE(a) (!a)
#define REJECT(a,b) (a&b)
#define ACCEPT(a,b) (!(a|b))
void swapcodes(unsigned char *c1,unsigned char *c2){
    unsigned char temp;
    temp=*c1;
    *c1=*c2;
    *c2=temp;
}

void swappts(QPoint *p1,QPoint *p2){
    QPoint tmp;
    tmp=*p1;
    *p1=*p2;
    *p2=tmp;
}

unsigned char encode(QPoint p,int minx,int miny,int maxx,int maxy){
    unsigned char code=0x0;
    if(p.x()<minx)
        code=code|LEFTEDGE;
    if(p.x()>maxx)
        code=code|RIGHTEDGE;
    if(p.y()<miny)
        code=code|BOTTOMEDGE;
    if(p.y()>maxy)
        code=code|TOPEDGE;
    return code;
}

void MainWindow::on_minwin_clicked()
{
    wmin.setX(ui->frame->x);
    wmin.setY(ui->frame->y);
}
void MainWindow::on_maxwin_clicked()
{
    wmax.setX(ui->frame->x);
    wmax.setY(ui->frame->y);
}

void MainWindow::on_draw_window_clicked()
{
    int a=0,b=0,c=0,d=0;
    a=getx(wmax.x());
    b=gety(wmax.y());
    c=getx(wmin.x());
    d=b;
    drawbresenham(a,b,c,d);

    c=a;
    d=gety(wmin.y());
    drawbresenham(a,b,c,d);

    a=getx(wmin.x());
    b=gety(wmin.y());
    drawbresenham(a,b,c,d);

    c=a;
    b=gety(wmax.y());
    drawbresenham(a,b,c,d);

}

void MainWindow::on_clipline_clicked()
{
    QPoint tmp;

    int a,b,c,d;
    a=getx(p1.x());
    b=gety(p1.y());
    c=getx(p2.x());
    d=gety(p2.y());

    p3.setX(a);
    p3.setY(b);
    p4.setX(c);
    p4.setY(d);
    int minx=getx(wmin.x());
    int miny=gety(wmin.y());
    int maxx=getx(wmax.x());
    int maxy=gety(wmax.y());
    //-----------------------------------
    unsigned char code1,code2;
    int done=0,draw=0;
    float m;
    qDebug()<<QString::number(minx)+" "+QString::number(miny);
    qDebug()<<QString::number(maxx)+" "+QString::number(maxy);

    qDebug()<<QString::number(p3.x())+" "+QString::number(p3.y());
    while(!done){
        code1=encode(p3,minx,miny,maxx,maxy);
        code2=encode(p4,minx,miny,maxx,maxy);
        qDebug()<<code1<<" "<<code2<<" ";
        if(ACCEPT(code1,code2)){
            done=1;
            draw=1;
            qDebug()<<"accept";
        }
        else if(REJECT(code1,code2)){
            done=1;
            qDebug()<<"reject";
        }
        else{
            if(INSIDE(code1)){
                swappts(&p3,&p4);
                swapcodes(&code1,&code2);
            }
            if(p3.x()!=p4.x())
                m=(p4.y()-p3.y())/(float)(p4.x()-p3.x());
            if(code1&LEFTEDGE){
                int temp=p3.y()+(minx-p3.x())*m;
                p3.setY(temp);
                p3.setX(minx);
            }
            else if(code1&BOTTOMEDGE){
                qDebug()<<"bottom";
                if(p3.x()!=p4.x()){
                    int temp=p3.x()+(miny-p3.y())/m;
                    p3.setX(temp);
                }
                p3.setY(miny);
            }
            else if(code1&TOPEDGE){
                qDebug()<<"top";
                if(p3.x()!=p4.x()){
                    int temp=p3.x()+(maxy-p3.y())/m;
                    p3.setX(temp);
                }
                p3.setY(maxy);
            }
            else if(code1&RIGHTEDGE){
                qDebug()<<"right";
                int temp=p3.y()+(maxx-p3.x())*m;
                p3.setY(temp);
                p3.setX(maxx);
            }
        }
    }
    if(draw){
        drawbresenham(p3.x(),p3.y(),p4.x(),p4.y());
    }
}
//----------------------POLYGON CLIPPING---------------------------------------------------
#define MAX_POINTS 50
int x_intersect(int x1, int y1, int x2, int y2,
                int x3, int y3, int x4, int y4)
{
    int num = (x1*y2 - y1*x2) * (x3-x4) -
              (x1-x2) * (x3*y4 - y3*x4);
    int den = (x1-x2) * (y3-y4) - (y1-y2) * (x3-x4);
    return num/den;
}

// Returns y-value of point of intersectipn of
// two lines
int y_intersect(int x1, int y1, int x2, int y2,
                int x3, int y3, int x4, int y4)
{
    int num = (x1*y2 - y1*x2) * (y3-y4) -
              (y1-y2) * (x3*y4 - y3*x4);
    int den = (x1-x2) * (y3-y4) - (y1-y2) * (x3-x4);
    return num/den;
}

// This functions clips all the edges w.r.t one clip
// edge of clipping area
void clip(int poly_points[][2], int &poly_size,
          int x1, int y1, int x2, int y2)
{
    int new_points[MAX_POINTS][2], new_poly_size = 0;

    // (ix,iy),(kx,ky) are the co-ordinate values of
    // the points
    for (int i = 0; i < poly_size; i++)
    {
        // i and k form a line in polygon
        int k = (i+1) % poly_size;
        int ix = poly_points[i][0], iy = poly_points[i][1];
        int kx = poly_points[k][0], ky = poly_points[k][1];

        // Calculating position of first point
        // w.r.t. clipper line
        int i_pos = (x2-x1) * (iy-y1) - (y2-y1) * (ix-x1);

        // Calculating position of second point
        // w.r.t. clipper line
        int k_pos = (x2-x1) * (ky-y1) - (y2-y1) * (kx-x1);

        // Case 1 : When both points are inside
        if (i_pos < 0  && k_pos < 0)
        {
            //Only second point is added
            new_points[new_poly_size][0] = kx;
            new_points[new_poly_size][1] = ky;
            new_poly_size++;
        }

        // Case 2: When only first point is outside
        else if (i_pos >= 0  && k_pos < 0)
        {
            // Point of intersection with edge
            // and the second point is added
            new_points[new_poly_size][0] = x_intersect(x1,
                              y1, x2, y2, ix, iy, kx, ky);
            new_points[new_poly_size][1] = y_intersect(x1,
                              y1, x2, y2, ix, iy, kx, ky);
            new_poly_size++;

            new_points[new_poly_size][0] = kx;
            new_points[new_poly_size][1] = ky;
            new_poly_size++;
        }

        // Case 3: When only second point is outside
        else if (i_pos < 0  && k_pos >= 0)
        {
            //Only point of intersection with edge is added
            new_points[new_poly_size][0] = x_intersect(x1,
                              y1, x2, y2, ix, iy, kx, ky);
            new_points[new_poly_size][1] = y_intersect(x1,
                              y1, x2, y2, ix, iy, kx, ky);
            new_poly_size++;
        }

        // Case 4: When both points are outside
        else
        {
            //No points are added
        }
    }

    // Copying new points into original array
    // and changing the no. of vertices
    poly_size = new_poly_size;
    for (int i = 0; i < poly_size; i++)
    {
        poly_points[i][0] = new_points[i][0];
        poly_points[i][1] = new_points[i][1];
    }
}

// Implements Sutherland–Hodgman algorithm
void suthHodgClip(int poly_points[][2], int &poly_size,
                  int clipper_points[][2], int clipper_size)
{
    //i and k are two consecutive indexes
    for (int i=0; i<clipper_size; i++)
    {
        int k = (i+1) % clipper_size;

        // We pass the current array of vertices, it's size
        // and the end points of the selected clipper line
        clip(poly_points, poly_size, clipper_points[i][0],
             clipper_points[i][1], clipper_points[k][0],
             clipper_points[k][1]);
    }

    // Printing vertices of clipped polygon
    for (int i=0; i < poly_size; i++){

    }
}


void MainWindow::on_clip_pol_clicked()
{
    int i;
    int clipper_size=4;
    int clipper_points[4][2];
    clipper_points[0][0]= getx(wmin.x()); clipper_points[0][1]=gety(wmin.y()) ;
    clipper_points[1][0]= getx(wmin.x()); clipper_points[1][1]=gety(wmax.y());
    clipper_points[2][0]= getx(wmax.x()); clipper_points[2][1]=gety(wmax.y());
    clipper_points[3][0]= getx(wmax.x()); clipper_points[3][1]=gety(wmin.y());

    int poly_size=polygonindex;
    int pol_pts[MAX_POINTS][2];
    for(i=0;i<polygonindex;i++){
        pol_pts[i][0]= polygon[i][0];
        pol_pts[i][1]= polygon[i][1];
    }
    suthHodgClip(pol_pts,poly_size,clipper_points,clipper_size);
    for(i=1;i<poly_size;i++){
        drawbresenham(pol_pts[i-1][0],pol_pts[i-1][1],pol_pts[i][0],pol_pts[i][1],200,200,200);
    }
    drawbresenham(pol_pts[i-1][0],pol_pts[i-1][1],pol_pts[0][0],pol_pts[0][1],200,200,200);
}

//===================================BEIZER CURVE===========================================================
int bincoeff(int n, int k)
{
    int res = 1;
    if ( k > n - k )
        k = n - k;
    for (int i = 0; i < k; ++i){
        res *= (n - i);
        res /= (i + 1);
    }
    return res;
}
void computecoeffs(int n,int *c){
    int k,i;
    for(k=0;k<=n;k++){
        c[k]=1;
        c[k]=bincoeff(n,k);
    }
}
void computepoint(float u,int points[][2],int npts,int *c,int outpts[][2],int index){
    int k,n=npts-1;
    float blend,a,b;
    a=0.0;b=0.0;
    for(k=0;k<npts;k++){
        blend=c[k]*pow(u,k)*pow(1-u,n-k);
        a+=points[k][0]*blend;
        b+=points[k][1]*blend;
    }
    outpts[index][0]=ROUND(a);
    outpts[index][1]=ROUND(b);
}


void MainWindow::on_beizer_clicked() 
{
    int m= ui->curve_pts->value();
    int outpts[m+1][2];
    int npts=polygonindex;
    int points[npts][2];
    int i;
    for(i=0;i<npts;i++){
        points[i][0]=polygon[i][0];
        points[i][1]=polygon[i][1];
    }
    int *c=(int*)malloc(npts*sizeof(int));
    computecoeffs(npts-1,c);
    for(i=0;i<=m;i++){
        computepoint(i/(float)m,points,npts,c,outpts,i);
        //qDebug()<<QString::number(outpts[i][0])<<" "<<QString::number(outpts[i][1]);
    }
    free(c);
    for(i=1;i<=m;i++){
        drawbresenham(outpts[i-1][0],outpts[i-1][1],outpts[i][0],outpts[i][1],200,200,100);
    }
}
//scanline==========================================================================
#define maxHt 100
#define maxWd 100
#define maxVer 100


// Start from lower left corner
typedef struct edgebucket
{
    int ymax;   //max y-coordinate of edge
    float xofymin;  //x-coordinate of lowest edge point updated only in aet
    float slopeinverse;
}EdgeBucket;

typedef struct edgetabletup
{
    // the array will give the scanline number
    // The edge table (ET) with edges entries sorted
    // in increasing y and x of the lower end

    int countEdgeBucket;    //no. of edgebuckets
    EdgeBucket buckets[maxVer];
}EdgeTableTuple;

EdgeTableTuple EdgeTable[maxHt], ActiveEdgeTuple;


// Scanline Function
void initEdgeTable()
{
    int i;
    for (i=0; i<maxHt; i++)
    {
        EdgeTable[i].countEdgeBucket = 0;
    }

    ActiveEdgeTuple.countEdgeBucket = 0;
}
void printTuple(EdgeTableTuple *tup)
{
}
void printTable()
{
}
/* Function to sort an array using insertion sort*/
void insertionSort(EdgeTableTuple *ett)
{
    int i,j;
    EdgeBucket temp;

    for (i = 1; i < ett->countEdgeBucket; i++)
    {
        temp.ymax = ett->buckets[i].ymax;
        temp.xofymin = ett->buckets[i].xofymin;
        temp.slopeinverse = ett->buckets[i].slopeinverse;
        j = i - 1;

    while ((temp.xofymin < ett->buckets[j].xofymin) && (j >= 0))
    {
        ett->buckets[j + 1].ymax = ett->buckets[j].ymax;
        ett->buckets[j + 1].xofymin = ett->buckets[j].xofymin;
        ett->buckets[j + 1].slopeinverse = ett->buckets[j].slopeinverse;
        j = j - 1;
    }
    ett->buckets[j + 1].ymax = temp.ymax;
    ett->buckets[j + 1].xofymin = temp.xofymin;
    ett->buckets[j + 1].slopeinverse = temp.slopeinverse;
    }
}


void storeEdgeInTuple (EdgeTableTuple *receiver,int ym,int xm,float slopInv)
{
    // both used for edgetable and active edge table..
    // The edge tuple sorted in increasing ymax and x of the lower end.
    (receiver->buckets[(receiver)->countEdgeBucket]).ymax = ym;
    (receiver->buckets[(receiver)->countEdgeBucket]).xofymin = (float)xm;
    (receiver->buckets[(receiver)->countEdgeBucket]).slopeinverse = slopInv;

    // sort the buckets
    insertionSort(receiver);

    (receiver->countEdgeBucket)++;


}

void storeEdgeInTable (int x1,int y1, int x2, int y2)
{
    float m,minv;
    int ymaxTS,xwithyminTS, scanline; //ts stands for to store

    if (x2==x1)
    {
        minv=0.000000;
    }
    else
    {
    m = ((float)(y2-y1))/((float)(x2-x1));

    // horizontal lines are not stored in edge table
    if (y2==y1)
        return;

    minv = (float)1.0/m;
    printf("\nSlope string for %d %d & %d %d: %f",x1,y1,x2,y2,minv);
    }

    if (y1>y2)
    {
        scanline=y2;
        ymaxTS=y1;
        xwithyminTS=x2;
    }
    else
    {
        scanline=y1;
        ymaxTS=y2;
        xwithyminTS=x1;
    }
    // the assignment part is done..now storage..
    storeEdgeInTuple(&EdgeTable[scanline],ymaxTS,xwithyminTS,minv);


}

void removeEdgeByYmax(EdgeTableTuple *Tup,int yy)
{
    int i,j;
    for (i=0; i< Tup->countEdgeBucket; i++)
    {
        if (Tup->buckets[i].ymax == yy)
        {
            printf("\nRemoved at %d",yy);

            for ( j = i ; j < Tup->countEdgeBucket -1 ; j++ )
                {
                Tup->buckets[j].ymax =Tup->buckets[j+1].ymax;
                Tup->buckets[j].xofymin =Tup->buckets[j+1].xofymin;
                Tup->buckets[j].slopeinverse = Tup->buckets[j+1].slopeinverse;
                }
                Tup->countEdgeBucket--;
            i--;
        }
    }
}


void updatexbyslopeinv(EdgeTableTuple *Tup)
{
    int i;

    for (i=0; i<Tup->countEdgeBucket; i++)
    {
        (Tup->buckets[i]).xofymin =(Tup->buckets[i]).xofymin + (Tup->buckets[i]).slopeinverse;
    }
}


void MainWindow::ScanlineFill()
{
    /* Follow the following rules:
    1. Horizontal edges: Do not include in edge table
    2. Horizontal edges: Drawn either on the bottom or on the top.
    3. Vertices: If local max or min, then count twice, else count
        once.
    4. Either vertices at local minima or at local maxima are drawn.*/


    int i, j, x1, ymax1, x2, ymax2, FillFlag = 0, coordCount;

    // we will start from scanline 0;
    // Repeat until last scanline:
    for (i=0; i<maxHt; i++)//4. Increment y by 1 (next scan line)
    {

        // 1. Move from ET bucket y to the
        // AET those edges whose ymin = y (entering edges)
        for (j=0; j<EdgeTable[i].countEdgeBucket; j++)
        {
            storeEdgeInTuple(&ActiveEdgeTuple,EdgeTable[i].buckets[j].
                     ymax,EdgeTable[i].buckets[j].xofymin,
                    EdgeTable[i].buckets[j].slopeinverse);
        }
        printTuple(&ActiveEdgeTuple);

        // 2. Remove from AET those edges for
        // which y=ymax (not involved in next scan line)
        removeEdgeByYmax(&ActiveEdgeTuple, i);

        //sort AET (remember: ET is presorted)
        insertionSort(&ActiveEdgeTuple);

        printTuple(&ActiveEdgeTuple);

        //3. Fill lines on scan line y by using pairs of x-coords from AET
        j = 0;
        FillFlag = 0;
        coordCount = 0;
        x1 = 0;
        x2 = 0;
        ymax1 = 0;
        ymax2 = 0;
        while (j<ActiveEdgeTuple.countEdgeBucket)
        {
            if (coordCount%2==0)
            {
                x1 = (int)(ActiveEdgeTuple.buckets[j].xofymin);
                ymax1 = ActiveEdgeTuple.buckets[j].ymax;
                if (x1==x2)
                {
                /* three cases can arrive-
                    1. lines are towards top of the intersection
                    2. lines are towards bottom
                    3. one line is towards top and other is towards bottom
                */
                    if (((x1==ymax1)&&(x2!=ymax2))||((x1!=ymax1)&&(x2==ymax2)))
                    {
                        x2 = x1;
                        ymax2 = ymax1;
                    }

                    else
                    {
                        coordCount++;
                    }
                }

                else
                {
                        coordCount++;
                }
            }
            else
            {
                x2 = (int)ActiveEdgeTuple.buckets[j].xofymin;
                ymax2 = ActiveEdgeTuple.buckets[j].ymax;

                FillFlag = 0;

                // checking for intersection...
                if (x1==x2)
                {
                /*three cases can arive-
                    1. lines are towards top of the intersection
                    2. lines are towards bottom
                    3. one line is towards top and other is towards bottom
                */
                    if (((x1==ymax1)&&(x2!=ymax2))||((x1!=ymax1)&&(x2==ymax2)))
                    {
                        x1 = x2;
                        ymax1 = ymax2;
                    }
                    else
                    {
                        coordCount++;
                        FillFlag = 1;
                    }
                }
                else
                {
                        coordCount++;
                        FillFlag = 1;
                }


            if(FillFlag)
            {
                drawbresenham(x1,i,x2,i,200,200,200);
            }
        }
        j++;
    }
    // 5. For each nonvertical edge remaining in AET, update x for new y
    updatexbyslopeinv(&ActiveEdgeTuple);
    }
}
void MainWindow::drawPolyDino()
{
    int count = 0,x1,y1,x2,y2;
    for(int iter=0;iter<polygonindex;iter++)
    {
        count++;
        if (count>2)
        {
            x1 = x2;
            y1 = y2;
            count=2;
        }
        if (count==1)
        {
            x1=polygon[iter][0];
            y1=polygon[iter][1];
        }
        else
        {
            x2=polygon[iter][0];
            y2=polygon[iter][1];
            storeEdgeInTable(x1, y1, x2, y2);
        }
    }
}

void MainWindow::on_scan_fill_clicked()
{
    //only for the first coordinate (all points must be positive)
    initEdgeTable();
    drawPolyDino();
    ScanlineFill();
}
//=============boundary fill=======================

void MainWindow::on_boundary_fill_clicked()
{

    p3.setX(ui->frame->x);
    p3.setY(ui->frame->y);

    int x1=(p3.x());
    int y1=(p3.y());

    boundary_fill_util(x1,y1,qRgb(150,150,150),255,255,25);
}

void MainWindow::boundary_fill_util(int x1,int y1,QRgb q1,int r, int g,int b){
    QRgb current;
    current=img.pixel(x1,y1);
    if(x1<=0||x1>img.width()||y1<=0||y1>img.height()) return ;
    if(current != q1 && current!=qRgb(r,g,b)){
        mysetpixel(getx(x1),gety(y1),r,g,b);
        boundary_fill_util(x1+grsz,y1,q1,r,g,b);
        boundary_fill_util(x1,y1+grsz,q1,r,g,b);
        boundary_fill_util(x1-grsz,y1,q1,r,g,b);
        boundary_fill_util(x1,y1-grsz,q1,r,g,b);
        ui->frame->setPixmap(QPixmap::fromImage(img));
    }
}
//=============flood fill=============================
void MainWindow::flood_fill_util(int x1,int y1,QRgb q1,int r, int g,int b){
    if(x1<0||x1>=img.width()||y1<0||y1>=img.height()) return ;
    if(img.pixel(x1,y1)!=q1 ||img.pixel(x1,y1)==qRgb(r,g,b) ) return;
    mysetpixel(getx(x1),gety(y1),r,g,b);
    flood_fill_util(x1+grsz,y1,q1,r,g,b);
    flood_fill_util(x1,y1+grsz,q1,r,g,b);
    flood_fill_util(x1-grsz,y1,q1,r,g,b);
    flood_fill_util(x1,y1-grsz,q1,r,g,b);
    //ui->frame->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::on_flood_fill_clicked()
{

    p3.setX(ui->frame->x);
    p3.setY(ui->frame->y);

    int x1=(p3.x());
    int y1=(p3.y());

    flood_fill_util(x1,y1,img.pixel(x1,y1),255,255,25);
}
//================================================================
