#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>

#include <fstream>
#include <iostream>

namespace Ui {
class MainWindow;
}

struct X{ //input vector of reference point coordinate
    int x0;
    int y0;
    int x1;
    int y1;
    int x2;
    int y2;
};

struct Y{ //otput vector of states
    float s0;
    float s1;
    float s2;
    float s3;

    int type;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


    
private:
    Ui::MainWindow *ui;

    void readpoints();

protected:
    void paintEvent(QPaintEvent *);
};

#endif // MAINWINDOW_H
