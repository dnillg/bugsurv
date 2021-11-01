/*
** @project			BugSurv Framework
** @file			Connection.h
** @author			Giczi D�niel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Minden kapcsolatt�pus alapja. Egy absztrakt oszt�ly, nem p�ld�nyos�that�.
		A specifikus r�szeknek csak az interf�sze van megadva, m�g a k�z�s r�szek
		defini�lva vannak a cpp f�jlban.
*/

#ifndef CONNECTION_H
#define CONNECTION_H

// $ Konfigur�ci�
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

// < Saj�t oszt�lyok
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
	typedef enum { PT_STOP, PT_UP, PT_UPLEFT, PT_LEFT, PT_DOWNLEFT, PT_DOWN, PT_DOWNRIGHT, PT_RIGHT, PT_UPRIGHT, ZOOM_STOP, ZOOM_IN, ZOOM_OUT, FOCUS_STOP, FOCUS_IN, FOCUS_OUT, DONTCARE, DONOTHING, ABSPOS} PTZcommand; // : PTZ instrukci�k
	/* PT: Pan-tilt
	** ZOOM: k�zel�t�s
	** Focus: F�kusz �ll�t�sa
	** DONTCARE: A processzor nem foglalkozik vele, hogy mi t�rt�nik a PTZ-vel, ak�r alacsonyabb priorit�s� feldolgoz� PTZ m�velete is v�grehajt�dhat.
	** DONOTHING: A processzor nem k�v�n v�ltoztatni a PTZ �llapot�n �s alacsonyabb priorit�s� feldolgoz� nem sz�lhat bele.
	** ABSPOS: Abszol�t poz�ci�ba �ll�s
	*/
	typedef enum { ACTI, OPENCV , CVUSB} ConnectionTypes; // : sz�rmaztatott kapcsolatt�pusok felsorolva
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
		bool endRecord; // : felv�tel v�g�t jelz� flag (igaz: k�vetkez� frame helyett lez�rja a felv�telt)
		bool ifrecord; // : felv�telt jelz� v�ltoz� (igaz: felv�telt �runk)
		bool nf_snapshot; // : pillanatk�pet jelz� flag (igaz: k�vetkez� frame-et mentj�k)
		Mat lastframe; // : Az utols� felvett �s feldolgozott frame-t t�rolja el (esetleges duplik�ci� c�lj�b�l)
		bool frameExists; // : Van-e m�r feldolgozott frame, ami duplik�lhat�
		QMutex recordMutex; // : Felv�tel konkurenci�ihoz haszn�lt mutex
	} Recorder;
	static vector<ImageProcessor*> initIplist; // : inicializ�l�shoz sz�ks�ges processzorok
	static QString logHeader; // : a nalp�bejegyz�sek fejl�cei ezzel ker�lnek kieg�sz�t�sre
	bool recording; // : t�rt�nik-e �pp vide�felv�tel
	Recorder* recorders; // : openCV video�r� oszt�ly egy objektuma
	QString snapFormat; // : pillanatk�phez haszn�lt k�pform�tum kiterjeszt�se
	int videoFormat; // : videofelv�telhez haszn�lt form�tum
	CvSize imageSize; // : aktu�lis k�pkocka m�ret
	int fps; // : k�pkocka per m�sodperc
	bool manualPTZ; // : manu�lis PTZ enged�lyezett-e (igaz: a k�pfeldolgoz�k nem sz�lhatnak bele PTZ-be)
	int iplSize; // : feldolgoz�k t�mb m�rete
	int oImageIdx; // : az el�n�zetben megjelen�tett k�pfeldolgoz� indexe (-1: eredeti)
	int rImageIdx; // : a felv�telhez haszn�lt k�pfeldolgoz� indexe (-1: eredeti)
	double snapSizeMultiplier; // : pillanatk�p m�ret�nek szorz�ja
	double videoSizeMultiplier; // : pillanatk�pt m�ret�nek szorz�ja
	CvSize recordingSize; // : jelenlegi felv�tel k�pkockam�rete
	ImageProcessor** processors; // : az initIplistb�l m�solt feldolgoz�k objektumainak dinamikusan foglalt t�mbje
	ProcessorThread** processorThreads; // : feldolgoz�sz�lak
	ChangeFpsDialog fpsDialog; // : Manu�lis FPS �ll�t� dialog
	DetachedView defDetachedView; // : eredeti stream k�l�n ablaka
	bool defDetached; // : eredeti stream meg van-e k�l�n jelen�tve
	string snapFolder; // : pillanatk�pek k�nyvt�ra (abszol�t vagy relat�v)
	string recFolder; // : felv�telek k�nyvt�ra (abszol�t vagy relat�v)
	int connID; // : A GUI-n foglalt hely azonos�t�ja
	QString curLogHeader; // : A kapcsolat kieg�sz�tett napl� fejl�ce
	QDateTime lastPTZCommand; // : Utols� kiadott automatikus PTZ utas�t�s ideje
	QMutex frameMutex; // : K�pek fogad�s�nak k�lcs�n�s kiz�r�sa
	unsigned long counter; // : Frame sz�ml�l�
	void processFrame(Mat& frame); 
	virtual void setFPS(int value);
	void recordFrame(Mat& frame, int idx);
signals:
	void imageChanged(Mat image); // Megv�ltozott az el�n�zetben megjelen�tett k�p
	void resolutionChanged(int w,int h); // Megv�ltozott a felbont�s
	void FPSChanged(int value); // Megv�ltozott az FPS
	void recordForceStopped(); // A videofelv�tel le�ll�sra k�nyszer�lt
	void procFPSChanged(int, int); // Az (1. param�ter) index� feldolgoz� FPS-e (2. param�ter) �rt�kre v�ltozott
	void logMessageSent(QString, QString); // Napl��zenet k�ld�se
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
// Feldolgoz� hozz�ad�sa
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

