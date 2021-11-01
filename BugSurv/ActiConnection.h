/*
** @project			BugSurv Framework
** @file			ActiConnection.h
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		ACTi kamerákhoz használt SDK használatához készített osztály deklarációja.
*/

#ifndef ACTICONNECTION_H
#define ACTICONNECTION_H

// $ Konfiguráció
#include "cfg.h"

// < ACTi SDK
#include "qt_windows.h"
#include "SDK10000.h"
#ifndef PTZ_MEDIA_SOURCE_RS
#define PTZ_MEDIA_SOURCE_RS		1
#endif // PTZ_MEDIA_SOURCE_RS
#ifndef PTZ_MEDIA_SOURCE_FILE
#define PTZ_MEDIA_SOURCE_FILE	2
#endif // PTZ_MEDIA_SOURCE_FILE
// >

// < Saját osztályok
#include "Connection.h"
// >

// < QT
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
// >

// < Alapértelmezett konfiguráció
#ifndef ACTI_MAXFPS
#define ACTI_MAXFPS 60
#endif
// >


class ActiConnection : public Connection
{
	Q_OBJECT
public:
	struct EXT_MEDIA_CONNECTION_CONFIG : public MEDIA_CONNECTION_CONFIG{
		string ptzFile;
	};
	ActiConnection(int _connID);
	virtual ~ActiConnection(void);
	virtual int getConnectionType();
	virtual bool connect(void* conf, bool test = false);
	virtual bool disconnect();
	virtual bool isAlive();
	virtual bool hasPTZ();
	void imageHandler( DWORD UserParam, BYTE* pBuf, DWORD len, DWORD dwWidth, DWORD dwHeight );
	void setPTZFile(QString url);
	static void CALLBACK ImageCallBack( DWORD UserParam, BYTE* pBuf, DWORD len, DWORD dwWidth, DWORD dwHeight );
protected:
	QNetworkAccessManager* networkManager; // : HTTP kérésekhez használt QT osztály
	HANDLE h; // : SDK WINAPI-s handlerje
	EXT_MEDIA_CONNECTION_CONFIG mcc; // : A kapcsolat konfigurációjához szükséges struktúra
	MEDIA_PTZ_PROTOCOL mPTZp; // : PTZ konfigurációjához szükséges struktúra
	void resetCB();
	void getFPSbyHTTP();
public slots:
	virtual void performZoom(int r);
	virtual void performPT(int direction, byte speed);
	virtual void performFocus(int);
private slots:
	void requestFinished(QNetworkReply*);
};
#endif // ACTICONNECTION_H
