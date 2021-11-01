/*
** @project			BugSurv Framework
** @file			Connection.h
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Minden kapcsolattípus alapja. Egy absztrakt osztály, nem példányosítható.
		A specifikus részeknek csak az interfésze van megadva, míg a közös részek
		definiálva vannak a cpp fájlban.
*/

#ifndef CONNECTION_H
#define CONNECTION_H

// $ Konfiguráció
#include "cfg.h"

// $ OpenCV
#include "opencv2/opencv.hpp" 

// < QT
#include <QWidget>
#include <QDir>
#include <QDateTime>
#include <QThread>
#include <QMutex>
// >

// < Saját osztályok
#include "ipClasses.h"
#include "PTZControlGV.h"
#include "ProcessorThread.h"
#include "ChangeFpsDialog.h"
#include "Logger.h"
// >

using namespace cv;

class Connection : public QObject
{
protected:
	Q_OBJECT
public:
	typedef enum { PT_STOP, PT_UP, PT_UPLEFT, PT_LEFT, PT_DOWNLEFT, PT_DOWN, PT_DOWNRIGHT, PT_RIGHT, PT_UPRIGHT, ZOOM_STOP, ZOOM_IN, ZOOM_OUT, FOCUS_STOP, FOCUS_IN, FOCUS_OUT, DONTCARE, DONOTHING, ABSPOS} PTZcommand; // : PTZ instrukciók
	/* PT: Pan-tilt
	** ZOOM: közelítés
	** Focus: Fókusz állítása
	** DONTCARE: A processzor nem foglalkozik vele, hogy mi történik a PTZ-vel, akár alacsonyabb prioritású feldolgozó PTZ mûvelete is végrehajtódhat.
	** DONOTHING: A processzor nem kíván változtatni a PTZ állapotán és alacsonyabb prioritású feldolgozó nem szólhat bele.
	** ABSPOS: Abszolút pozícióba állás
	*/
	typedef enum { ACTI, OPENCV , CVUSB} ConnectionTypes; // : származtatott kapcsolattípusok felsorolva
	Connection(int _connID);
	virtual ~Connection(void);
	virtual int getConnectionType() = 0;
	virtual bool connect(void* conf, bool test = false) = 0;
	virtual bool disconnect() = 0;
	bool startRecord();
	bool stopRecord();
	void snapshot();
	virtual bool isAlive() = 0;
	void setSnapFormat(QString format);
	void setVideoFormat(int format);
	bool isRecording();
	CvSize getImageSize();
	template <class T> static bool addIpClass(int p);
	static bool addIpClass(ImageProcessor* ipp, int p = 0);
	virtual bool hasPTZ();
	void handlePTZ(const ImageProcessor::ipData*);
	void setOImageIdx(int idx);
	int getOImageIdx();
	void setProcRecording(int idx, bool value);
	int getProcRecording(int idx);
	static QString getIpName(int idx);
	static int ipCount();
	void setIPEnabled(int idx, bool enabled);
	bool getIPEnabled(int idx);
	void setVideoSize(double mult);
	void setSnapSize(double mult);
	int getFPS();
	static void deleteIPlist();
	void setManualPTZ(bool);
	void showProcSettingsDialog(int idx);
	void showFpsChangeDialog();
	void setIPDetached(int idx, bool val);
	bool getIPDetached(int idx);
	bool isIPRecording(int idx);
	void setRecFolder(string folder);
	void setSnapFolder(string folder);
	const QString& getLogHeader();
	void showProcActionsDialog(int idx);
protected:
	typedef struct {
		VideoWriter vWriter;
		bool endRecord; // : felvétel végét jelzõ flag (igaz: következõ frame helyett lezárja a felvételt)
		bool ifrecord; // : felvételt jelzõ változó (igaz: felvételt írunk)
		bool nf_snapshot; // : pillanatképet jelzõ flag (igaz: következõ frame-et mentjük)
		Mat lastframe; // : Az utolsó felvett és feldolgozott frame-t tárolja el (esetleges duplikáció céljából)
		bool frameExists; // : Van-e már feldolgozott frame, ami duplikálható
		QMutex recordMutex; // : Felvétel konkurenciáihoz használt mutex
	} Recorder;
	static vector<ImageProcessor*> initIplist; // : inicializáláshoz szükséges processzorok
	static QString logHeader; // : a nalpóbejegyzések fejlécei ezzel kerülnek kiegészítésre
	bool recording; // : történik-e épp videófelvétel
	Recorder* recorders; // : openCV videoíró osztály egy objektuma
	QString snapFormat; // : pillanatképhez használt képformátum kiterjesztése
	int videoFormat; // : videofelvételhez használt formátum
	CvSize imageSize; // : aktuális képkocka méret
	int fps; // : képkocka per másodperc
	bool manualPTZ; // : manuális PTZ engedélyezett-e (igaz: a képfeldolgozók nem szólhatnak bele PTZ-be)
	int iplSize; // : feldolgozók tömb mérete
	int oImageIdx; // : az elõnézetben megjelenített képfeldolgozó indexe (-1: eredeti)
	int rImageIdx; // : a felvételhez használt képfeldolgozó indexe (-1: eredeti)
	double snapSizeMultiplier; // : pillanatkép méretének szorzója
	double videoSizeMultiplier; // : pillanatképt méretének szorzója
	CvSize recordingSize; // : jelenlegi felvétel képkockamérete
	ImageProcessor** processors; // : az initIplistbõl másolt feldolgozók objektumainak dinamikusan foglalt tömbje
	ProcessorThread** processorThreads; // : feldolgozószálak
	ChangeFpsDialog fpsDialog; // : Manuális FPS állító dialog
	DetachedView defDetachedView; // : eredeti stream külön ablaka
	bool defDetached; // : eredeti stream meg van-e külön jelenítve
	string snapFolder; // : pillanatképek könyvtára (abszolút vagy relatív)
	string recFolder; // : felvételek könyvtára (abszolút vagy relatív)
	int connID; // : A GUI-n foglalt hely azonosítója
	QString curLogHeader; // : A kapcsolat kiegészített napló fejléce
	QDateTime lastPTZCommand; // : Utolsó kiadott automatikus PTZ utasítás ideje
	QMutex frameMutex; // : Képek fogadásának kölcsönös kizárása
	unsigned long counter; // : Frame számláló
	void processFrame(Mat& frame); 
	virtual void setFPS(int value);
	void recordFrame(Mat& frame, int idx);
signals:
	void imageChanged(Mat image); // Megváltozott az elõnézetben megjelenített kép
	void resolutionChanged(int w,int h); // Megváltozott a felbontás
	void FPSChanged(int value); // Megváltozott az FPS
	void recordForceStopped(); // A videofelvétel leállásra kényszerült
	void procFPSChanged(int, int); // Az (1. paraméter) indexû feldolgozó FPS-e (2. paraméter) értékre változott
	void logMessageSent(QString, QString); // Naplóüzenet küldése
protected slots:
	virtual void performZoom(const int r);
	virtual void performPT(const int direction, const byte speed);
	virtual void performFocus(const int);
	virtual void performAbsolute(const double P, const double T, const double Z);
	void handleResult(ImageProcessor* ipp, ImageProcessor::ipData ipdata);
	void passProcFPSChanged(int);
	void setManualFPS(int);
	void writeLog(QString, QString = QStringLiteral(""));
};

template <class T>
// Feldolgozó hozzáadása
bool Connection::addIpClass(int p = 0){
	T* object = new T();
	ImageProcessor* ipp;
	if((ipp = dynamic_cast<ImageProcessor*>(object))){
		ipp->setPriority(p);
		initIplist.push_back(ipp);
		return true;
	} else {
		delete object;
		return false;
	}
}

#endif // CONNECTION_H

