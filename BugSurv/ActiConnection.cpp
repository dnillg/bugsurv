/*
** @project			BugSurv Framework
** @file			ActiConnection.cpp
** @author			Giczi D�niel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		ACTi kamer�khoz haszn�lt SDK haszn�lat�hoz k�sz�tett oszt�ly defin�ci�j�ra.
*/

#include "ActiConnection.h"
#include "Configurations.h"

ActiConnection::ActiConnection(int _connID):Connection(_connID),h(0)
{
	networkManager = new QNetworkAccessManager(this);
	QObject::connect(networkManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(requestFinished(QNetworkReply*)));
}

// FPS lek�rdez� HTTP k�r�sek v�lasz�nak REGEXP ki�rt�kel�se.
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

// Csatlakoz�s implement�ci�ja ACTI SDK-hoz (ACTI SDK10000 v1.2).
bool ActiConnection::connect(void* conf, bool test){
	// < Kapcsolat konfigur�ci�ja
	mcc = *((EXT_MEDIA_CONNECTION_CONFIG*)conf);
	mcc.SearchPortC2S	= 6005;
	mcc.SearchPortS2C	= 6006;
	mcc.ConnectTimeOut	= 5;


	h = KOpenInterface();

	if( h )
	{	
		// $ Protokoll be�ll�t�sa
		KSetTCPType( h, 2 );
		KSetMediaConfig( h, &mcc ); // Konfigur�ci�s strukt�ra �tad�sa
		if(!test){
			// $ Callback f�ggv�nyek be�ll�t�sa
			KSetImageCallback(h, (DWORD)this, ImageCallBack);
		}

		if( KConnect( h ) ) 
		{
			MEDIA_VIDEO_CONFIG VideoConfig;
			memset (&VideoConfig, 0, sizeof MEDIA_VIDEO_CONFIG);
			KGetVideoConfig( h, &VideoConfig );

			if(VideoConfig.dwFps > ACTI_MAXFPS){
					/* Nem minden kamer�n�l adja vissza az SDK
					** a stream FPS-�t. Ha nem siker�l, akkor
					** pr�b�t tesz HTTP lek�rdez�ssel.
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
//<Sikertelen csatlakzo�s ut�ni lez�r�s
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

// Acti videoszerverr�l a gyakori lek�rdez� URL-eken tesz pr�b�t az FPS meghat�roz�s�ra.
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

// Acti SDK HANDLE-je alapj�n visszaadja, hogy �l-e a kapcsolat.
bool ActiConnection::isAlive(){
	return h;
}

// Visszat�r az enumerate typ�us ACTI-nak megfelel� �rt�k�vel.
int ActiConnection::getConnectionType(){
	return ACTI;
}

// Be�ll�tja a PTZ-hez a f�jl el�r�si �tj�t (amennyiben �l a kapcsolat).
void ActiConnection::setPTZFile(QString url){
	strcpy(mPTZp.szProtocolFileName, url.toStdString().c_str());
	if(isAlive()) KPTZLoadProtocol(h,&mPTZp);
}

// Acti SDK �ltal dek�dolt nyert k�pi inform�ci�t 3 csatorn�s CV form�tumra konvert�lja, majd feldolgoz�sra �tadja.
void ActiConnection::imageHandler( DWORD UserParam, BYTE* pBuf, DWORD len, DWORD dwWidth, DWORD dwHeight ){
	Mat img4ch = Mat(dwHeight,dwWidth,CV_8UC4, 44+pBuf);
	Mat img3ch;
	cvtColor(img4ch,img3ch,CV_BGRA2BGR);
	processFrame(img3ch);
}

// Acti SDK kezel PTZ-t, �gy igazat ad vissza.
bool ActiConnection::hasPTZ(){
	return true;
}

// Megh�vja a megfelel� objektum k�pkezel� f�ggv�ny�t a kapott param�terekkel.
void CALLBACK ActiConnection::ImageCallBack( DWORD UserParam, BYTE* pBuf, DWORD len, DWORD dwWidth, DWORD dwHeight )
{
	((ActiConnection*)UserParam)->imageHandler(UserParam, pBuf, len, dwWidth, dwHeight );
}

// Kapcsolat z�r�sa - Igazzal t�r vissza, ha z�rt le kapcsolatot.
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

// Callback f�ggv�nyek resetel�se.
void ActiConnection::resetCB(){
	if(h){
		KSetDICallback( h, 0, NULL );
		KSetMotionDetectionCallback( h, 0, NULL );
		KSetResolutionChangeCallback( h, 0, NULL );
		KSetRawDataCallback(h, 0, NULL);
		KSetImageCallback(h, 0, NULL);
	}
}

// ACTi SDK Zoom funkci�j�nak k�zvetett interf�sze.
void ActiConnection::performZoom(int r){
	const int offset = PTZcommand::ZOOM_STOP;
	if(r != PTZcommand::ZOOM_STOP-offset)
			KPTZZoom(h, mPTZp.dwAddressID, 1,(r == PTZcommand::ZOOM_IN-offset)?PTZ_ZOOM_IN:PTZ_ZOOM_OUT);
		else
			KPTZZoom(h, mPTZp.dwAddressID, 0, PTZ_ZOOM_STOP);
}

// ACTi SDK pan-tilt funkci�j�nak k�zvetett interf�sze.
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

// ACTi SDK f�kusz funkci�j�nak k�zvetett interf�sze.
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
