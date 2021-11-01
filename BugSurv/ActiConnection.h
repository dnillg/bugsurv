/*
** @project			BugSurv Framework
** @file			ActiConnection.h
** @author			Giczi D�niel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		ACTi kamer�khoz haszn�lt SDK haszn�lat�hoz k�sz�tett oszt�ly deklar�ci�ja.
*/

#ifndef ACTICONNECTION_H
#define ACTICONNECTION_H

// $ Konfigur�ci�
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

// < Saj�t oszt�lyok
#include "Connection.h"
// >

// < QT
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
// >

// < Alap�rtelmezett konfigur�ci�
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
	QNetworkAccessManager* networkManager; // : HTTP k�r�sekhez haszn�lt QT oszt�ly
	HANDLE h; // : SDK WINAPI-s handlerje
	EXT_MEDIA_CONNECTION_CONFIG mcc; // : A kapcsolat konfigur�ci�j�hoz sz�ks�ges strukt�ra
	MEDIA_PTZ_PROTOCOL mPTZp; // : PTZ konfigur�ci�j�hoz sz�ks�ges strukt�ra
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
