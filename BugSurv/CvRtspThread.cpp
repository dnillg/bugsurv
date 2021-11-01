/*
** @project			BugSurv Framework
** @file			CvRtspThread.cpp
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Az openCV-s kapcsolat háttérben futó streamelõ szálának definíciója.
*/

#include "CvRtspThread.h"

CvRtspThread::CvRtspThread(void) :
	stop(false),
	stime(0)
{
}

// Cél beállítása.
void CvRtspThread::setAddress(QString& addr){
	address = addr.toStdString();
}

// Szál futása.
void CvRtspThread::run(){
	cv::VideoCapture vcap;
    cv::Mat image;

	if(vcap.open(address)){
		emit FPSChanged(vcap.get(CV_CAP_PROP_FPS));
		stime = 1.0/vcap.get(CV_CAP_PROP_FPS)*1000; // : sleepTime - ennyi idõ telik el frame-k között
		QThread::msleep(CVRTSP_TIMEOUT);
		while(!stop){
			if(vcap.read(image)) {
				emit frameCaptured(image);
				QThread::msleep(stime);
				//TODO: Késhet/siethet, valószínûleg nem pontos.
			} else {
				emit errorOccured();
			}
		}
		vcap.release();
	}
}

// Beállítja igazra a stopflaget.
void CvRtspThread::stopStreaming(){
	stop = true;
}

// Beállítja "kézzel" az FPS-t.
void CvRtspThread::setFPS(int fps){
	stime = 1.0/fps*1000;
}

CvRtspThread::~CvRtspThread(void)
{
}
