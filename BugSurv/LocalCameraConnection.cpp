/*
** @project			BugSurv Framework
** @file			LocalCameraConnection.cpp
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Az USB-n csatlakoztatott kamerákhoz használt kapcsolattípus osztálydeklarációja.
*/

#include "LocalCameraConnection.h"

LocalCameraConnection::LocalCameraConnection(int _connID):Connection(_connID),worker(0)
{
	QObject::connect(&worker, SIGNAL(frameCaptured(Mat)), this, SLOT(passFrame(Mat)));
}

LocalCameraConnection::~LocalCameraConnection(void)
{}

bool LocalCameraConnection::isAlive(){
	for(int i = 0; i < 15; i++){
		if(worker.isAlive())
			return true;
		QThread::msleep(50);
	}
	return worker.isAlive();
}

bool LocalCameraConnection::disconnect(){
	worker.softStop();
	while(worker.isAlive());
	return true;
}

bool LocalCameraConnection::connect(void* conf, bool test){
	setCameraId(((QString*)conf)->toInt());
	worker.moveToThread(&thread);
	thread.start();
	QMetaObject::invokeMethod(&worker, "doWork", Qt::QueuedConnection);
	QThread::msleep(500);
	return true;
}

void LocalCameraConnection::setCameraId(int _id){
	worker.setCamId(_id);
}

void LocalCameraConnection::passFrame(Mat frame){
	processFrame(frame);
}

int LocalCameraConnection::getConnectionType(){
	return Connection::CVUSB;
}