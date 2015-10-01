#include "mainwindow.h"
#include "ui_mainwindow.h"

int counter;

X* points;
Y* classes;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readpoints(){
    std::ifstream t0("../../data/experiments/test0_pairs.txt");
    //std::ifstream t1("../../data/experiments/test1_pairs.txt");
    std::ifstream t2("../../data/experiments/test2_pairs.txt");
    //std::ifstream t3("../../data/experiments/test3_pairs.txt");
    std::ifstream t4("../../data/experiments/test4_pairs.txt");
    std::ifstream t5("../../data/experiments/test5_pairs.txt");

    std::string buff;
    counter = 0;
    while (!t0.eof()){
        getline(t0, buff);
        counter++;
    }
    t0.close();
   /* while (!t1.eof()){
        getline(t1, buff);
        counter++;
    }
    t1.close();*/
    while (!t2.eof()){
        getline(t2, buff);
        counter++;
    }
    t2.close();
    /*while (!t3.eof()){
        getline(t3, buff);
        counter++;
    }
    t3.close();*/
    while (!t4.eof()){
        getline(t4, buff);
        counter++;
    }
    t4.close();
    while (!t5.eof()){
        getline(t5, buff);
        counter++;
    }
    t5.close();

    counter--;

    points = new X[counter];
    classes = new Y[counter];

    t0.open("../../data/experiments/test0_pairs.txt");
    //t1.open("../../data/experiments/test1_pairs.txt");
    t2.open("../../data/experiments/test2_pairs.txt");
    //t3.open("../../data/experiments/test3_pairs.txt");
    t4.open("../../data/experiments/test4_pairs.txt");
    t5.open("../../data/experiments/test5_pairs.txt");

    int i = 0;
    std::string line;

    while(!t0.eof()){
        getline(t0, line);
        sscanf(line.c_str(), "%d %d %d %d %d %d %f %f %f %f",
               &points[i].x0, &points[i].y0, &points[i].x1, &points[i].y1, &points[i].x2, &points[i].y2,
               &classes[i].s0, &classes[i].s1, &classes[i].s2, &classes[i].s3);
        i++;
    }
    t0.close();
    /*while(!t1.eof()){
        getline(t1, line);
        sscanf(line.c_str(), "%d %d %d %d %d %d %f %f %f %f",
               &points[i].x0, &points[i].y0, &points[i].x1, &points[i].y1, &points[i].x2, &points[i].y2,
               &classes[i].s0, &classes[i].s1, &classes[i].s2, &classes[i].s3);
        i++;
    }
    t1.close();*/
    while(!t2.eof()){
        getline(t2, line);
        sscanf(line.c_str(), "%d %d %d %d %d %d %f %f %f %f",
               &points[i].x0, &points[i].y0, &points[i].x1, &points[i].y1, &points[i].x2, &points[i].y2,
               &classes[i].s0, &classes[i].s1, &classes[i].s2, &classes[i].s3);
        i++;
    }
    t2.close();
    /*while(!t3.eof()){
        getline(t3, line);
        sscanf(line.c_str(), "%d %d %d %d %d %d %f %f %f %f",
               &points[i].x0, &points[i].y0, &points[i].x1, &points[i].y1, &points[i].x2, &points[i].y2,
               &classes[i].s0, &classes[i].s1, &classes[i].s2, &classes[i].s3);
        i++;
    }
    t3.close();*/
    while(!t4.eof()){
        getline(t4, line);
        sscanf(line.c_str(), "%d %d %d %d %d %d %f %f %f %f",
               &points[i].x0, &points[i].y0, &points[i].x1, &points[i].y1, &points[i].x2, &points[i].y2,
               &classes[i].s0, &classes[i].s1, &classes[i].s2, &classes[i].s3);
        i++;
    }
    t4.close();
    while(!t5.eof()){
        getline(t5, line);
        sscanf(line.c_str(), "%d %d %d %d %d %d %f %f %f %f",
               &points[i].x0, &points[i].y0, &points[i].x1, &points[i].y1, &points[i].x2, &points[i].y2,
               &classes[i].s0, &classes[i].s1, &classes[i].s2, &classes[i].s3);
        i++;
    }
    t5.close();

}

void MainWindow::paintEvent(QPaintEvent *){
    QPainter p(this);

    QPen penB(Qt::black);
    penB.setWidth(1);

    //first axis
    QPoint p0;
    p0.setX(10);
    p0.setY(170);
    QPoint p1;
    p1.setX(10);
    p1.setY(50);
    QPoint p2;
    p2.setX(180);
    p2.setY(170);

    p.setPen(penB);
    p.drawLine(p0, p1);
    p.drawLine(p0, p2);

    //second axis
    QPoint p01;
    p01.setX(20+200);
    p01.setY(170);
    QPoint p11;
    p11.setX(20+200);
    p11.setY(50);
    QPoint p21;
    p21.setX(180+200);
    p21.setY(170);

    p.drawLine(p01, p11);
    p.drawLine(p01, p21);

    //third axis
    QPoint p02;
    p02.setX(20+400);
    p02.setY(170);
    QPoint p12;
    p12.setX(20+400);
    p12.setY(50);
    QPoint p22;
    p22.setX(180+400);
    p22.setY(170);

    p.drawLine(p02, p12);
    p.drawLine(p02, p22);

    readpoints();

    QPen pens[counter];
    QPen pens1[counter];
    QPen pens2[counter];
    QPoint po[counter];
    QPoint po1[counter];
    QPoint po2[counter];

    //legend
    p.setPen(Qt::yellow);
    p.drawText(rect(), Qt::AlignBottom, "   standing\n\n\n\n");
    p.setPen(Qt::green);
    p.drawText(rect(), Qt::AlignBottom, "   sitting\n\n\n");
    p.setPen(Qt::blue);
    p.drawText(rect(), Qt::AlignBottom, "   lying on the bed\n\n");
    p.setPen(Qt::red);
    p.drawText(rect(), Qt::AlignBottom, "   lying on the floor\n");

    ui->label_8->setText(QString::number(counter));


    for (int i = 0; i < counter; i++){
        if (classes[i].s0 == 1){      pens[i].setColor(Qt::yellow); pens1[i].setColor(Qt::yellow); pens2[i].setColor(Qt::yellow);}
        else if (classes[i].s1 == 1){ pens[i].setColor(Qt::green); pens1[i].setColor(Qt::green); pens2[i].setColor(Qt::green);}
        else if (classes[i].s2 == 1){ pens[i].setColor(Qt::blue); pens1[i].setColor(Qt::blue); pens2[i].setColor(Qt::blue);}
        else if (classes[i].s3 == 1){ pens[i].setColor(Qt::red); pens1[i].setColor(Qt::red); pens2[i].setColor(Qt::red);}
        else {pens[i].setColor(Qt::gray); pens1[i].setColor(Qt::gray); pens2[i].setColor(Qt::gray);}

        pens[i].setWidth(3);
        pens1[i].setWidth(3);
        pens2[i].setWidth(3);

        po[i].setX(2*points[i].x0 + 10);
        po[i].setY(2*points[i].y0 + 50);

        po1[i].setX(2*(points[i].x1 + 110));
        po1[i].setY(2*points[i].y1 + 50);

        po2[i].setX(2*(points[i].x2 + 210));
        po2[i].setY(2*points[i].y2 + 50);

        p.setPen(pens[i]);
        p.drawPoint(po[i]);

        p.setPen(pens1[i]);
        p.drawPoint(po1[i]);

        p.setPen(pens2[i]);
        p.drawPoint(po2[i]);
    }
}
