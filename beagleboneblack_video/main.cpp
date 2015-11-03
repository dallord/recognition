#include <QApplication>
#include <QThread>
#include <QMutex>
#include <QMessageBox>

#include <QColor>
#include <QLabel>
#include <QTextEdit>
#include <QtDebug>
#include <QString>
#include <QPushButton>

#include "LeptonThread.h"
#include "MyLabel.h"

int main( int argc, char **argv )
{
	//create the app
	QApplication a( argc, argv );
	
	QWidget *myWidget = new QWidget;
    myWidget->setGeometry(400, 300, 340, 290);

	//create an image placeholder for myLabel
	//fill the top left corner with red, just bcuz
	QImage myImage;
	myImage = QImage(320, 240, QImage::Format_RGB888);
    QRgb red = qRgb(255,0,0);
	for(int i=0;i<80;i++) {
		for(int j=0;j<60;j++) {
			myImage.setPixel(i, j, red);
		}
    }

	//create a label, and set it's image to the placeholder
	MyLabel myLabel(myWidget);
    myLabel.setGeometry(10, 10, 320, 240);
	myLabel.setPixmap(QPixmap::fromImage(myImage));

    //create a save button and label
    //QPushButton *button1 = new QPushButton("Save", myWidget);
    //button1->setGeometry(320/2-120, 290-35, 100, 30);
    /*QTextEdit *fname =  new QTextEdit("test0",myWidget);
    fname->setGeometry(320/2-120, 290-35, 100, 30);

    //create a start button
    QPushButton *button2 = new QPushButton("Start", myWidget);
    button2->setGeometry(320/2+10, 290-35, 50, 30);
    //create a stop button
    QPushButton *button3 = new QPushButton("Stop", myWidget);
    button3->setGeometry(320/2+80, 290-35, 50, 30);*/



	//create a thread to gather SPI data
	//when the thread emits updateImage, the label should update its image accordingly
	LeptonThread *thread = new LeptonThread();
	QObject::connect(thread, SIGNAL(updateImage(QImage)), &myLabel, SLOT(setImage(QImage)));

    thread->start();

    //QObject::connect(button2, SIGNAL(clicked()), thread, SLOT(start()));
    //QObject::connect(button3, SIGNAL(clicked()), thread, SLOT(Test()));
	
	myWidget->show();

	return a.exec();
}

