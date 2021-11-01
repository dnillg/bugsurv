/*
** @project			BugSurv Framework
** @file			LCCThread.cpp
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Az USB-n csatlakoztatott kamerákhoz használt kapcsolati szál implementációja.
*/

#include "LCCThread.h"

LCCThread::LCCThread(int _id):stopFlag(false),camId(_id),pCapture(0)
{}


LCCThread::~LCCThread()
{}

void LCCThread::softStop(){
	stopFlag = true;
}

void LCCThread::setCamId(int _id){
	camId = _id;
}

void LCCThread::doWork(){
	pCapture = new VideoCapture(camId);
	pCapture->read(frame);
	int fps = pCapture->get(CV_CAP_PROP_FPS);
	int stime = (fps > 60 || fps < 1)?25:1000/fps;
	while(!stopFlag){
		pCapture->read(frame);
		if(frame.datastart){
			emit frameCaptured(frame);
		}
		QThread::msleep(stime);
	}
	delete pCapture;
	pCapture = 0;
}

bool LCCThread::isAlive(){
	return (pCapture)?pCapture->isOpened():false;
}