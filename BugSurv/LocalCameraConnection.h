/*
** @project			BugSurv Framework
** @file			LocalCameraConnection.h
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Az USB-n csatlakoztatott kamerákhoz használt kapcsolattípus osztálydeklarációja.
*/

#ifndef LOCALCAMERACONNECTION_H
#define LOCALCAMERACONNECTION_H

#include "connection.h"
#include "LCCThread.h"
#include "Configurations.h"

class LocalCameraConnection : public Connection{
	Q_OBJECT
public:
	LocalCameraConnection(int _connID);
	virtual ~LocalCameraConnection(void);
public:
	virtual bool isAlive();
	virtual bool connect(void*, bool test = false);
	virtual int getConnectionType();
protected:
	LCCThread worker;
	QThread thread;
	virtual bool disconnect();
public slots:
	void setCameraId(int);
	void passFrame(Mat frame);
};

#endif // LOCALCAMERACONNECTION_H
