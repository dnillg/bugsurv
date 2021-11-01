/*
** @project			BugSurv Framework
** @file			bugsurv.h
** @author			Giczi D�niel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		A grafikus fel�letet megval�s�t� oszt�ly deklar�ci�ja.
*/

#ifndef BUGSURV_H
#define BUGSURV_H

// $ Konfigur�ci�
#include "cfg.h"

// $ Gener�lt GUI
#include "ui_bugsurv.h"

// < QT
#include <QtWidgets/QMainWindow>
#include <QMessageBox>
#include <QLayout.h>
#include <QFileDialog>
#include <QDateTime>
#include <QProcess>
#include <QDesktopServices>
#include <QShortcut>
#include <QKeySequence>
// >

// < Saj�t oszt�lyok
#include "Configurations.h"
#include "connectionClasses.h"
#include "PTZControlGV.h"
#include "CVImageWidget.h"
#include "Logger.h"
#include "About.h"
// >

// < OpenCV
#include "opencv2/opencv.hpp" 
using namespace cv;
// >

// < Alap�rtelmezett konfigur�ci�
#ifndef DEFAULT_CONNECTION_NAME
#define DEFAULT_CONNECTION_NAME "(ismeretlen kapcsolat)"
#endif
// >

#define CONNC 4 // : Maxim�lis Kapcsolatok sz�ma

