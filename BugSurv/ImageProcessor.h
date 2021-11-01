/*
** @project			BugSurv Framework
** @file			ImageProcessor.h
** @author			Giczi D�niel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Minden k�pfeldolgoz� alapja. Meghat�rozza a feldolgoz�k interf�sz�t,
		�s a k�z�s r�szek defini�lva vannak a cpp f�jlban.
*/

#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

// $ Konfigur�ci�
#include "cfg.h"

// < OpenCV
#include "opencv2/opencv.hpp" 
// >

// < Saj�t formok
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
	// < Abszol�t poz�ci�t le�r� strukt�ra
	struct ptzPosition{
		double P;
		double T;
		double Z;
	};
	// >
	// < Feldolgoz� adatstrukt�r�ja
	struct ipData{
		Mat frame; // : feldolgozand� k�pkocka
		int ptzCommand; // : kiadott PTZ utas�t�s
		int ptzSpeed; // : ptz sebess�g
		ptzPosition absPosition; // : abszol�t poz�ci�
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
		QVariant::Type type; // : Param�ter t�pusa
		void* pointer; // : Az eredeti v�ltoz�ra mutat� pointer
		bool editable; // : V�ltoztathat�-e GUI-n kereszt�l
	} Parameter;
	typedef struct{
		std::string slotName; // : SLOT neve, amely meg lesz h�vva
		bool needCall; // : K�vetkez� k�r el�tt megh�vand�-e
	} Action;
	DetachedView dView; // : A k�l�n ablak (widget), amiben megjelenhet a feldolgozott k�p
	QString windowName; // : A k�l�n ablak neve
	SettingsDialog sDialog; // : A param�terablak
	ActionsDialog aDialog;
	std::vector<QVariant> changedParams; // : Az el�z� frame sz�m�t�sa �ta megv�ltoztak a param�terek
	std::vector<Parameter> params; // : Param�terek
	bool detached; // : k�l�n ablakban megjelen�t�s
	bool paramsChangedFlag; // : v�ltozott param�terek
	bool enabled; // : enged�lyezett-e
	int priority; // : priorit�s
	ipData ipdata; // : feldolgozott adat/eredm�ny
	QString curLogHeader; // : napl�bejegyz�sekhez �rt fejl�c
	std::vector<Action> actions; // : adatstrukt�r�k az akci�gombok kezel�s�hez
	QMutex paramMutex; // : param�terek kezel�s�t biztons�goss� tev� mutex
	QMutex initMutex; // : inicializ�ci� egy id�ben csak egy t�rt�nhet
	string name; // : feldolgoz� megjelen�tett neve

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
