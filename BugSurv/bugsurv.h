/*
** @project			BugSurv Framework
** @file			bugsurv.h
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		A grafikus felületet megvalósító osztály deklarációja.
*/

#ifndef BUGSURV_H
#define BUGSURV_H

// $ Konfiguráció
#include "cfg.h"

// $ Generált GUI
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

// < Saját osztályok
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

// < Alapértelmezett konfiguráció
#ifndef DEFAULT_CONNECTION_NAME
#define DEFAULT_CONNECTION_NAME "(ismeretlen kapcsolat)"
#endif
// >

#define CONNC 4 // : Maximális Kapcsolatok száma

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
	Ui::BugSurvClass ui; // : GUI QT által generált GUI elemek
	// < Feldolgozók controljait tároló struktúra típusdefiníciója
	typedef struct {
		QCheckBox* enabledCB; // : Feldolgozó engedélyezése CheckBox
		QRadioButton* renderRB; // : Feldolgozó "megjelenítés" RadioButton
		QCheckBox* recordCB; // : Feldolgozó "felvétel" CheckBox
		QLabel* titleLabel; // : Feldolgozó neve Label
		QPushButton* settingsButton; // : Feldolgozó paraméter beállításai PushButton
		QLabel* fpsLabel; // : Feldolgozó sebessége frame/sec-ben Label
		QCheckBox* detachedCB; // : Feldolgozó megjelenítése külön ablakban CheckBox
		QPushButton* actionButton; // : A feldolgozóhoz csatolható események ablakához PushButton
	} ipListItem;
	// >
	// < GUI értékei egy struktúrában
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
	GuiValues guiValues[CONNC]; // : Kapcsolatváltáskor ebbe menti és ebbõl tölti be a GUI tartalmát
	vector<ipListItem> ipListGUI; // : A feldolgozók listájához használt controlstruktúrák tömbje
	bool guiSetup; // : Igaz állapotban a GUI elemek változása hatástalan lesz
	bool guiExpanded; // : A form méretét tárolja, igaz állapotban nagyobb a form
	QLayout* ptzlay; // : PTZControlGV control futás közben létrehozott layoutját tárolja
	PTZControlGV* cControl; // : Futás közben létrehozott PTZControlGV controlt tárolja
	Configurations configurations; // : Kapcsolatkonfigurációkat kezeli
	int connId; // : Aktuális kapcsolat indexét tárolja
	Connection* connections[CONNC]; // : Kapcsolatok mutatói egy tömbben
	CVImageWidget* cvWidget; // : Az elõnézethez használt Widget pointerét tároja
	list<pair<Hotkeys, QShortcut*>> shortcuts; // : Gyorsgomb objektumok listája
	Logger basicLogger; // : Naplózó objektum
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
