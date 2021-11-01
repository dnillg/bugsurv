/*
** @project			BugSurv Framework
** @file			opencvConnection.cpp
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		OpenCV-s kapcsolat osztályának definíciója.
*/

#include "opencvConnection.h"

OpencvConnection::OpencvConnection(int _connID):Connection(_connID),stop(false)
{}

// Visszatér az enumerate typíus OPENCV-nak megfelelõ értékével.
int OpencvConnection::getConnectionType(){
	return Connection::OPENCV;
}

// Csatlakozás implementációja OpenCV-s kapcsolathoz.
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

// Kapcsolat bontása
bool OpencvConnection::disconnect(){
	QObject::disconnect(&streamThread,SIGNAL(frameCaptured(Mat)),this,SLOT(handleCapturedFrame(Mat)));
	stop = true;
	streamThread.stopStreaming();
	while(streamThread.isRunning()){
		QThread::msleep(50);
	}
	return true;
}

// Visszaadja, hogy él-e még a kapcsolat. Sikertelen kapcsolódás esetén a szál véget ér.
bool OpencvConnection::isAlive(){
	return streamThread.isRunning();
}

// Átpasszolja a Connection és CvRtspThread között a jelet.
void OpencvConnection::passFPSChanged(int v){
	setFPS(v);
}

// Feldolgozásra továbbítja a képkockát.
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
