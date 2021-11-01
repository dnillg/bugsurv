/*
** @project			BugSurv Framework
** @file			opencvConnection.h
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		OpenCV-s kapcsolat osztályának deklarációja.
*/

#ifndef OPENCVCONNECTION_H
#define OPENCVCONNECTION_H

// $ Konfiguráció
#include "cfg.h"

// < Saját osztályok
#include "Connection.h"
#include "CvRtspThread.h"
// >

// < Alapértelmezett konfiguráció
#ifndef CVRTSP_TIMEOUT
#define CVRTSP_TIMEOUT 3000
#endif
// >

class OpencvConnection : public Connection
{
	Q_OBJECT
public:
	OpencvConnection(int _connID);
	virtual ~OpencvConnection(void);
	virtual int getConnectionType();
	virtual bool connect(void* conf, bool test = false);
	virtual bool disconnect();
	virtual bool isAlive();
protected:
	CvRtspThread streamThread; // : A videófolyamot fogadó szál objektum
	bool stop; // : disconnect esetén átbillen igazra
	void setFPS(int);
protected slots:
	void handleCapturedFrame(Mat image);
	void passFPSChanged(int);
};
#endif // OPENCVCONNECTION_H
