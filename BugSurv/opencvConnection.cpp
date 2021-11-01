/*
** @project			BugSurv Framework
** @file			opencvConnection.cpp
** @author			Giczi D�niel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		OpenCV-s kapcsolat oszt�ly�nak defin�ci�ja.
*/

#include "opencvConnection.h"

OpencvConnection::OpencvConnection(int _connID):Connection(_connID),stop(false)
{}

// Visszat�r az enumerate typ�us OPENCV-nak megfelel� �rt�k�vel.
int OpencvConnection::getConnectionType(){
	return Connection::OPENCV;
}

// Csatlakoz�s implement�ci�ja OpenCV-s kapcsolathoz.
bool OpencvConnection::connect(void* conf, bool test){
	if(!test){
		QObject::connect(&streamThread,SIGNAL(frameCaptured(Mat)),this,SLOT(handleCapturedFrame(Mat)));
		QObject::connect(&streamThread,SIGNAL(FPSChanged(int)),this,SLOT(passFPSChanged(int)));
	}
	streamThread.setAddress(*(QString*)conf);
	int a = CVRTSP_TIMEOUT;
	streamThread.start();
	QThread::msleep(CVRTSP_TIMEOUT);
	return isAlive();
}

// Kapcsolat bont�sa
bool OpencvConnection::disconnect(){
	QObject::disconnect(&streamThread,SIGNAL(frameCaptured(Mat)),this,SLOT(handleCapturedFrame(Mat)));
	stop = true;
	streamThread.stopStreaming();
	while(streamThread.isRunning()){
		QThread::msleep(50);
	}
	return true;
}

// Visszaadja, hogy �l-e m�g a kapcsolat. Sikertelen kapcsol�d�s eset�n a sz�l v�get �r.
bool OpencvConnection::isAlive(){
	return streamThread.isRunning();
}

// �tpasszolja a Connection �s CvRtspThread k�z�tt a jelet.
void OpencvConnection::passFPSChanged(int v){
	setFPS(v);
}

// Feldolgoz�sra tov�bb�tja a k�pkock�t.
void OpencvConnection::handleCapturedFrame(Mat image){
	if(!stop){
		processFrame(image);
	}
}

void OpencvConnection::setFPS(int fps){
	Connection::setFPS(fps);
	streamThread.setFPS(fps);
}

OpencvConnection::~OpencvConnection(void)
{
}
