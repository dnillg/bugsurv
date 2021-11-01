/*
** @project			BugSurv Framework
** @file			opencvConnection.h
** @author			Giczi D�niel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		OpenCV-s kapcsolat oszt�ly�nak deklar�ci�ja.
*/

#ifndef OPENCVCONNECTION_H
#define OPENCVCONNECTION_H

// $ Konfigur�ci�
#include "cfg.h"

// < Saj�t oszt�lyok
#include "Connection.h"
#include "CvRtspThread.h"
// >

// < Alap�rtelmezett konfigur�ci�
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
	CvRtspThread streamThread; // : A vide�folyamot fogad� sz�l objektum
	bool stop; // : disconnect eset�n �tbillen igazra
	void setFPS(int);
protected slots:
	void handleCapturedFrame(Mat image);
	void passFPSChanged(int);
};
#endif // OPENCVCONNECTION_H
