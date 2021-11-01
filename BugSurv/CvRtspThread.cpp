/*
** @project			BugSurv Framework
** @file			CvRtspThread.cpp
** @author			Giczi D�niel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Az openCV-s kapcsolat h�tt�rben fut� streamel� sz�l�nak defin�ci�ja.
*/

#include "CvRtspThread.h"

CvRtspThread::CvRtspThread(void) :
	stop(false),
	stime(0)
{
}

// C�l be�ll�t�sa.
void CvRtspThread::setAddress(QString& addr){
	address = addr.toStdString();
}

// Sz�l fut�sa.
void CvRtspThread::run(){
	cv::VideoCapture vcap;
    cv::Mat image;

	if(vcap.open(address)){
		emit FPSChanged(vcap.get(CV_CAP_PROP_FPS));
		stime = 1.0/vcap.get(CV_CAP_PROP_FPS)*1000; // : sleepTime - ennyi id� telik el frame-k k�z�tt
		QThread::msleep(CVRTSP_TIMEOUT);
		while(!stop){
			if(vcap.read(image)) {
				emit frameCaptured(image);
				QThread::msleep(stime);
				//TODO: K�shet/siethet, val�sz�n�leg nem pontos.
			} else {
				emit errorOccured();
			}
		}
		vcap.release();
	}
}

// Be�ll�tja igazra a stopflaget.
void CvRtspThread::stopStreaming(){
	stop = true;
}

// Be�ll�tja "k�zzel" az FPS-t.
void CvRtspThread::setFPS(int fps){
	stime = 1.0/fps*1000;
}

CvRtspThread::~CvRtspThread(void)
{
}
