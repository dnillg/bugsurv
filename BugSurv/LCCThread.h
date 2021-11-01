/*
** @project			BugSurv Framework
** @file			LCCThread.h
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Az USB-n csatlakoztatott kamerákhoz használt kapcsolati szál osztálydeklarációja.
*/

#ifndef LCCTHREAD_H
#define LCCTHREAD_H

// $ OpenCV
#include <opencv2\opencv.hpp>

// < QT
#include <QObject>
#include <QThread>
// >

using namespace cv;

class LCCThread :
	public QObject
{
	Q_OBJECT
public:
	LCCThread(int);
	virtual ~LCCThread();
	void softStop();
	void setCamId(int);
	bool isAlive();
protected:
	Mat frame;
	VideoCapture* pCapture;
	bool stopFlag;
	int camId;
public slots:
	void doWork();
signals:
	void frameCaptured(Mat frame);
};

#endif // LCCTHREAD_H