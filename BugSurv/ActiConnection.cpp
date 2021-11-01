/*
** @project			BugSurv Framework
** @file			ActiConnection.cpp
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		ACTi kamerákhoz használt SDK használatához készített osztály definíciójára.
*/

#include "ActiConnection.h"
#include "Configurations.h"

ActiConnection::ActiConnection(int _connID):Connection(_connID),h(0)
{
	networkManager = new QNetworkAccessManager(this);
	QObject::connect(networkManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(requestFinished(QNetworkReply*)));
}

// FPS lekérdezõ HTTP kérések válaszának REGEXP kiértékelése.
void ActiConnection::requestFinished(QNetworkReply* reply){
	if(fps == -1){
		QString data = reply->readAll();
		QRegExp r("^VIDEO_FPS_NUM='(\\d+)'");
		if(r.indexIn(data) != -1){
			int rx_fps = r.cap(1).toInt();
			setFPS(rx_fps);
		}
	}
}

// Csatlakozás implementációja ACTI SDK-hoz (ACTI SDK10000 v1.2).
bool ActiConnection::connect(void* conf, bool test){
	// < Kapcsolat konfigurációja
	mcc = *((EXT_MEDIA_CONNECTION_CONFIG*)conf);
	mcc.SearchPortC2S	= 6005;
	mcc.SearchPortS2C	= 6006;
	mcc.ConnectTimeOut	= 5;


	h = KOpenInterface();

	if( h )
	{	
		// $ Protokoll beállítása
		KSetTCPType( h, 2 );
		KSetMediaConfig( h, &mcc ); // Konfigurációs struktúra átadása
		if(!test){
			// $ Callback függvények beállítása
			KSetImageCallback(h, (DWORD)this, ImageCallBack);
		}

		if( KConnect( h ) ) 
		{
			MEDIA_VIDEO_CONFIG VideoConfig;
			memset (&VideoConfig, 0, sizeof MEDIA_VIDEO_CONFIG);
			KGetVideoConfig( h, &VideoConfig );

			if(VideoConfig.dwFps > ACTI_MAXFPS){
					/* Nem minden kameránál adja vissza az SDK
					** a stream FPS-ét. Ha nem sikerül, akkor
					** próbát tesz HTTP lekérdezéssel.
					*/
					getFPSbyHTTP();
			} else {
				setFPS(VideoConfig.dwFps);
			}
			
			bool bstatr = KStartStreaming( h );

			if( bstatr ){
				KPlay( h );
				if(!test){
					KEnablePTZProtocol(h, true);
					//KEnableJitterLessMode(h, true);
					mPTZp.dwAddressID = mcc.ChannelNumber;
					strcpy(mPTZp.szProtocolFileName, mcc.ptzFile.c_str());
					mPTZp.nSourceType = PTZ_MEDIA_SOURCE_FILE;
					KPTZLoadProtocol(h, &mPTZp);
				}
				return true;
//<Sikertelen csatlakzoás utáni lezárás
			} else { 
				if( h ){
					resetCB();
					KStopStreaming( h );
				}
			}
		}
		KDisconnect( h );
		KCloseInterface( h );
		h = 0;
	}
//>
	return false;
}

// Acti videoszerverrõl a gyakori lekérdezõ URL-eken tesz próbát az FPS meghatározására.
void ActiConnection::getFPSbyHTTP(){
	QNetworkRequest request;
	QString url;

	url = QString("http://%1:%2@%3:%4/cgi-bin/cmd/encoder?VIDEO_FPS_NUM").arg(mcc.UserID).arg(mcc.Password).arg(mcc.UniCastIP).arg(mcc.HTTPPort);
	request.setUrl(QUrl(url));
	networkManager->get(request);

	url =QString("http://%1:%2@%3:%4/cgi-bin/cmd/system?VIDEO_FPS_NUM").arg(mcc.UserID).arg(mcc.Password).arg(mcc.UniCastIP).arg(mcc.HTTPPort);
	request.setUrl(QUrl(url));
	networkManager->get(request);

	url =QString("http://%1:%2@%3:%4/cgi-bin/system?VIDEO_FPS_NUM").arg(mcc.UserID).arg(mcc.Password).arg(mcc.UniCastIP).arg(mcc.HTTPPort);
	request.setUrl(QUrl(url));
	networkManager->get(request);

	url =QString("http://%1:%2@%3:%4/cgi-bin/encoder?VIDEO_FPS_NUM").arg(mcc.UserID).arg(mcc.Password).arg(mcc.UniCastIP).arg(mcc.HTTPPort);
	request.setUrl(QUrl(url));
	networkManager->get(request);
}

// Acti SDK HANDLE-je alapján visszaadja, hogy él-e a kapcsolat.
bool ActiConnection::isAlive(){
	return h;
}

// Visszatér az enumerate typíus ACTI-nak megfelelõ értékével.
int ActiConnection::getConnectionType(){
	return ACTI;
}

