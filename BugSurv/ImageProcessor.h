/*
** @project			BugSurv Framework
** @file			ImageProcessor.h
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Minden képfeldolgozó alapja. Meghatározza a feldolgozók interfészét,
		és a közös részek definiálva vannak a cpp fájlban.
*/

#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

// $ Konfiguráció
#include "cfg.h"

// < OpenCV
#include "opencv2/opencv.hpp" 
// >

// < Saját formok
#include "SettingsDialog.h"
#include "actionsdialog.h"
#include "DetachedView.h"
#include "Logger.h"
// >

// $ QT
#include <QObject>

using namespace cv;
using namespace std;

class ImageProcessor : public QObject
{	
protected:
	Q_OBJECT
public:
	// < Abszolút pozíciót leíró struktúra
	struct ptzPosition{
		double P;
		double T;
		double Z;
	};
	// >
	// < Feldolgozó adatstruktúrája
	struct ipData{
		Mat frame; // : feldolgozandó képkocka
		int ptzCommand; // : kiadott PTZ utasítás
		int ptzSpeed; // : ptz sebesség
		ptzPosition absPosition; // : abszolút pozíció
		unsigned long frameCounter;
		void* data;
	};
	// >
	virtual std::string getName();
	virtual ImageProcessor* clone() = 0;
	ImageProcessor(const string& name = "Unnamed Processor");
	ImageProcessor(const ImageProcessor& o);
	virtual ~ImageProcessor(void);
	void setEnabled(bool value);
	bool isEnabled();
	void setPriority(int p);
	int getPriority();
	void initIPdata(Mat&, const unsigned long&);
	ipData* getResults();
	void showSettings();
	void setDetached(bool);
	bool getDetached();
	void showActions();
	void setName(const string);
protected:
	typedef struct{
		QVariant::Type type; // : Paraméter típusa
		void* pointer; // : Az eredeti változóra mutató pointer
		bool editable; // : Változtatható-e GUI-n keresztül
	} Parameter;
	typedef struct{
		std::string slotName; // : SLOT neve, amely meg lesz hívva
		bool needCall; // : Következõ kör elõtt meghívandó-e
	} Action;
	DetachedView dView; // : A külön ablak (widget), amiben megjelenhet a feldolgozott kép
	QString windowName; // : A külön ablak neve
	SettingsDialog sDialog; // : A paraméterablak
	ActionsDialog aDialog;
	std::vector<QVariant> changedParams; // : Az elõzõ frame számítása óta megváltoztak a paraméterek
	std::vector<Parameter> params; // : Paraméterek
	bool detached; // : külön ablakban megjelenítés
	bool paramsChangedFlag; // : változott paraméterek
	bool enabled; // : engedélyezett-e
	int priority; // : prioritás
	ipData ipdata; // : feldolgozott adat/eredmény
	QString curLogHeader; // : naplóbejegyzésekhez írt fejléc
	std::vector<Action> actions; // : adatstruktúrák az akciógombok kezeléséhez
	QMutex paramMutex; // : paraméterek kezelését biztonságossá tevõ mutex
	QMutex initMutex; // : inicializáció egy idõben csak egy történhet
	string name; // : feldolgozó megjelenített neve

	virtual void process() = 0;
	void addParam(const std::string, int*, bool = true);
	void addParam(const std::string, double*, bool = true);
	void addParam(const std::string, bool*, bool = true);
	void addParam(const std::string, QString*, bool = true);
	void addAction(const std::string&, const std::string&);
	void updateParam(const void* const);
	void loadParams();
	void initParams();
	void initActions();
	virtual void addParams();
	virtual void addActions();
	void initHeaders();
	void writeLog(QString, QString = QStringLiteral(""));
	void runActions();
signals:
  void finished();
  void error(QString err);
  void refreshDetached(Mat img);
  void logMessageSent(QString, QString);
public slots:
	void paramsChanged();
	void performProcessing();
protected slots:
	void setAction(int);
};

#endif // IMAGEPROCESSOR_H
