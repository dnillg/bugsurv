/*
** @project			BugSurv Framework
** @file			CvRtspThread.h
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Az openCV-s kapcsolat háttérben futó streamelõ szálának deklarációja.
*/

#ifndef CVRTSPTHREAD_H
#define CVRTSPTHREAD_H

// $ Konfiguráció
#include "cfg.h"

// $ OpenCV
#include "opencv2/opencv.hpp" 

// < QT
#include <qobject.h>
#include <qthread.h>
// >

using namespace cv;
using namespace std;

class CvRtspThread : public QThread
{
Q_OBJECT
public:
	CvRtspThread(void);
	virtual ~CvRtspThread(void);
	void stopStreaming();
	void setAddress(QString& addr);
	void setFPS(int);
protected:
	int stime; // : Ennyi idõ telik el egy-egy képkocka között
	bool stop; // : Következõ frame helyett megáll
	string address; // : RTSP cél címe
	void run();
signals:
	void frameCaptured(Mat image);
	void errorOccured();
	void FPSChanged(int);
};

#endif // CVRTSPTHREAD_H