// Beállítja a PTZ-hez a fájl elérési útját (amennyiben él a kapcsolat).
void ActiConnection::setPTZFile(QString url){
	strcpy(mPTZp.szProtocolFileName, url.toStdString().c_str());
	if(isAlive()) KPTZLoadProtocol(h,&mPTZp);
}

// Acti SDK által dekódolt nyert képi információt 3 csatornás CV formátumra konvertálja, majd feldolgozásra átadja.
void ActiConnection::imageHandler( DWORD UserParam, BYTE* pBuf, DWORD len, DWORD dwWidth, DWORD dwHeight ){
	Mat img4ch = Mat(dwHeight,dwWidth,CV_8UC4, 44+pBuf);
	Mat img3ch;
	cvtColor(img4ch,img3ch,CV_BGRA2BGR);
	processFrame(img3ch);
}

// Acti SDK kezel PTZ-t, így igazat ad vissza.
bool ActiConnection::hasPTZ(){
	return true;
}

// Meghívja a megfelelõ objektum képkezelõ függvényét a kapott paraméterekkel.
void CALLBACK ActiConnection::ImageCallBack( DWORD UserParam, BYTE* pBuf, DWORD len, DWORD dwWidth, DWORD dwHeight )
{
	((ActiConnection*)UserParam)->imageHandler(UserParam, pBuf, len, dwWidth, dwHeight );
}

// Kapcsolat zárása - Igazzal tér vissza, ha zárt le kapcsolatot.
bool ActiConnection::disconnect(){
	if( h )
	{
		resetCB();
		KStopStreaming( h );
		KDisconnect( h );
		KCloseInterface( h );
		h = 0;
		return true;
	}
	return false;
}

// Callback függvények resetelése.
void ActiConnection::resetCB(){
	if(h){
		KSetDICallback( h, 0, NULL );
		KSetMotionDetectionCallback( h, 0, NULL );
		KSetResolutionChangeCallback( h, 0, NULL );
		KSetRawDataCallback(h, 0, NULL);
		KSetImageCallback(h, 0, NULL);
	}
}

// ACTi SDK Zoom funkciójának közvetett interfésze.
void ActiConnection::performZoom(int r){
	const int offset = PTZcommand::ZOOM_STOP;
	if(r != PTZcommand::ZOOM_STOP-offset)
			KPTZZoom(h, mPTZp.dwAddressID, 1,(r == PTZcommand::ZOOM_IN-offset)?PTZ_ZOOM_IN:PTZ_ZOOM_OUT);
		else
			KPTZZoom(h, mPTZp.dwAddressID, 0, PTZ_ZOOM_STOP);
}

// ACTi SDK pan-tilt funkciójának közvetett interfésze.
void ActiConnection::performPT(int direction, byte speed){
	if(direction != PTZcommand::PT_STOP && speed != 0){
		switch (direction)
		{
		case PTZcommand::PT_UP:
			KPTZMove(h, mPTZp.dwAddressID, speed, PTZ_MOVE_UP);
			break;
		case PTZcommand::PT_UPLEFT:
			KPTZMove(h, mPTZp.dwAddressID, speed, PTZ_MOVE_UP_LEFT);
			break;
		case PTZcommand::PT_LEFT:
			KPTZMove(h, mPTZp.dwAddressID, speed, PTZ_MOVE_LEFT);
			break;
		case PTZcommand::PT_DOWNLEFT:
			KPTZMove(h, mPTZp.dwAddressID, speed, PTZ_MOVE_DOWN_LEFT);
			break;
		case PTZcommand::PT_DOWN:
			KPTZMove(h, mPTZp.dwAddressID, speed, PTZ_MOVE_DOWN);
			break;
		case PTZcommand::PT_DOWNRIGHT:
			KPTZMove(h, mPTZp.dwAddressID, speed, PTZ_MOVE_DOWN_RIGHT);
			break;
		case PTZcommand::PT_RIGHT:
			KPTZMove(h, mPTZp.dwAddressID, speed, PTZ_MOVE_RIGHT);
			break;
		case PTZcommand::PT_UPRIGHT:
			KPTZMove(h, mPTZp.dwAddressID, speed, PTZ_MOVE_UP_RIGHT);
			break;
		} 
	} else {
		KPTZMove(h, mPTZp.dwAddressID, 0, PTZ_MOVE_STOP);
	}
}

// ACTi SDK fókusz funkciójának közvetett interfésze.
void ActiConnection::performFocus(int r){
	const int offset = PTZcommand::FOCUS_STOP;
	switch(r){
		case PTZcommand::FOCUS_STOP-offset:
			KPTZFocus(h,mPTZp.dwAddressID, PTZ_FOCUS_STOP);
			break;
		case PTZcommand::FOCUS_IN-offset:
			KPTZFocus(h,mPTZp.dwAddressID, PTZ_FOCUS_IN);
			break;
		case PTZcommand::FOCUS_OUT-offset:
			KPTZFocus(h,mPTZp.dwAddressID, PTZ_FOCUS_OUT);
			break;
	}
}

ActiConnection::~ActiConnection(void)
{
	if(networkManager)delete networkManager;
	disconnect();
}