class BugSurv : public QMainWindow
{
protected:
	Q_OBJECT
public:
	BugSurv(QWidget *parent = 0);
	~BugSurv();
	void closeEvent( QCloseEvent* e);
	static void CriticalErrorMessage(QString& text);
	static void CriticalErrorMessage(char* text);
private:
	enum Hotkeys {CONN1, CONN2, CONN3, CONN4, START_STOP_RECORD, TAKE_SNAP, OPEN_SNAP_FOLDER, OPEN_REC_FOLDER, EXTEND_SHRINK_GUI};
	Ui::BugSurvClass ui; // : GUI QT �ltal gener�lt GUI elemek
	// < Feldolgoz�k controljait t�rol� strukt�ra t�pusdefin�ci�ja
	typedef struct {
		QCheckBox* enabledCB; // : Feldolgoz� enged�lyez�se CheckBox
		QRadioButton* renderRB; // : Feldolgoz� "megjelen�t�s" RadioButton
		QCheckBox* recordCB; // : Feldolgoz� "felv�tel" CheckBox
		QLabel* titleLabel; // : Feldolgoz� neve Label
		QPushButton* settingsButton; // : Feldolgoz� param�ter be�ll�t�sai PushButton
		QLabel* fpsLabel; // : Feldolgoz� sebess�ge frame/sec-ben Label
		QCheckBox* detachedCB; // : Feldolgoz� megjelen�t�se k�l�n ablakban CheckBox
		QPushButton* actionButton; // : A feldolgoz�hoz csatolhat� esem�nyek ablak�hoz PushButton
	} ipListItem;
	// >
	// < GUI �rt�kei egy strukt�r�ban
	typedef struct{
		int connectionComboBoxId;
		QString connectionNameLineEditValue;
		int connectionTypeComboBoxId;
		QString ipAddressLineEditValue;
		int contactTypeComboBoxId;
		QString ChannelNumLineEditValue;
		QString passwordLineEditValue;
		QString usernameLineEditValue;
		QString controlPortLineEditValue;
		QString httpPortLineEditValue;
		QString mcIpAddressLineEditValue;
		QString mcastPortLineEditValue;
		QString regPortLineEditValue;
		QString rtpmcastaPortLineEditValue;
		QString rtpmcastvPortLineEditValue;
		QString rtspPortLineEditValue;
		QString streamingPortLineEdit;
		QString rtspAddressLineEdit;
		QString ptzFileLineEditValue;
		bool ptzFocusCheckBoxValue;
		bool ptzZoomCheckBoxValue;
		bool ptzorCheckboxValue;
		bool ptzpushtoCheckBoxValue;
		int snapFormatComboBoxId;
		int snapSizeComboBoxId;
		int videoFormatComboBoxId;
		int videoSizeComboBoxId;
		QString usbcamIndex;
		QString snapshotFolder;
		QString recordFolder;
	} GuiValues;
	// >
	GuiValues guiValues[CONNC]; // : Kapcsolatv�lt�skor ebbe menti �s ebb�l t�lti be a GUI tartalm�t
	vector<ipListItem> ipListGUI; // : A feldolgoz�k list�j�hoz haszn�lt controlstrukt�r�k t�mbje
	bool guiSetup; // : Igaz �llapotban a GUI elemek v�ltoz�sa hat�stalan lesz
	bool guiExpanded; // : A form m�ret�t t�rolja, igaz �llapotban nagyobb a form
	QLayout* ptzlay; // : PTZControlGV control fut�s k�zben l�trehozott layoutj�t t�rolja
	PTZControlGV* cControl; // : Fut�s k�zben l�trehozott PTZControlGV controlt t�rolja
	Configurations configurations; // : Kapcsolatkonfigur�ci�kat kezeli
	int connId; // : Aktu�lis kapcsolat index�t t�rolja
	Connection* connections[CONNC]; // : Kapcsolatok mutat�i egy t�mbben
	CVImageWidget* cvWidget; // : Az el�n�zethez haszn�lt Widget pointer�t t�roja
	list<pair<Hotkeys, QShortcut*>> shortcuts; // : Gyorsgomb objektumok list�ja
	Logger basicLogger; // : Napl�z� objektum
	About about;
	void initGuiValues();
	void loadConnectionsList(bool keepSelection = false);
	ActiConnection::EXT_MEDIA_CONNECTION_CONFIG getActiConnectionConfig();
	void changeConnectionType(int type);
	int aliveConnections();
	void initIPlist();
	void getProcessorsState(int idx = -1);
	void setRecordSettings(int idx = -1);
	void connectConnectionSlots();
	void disconnectConnectionSlots();
	void resetVideoInfo();
	void remoiveIPlist();
	void initResolutionSettings();
	void colorizeProcTitles();
	bool recordCheckBoxCheck();
	Configurations::ConnectionConf getConnectionConf();
	void initShortcuts();
	void removeShortcuts();
	void switchConnection(int id);
private slots:
	void connBtnClicked();
	void helpBtnClicked();
	void snapBtnClicked();
	void recBtnClicked();
	void moreBtnClicked();
	void ptzorCheckBoxStateChanged(int); 
	void ptzptCheckBoxStateChanged(int);
	void ptzZoomCheckBoxStateChanged(int);
	void ptzFocusCheckBoxStateChanged(int);
	void connectionComboBoxIndexChanged(int);
	void connectionTestBtnClicked();
	void delConnectionBtnCliecked();
	void connectionSaveBtnCliecked();
	void ptzFileBtnClicked();
	void videoFormatComboBoxIndexChanged(int);
	void snapFormatComboBoxIndexChanged(int);
	void connectionTypeComboBoxIndexChanged(int);
	void renderProcessorChosen();
	void recordProcessorStateChanged(int);
	void processorEnableStateChanged(int);
	void snapSizeComboBoxIndexChanged(int);
	void videoSizeComboBoxIndexChanged(int);
	void changeImage(Mat img);
	void changeResolution(int, int);
	void changeFPS(int);
	void recordForceStopped();
	void showProcSettingsDialog();
	void changeProcFPS(int,int);
	void detachStateChanged(int);
	void showChangeFpsDialog();
	void browseRecFolder();
	void browseSnapFolder();
	void openRecFolderBtnClicked();
	void openSnapFolderBtnClicked();
	void switchConnectionBtnClicked();
	void switchConnectionPressed();
	void appendLogBox(QString header, QString message);
	void showProcActionsDialog();
};

#endif // BUGSURV_H
