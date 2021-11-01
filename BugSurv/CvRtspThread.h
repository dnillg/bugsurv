/*
** @project			BugSurv Framework
** @file			CvRtspThread.h
** @author			Giczi D�niel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Az openCV-s kapcsolat h�tt�rben fut� streamel� sz�l�nak deklar�ci�ja.
*/

#ifndef CVRTSPTHREAD_H
#define CVRTSPTHREAD_H

// $ Konfigur�ci�
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
	int stime; // : Ennyi id� telik el egy-egy k�pkocka k�z�tt
	bool stop; // : K�vetkez� frame helyett meg�ll
	string address; // : RTSP c�l c�me
	void run();
signals:
	void frameCaptured(Mat image);
	void errorOccured();
	void FPSChanged(int);
};

#endif // CVRTSPTHREAD_H