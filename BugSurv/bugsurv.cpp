/*
** @project			BugSurv Framework
** @file			bugsurv.cpp
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		A főablak grafikus felületét megvalósító osztály definíciója.
*/

#include "bugsurv.h"
#include "ChangeFpsDialog.h"
#include <opencv2\opencv.hpp>

BugSurv::BugSurv(QWidget *parent)
	: QMainWindow(parent)
	, guiExpanded(true)
	, connId(0)
	, guiSetup(false)
	, basicLogger("log.txt")
{
	// $ Generált GUI inicializáció
	ui.setupUi(this);

	// < Kapcsolatmutatók nullázása
	for(int idxA = 0; idxA < CONNC; idxA++){
		connections[idxA] = 0; 
	}
	//>
	
	// < Kezdőképernyő megjelenítése
	cvWidget = new CVImageWidget(ui.widget);
	cvWidget->setFixedHeight(576);
	cvWidget->setFixedWidth(720);
	cvWidget->setOverrideEnabled(true); 
	QImage img = QImage(":/files/Resources/startup.png");
	cvWidget->showImage(img);
	// >

	// < PTZ joy hozzáadása
	ptzlay = new QVBoxLayout();
	ptzlay->setAlignment(Qt::AlignTop);
	cControl = new PTZControlGV();
	ptzlay->addWidget(cControl);
	ptzlay->setMargin(14);
	ui.ptzWidget->setLayout(ptzlay);
	// >

	// < GUI slotok csatlakoztatása a komponensek jeleihez
	QObject::connect(ui.connBtn, SIGNAL(clicked()), SLOT ( connBtnClicked() ) );
	QObject::connect(ui.helpBtn, SIGNAL(clicked()), SLOT ( helpBtnClicked() ) );
	QObject::connect(ui.snapBtn, SIGNAL(clicked()), SLOT ( snapBtnClicked() ) );
	QObject::connect(ui.recBtn, SIGNAL(clicked()), SLOT ( recBtnClicked() ) );
	QObject::connect(ui.moreBtn, SIGNAL(clicked()), SLOT ( moreBtnClicked() ) );
	QObject::connect(ui.ptzorCheckbox, SIGNAL(stateChanged(int)),SLOT(ptzorCheckBoxStateChanged(int)));
	QObject::connect(ui.ptzpushtoCheckBox, SIGNAL(stateChanged(int)),SLOT(ptzptCheckBoxStateChanged(int)));
	QObject::connect(ui.ptzZoomCheckBox, SIGNAL(stateChanged(int)),SLOT(ptzZoomCheckBoxStateChanged(int)));
	QObject::connect(ui.ptzFocusCheckBox, SIGNAL(stateChanged(int)),SLOT(ptzFocusCheckBoxStateChanged(int)));
	QObject::connect(ui.connectionComboBox, SIGNAL(currentIndexChanged(int)), SLOT(connectionComboBoxIndexChanged(int)));
	QObject::connect(ui.actiTestBtn, SIGNAL(clicked()), SLOT ( connectionTestBtnClicked() ) );
	QObject::connect(ui.opencvTestBtn, SIGNAL(clicked()), SLOT ( connectionTestBtnClicked() ) );
	QObject::connect(ui.usbCvTestBtn, SIGNAL(clicked()), SLOT ( connectionTestBtnClicked() ) );
	QObject::connect(ui.delConnectionBtn, SIGNAL(clicked()), SLOT ( delConnectionBtnCliecked() ) );
	QObject::connect(ui.connectionSaveBtn, SIGNAL(clicked()), SLOT ( connectionSaveBtnCliecked() ) );
	QObject::connect(ui.ptzFileBtn, SIGNAL(clicked()), SLOT ( ptzFileBtnClicked() ) );
	QObject::connect(ui.videoFormatComboBox, SIGNAL(currentIndexChanged(int)), SLOT(videoFormatComboBoxIndexChanged(int)));
	QObject::connect(ui.snapFormatComboBox, SIGNAL(currentIndexChanged(int)), SLOT(snapFormatComboBoxIndexChanged(int)));
	QObject::connect(ui.connectionTypeComboBox, SIGNAL(currentIndexChanged(int)), SLOT(connectionTypeComboBoxIndexChanged(int)));
	QObject::connect(ui.snapSizeComboBox, SIGNAL(currentIndexChanged(int)), SLOT(snapSizeComboBoxIndexChanged(int)));
	QObject::connect(ui.videoSizeComboBox, SIGNAL(currentIndexChanged(int)), SLOT(videoSizeComboBoxIndexChanged(int)));
	QObject::connect(ui.manualFpsBtn, SIGNAL(clicked()), SLOT(showChangeFpsDialog()));
	QObject::connect(ui.switchBtn0, SIGNAL(clicked()), SLOT(switchConnectionBtnClicked()));
	QObject::connect(ui.switchBtn1, SIGNAL(clicked()), SLOT(switchConnectionBtnClicked()));
	QObject::connect(ui.switchBtn2, SIGNAL(clicked()), SLOT(switchConnectionBtnClicked()));
	QObject::connect(ui.switchBtn3, SIGNAL(clicked()), SLOT(switchConnectionBtnClicked()));
	QObject::connect(ui.defProcDetachCB, SIGNAL(stateChanged(int)), SLOT(detachStateChanged(int)));
	QObject::connect(ui.defProcRecordCB, SIGNAL(stateChanged(int)),SLOT(recordProcessorStateChanged(int)));
	QObject::connect(ui.brwSnapFolderbtn, SIGNAL(clicked()), SLOT(browseSnapFolder()));
	QObject::connect(ui.brwRecFolderbtn, SIGNAL(clicked()), SLOT(browseRecFolder()));
	QObject::connect(ui.openRecFolderBtn, SIGNAL(clicked()), SLOT(openRecFolderBtnClicked()));
	QObject::connect(ui.openSnapFolderBtn, SIGNAL(clicked()), SLOT(openSnapFolderBtnClicked()));
	QObject::connect(&basicLogger, SIGNAL(lineWritten(QString, QString)), SLOT(appendLogBox(QString, QString)));
	// >

	// $ Állapotsáv kezdőszövege
	ui.statusBar->showMessage("Állíts be egy kapcsolatot, majd kattints a kapcsolódás gombra!", 0);

	// < ACTi kapcsolatfajták
	ui.contactTypeComboBox->addItem("UNICAST WOC PREVIEW", CONTACT_TYPE_UNICAST_WOC_PREVIEW);
	ui.contactTypeComboBox->addItem("MULTICAST WOC PREVIEW", CONTACT_TYPE_MULTICAST_WOC_PREVIEW);
	ui.contactTypeComboBox->addItem("CONTROL ONLY", CONTACT_TYPE_CONTROL_ONLY);
	ui.contactTypeComboBox->addItem("UNICAST PREVIEW", CONTACT_TYPE_UNICAST_PREVIEW);
	ui.contactTypeComboBox->addItem("MULTICAST PREVIEW", CONTACT_TYPE_MULTICAST_PREVIEW);
	ui.contactTypeComboBox->setCurrentIndex(3);
	// >

	// < Képrögzítési formátumok (pillanatkép)
	ui.snapFormatComboBox->addItem("JPG", "jpg");
	ui.snapFormatComboBox->addItem("PNG", "png");
	ui.snapFormatComboBox->addItem("BMP", "bmp");
	ui.videoFormatComboBox->setCurrentIndex(0);
	// >

	// < Képrögzítési formátumok (videófelvétel)
	ui.videoFormatComboBox->addItem("MPEG-1", CV_FOURCC('P','I','M','1'));
	ui.videoFormatComboBox->addItem("MPEG-4.2", CV_FOURCC('M','P','4','2'));
	ui.videoFormatComboBox->addItem("MPEG-4.3 (ajánlott)", CV_FOURCC('D','I','V','3'));
	ui.videoFormatComboBox->addItem("MPEG-4", CV_FOURCC('D','I','V','X'));
	ui.videoFormatComboBox->addItem("H263", CV_FOURCC('H','2','6','3'));
	ui.videoFormatComboBox->addItem("H263I", CV_FOURCC('I','2','6','3'));
	ui.videoFormatComboBox->addItem("FLV1", CV_FOURCC('F','L','V','1'));
	ui.videoFormatComboBox->setCurrentIndex(2);
	// >

	// < Kapcsolattípusok
	ui.connectionTypeComboBox->addItem("ACTi SDK 10000 1.2", Connection::ACTI);
	ui.connectionTypeComboBox->addItem("RTP/RTSP/FILE (OpenCV)", Connection::OPENCV);
	ui.connectionTypeComboBox->addItem("USB kamera (OpenCV)", Connection::CVUSB);
	// >

	initResolutionSettings();
	loadConnectionsList();
	initIPlist();
	initGuiValues();
	initShortcuts();

	basicLogger.messageSent(QString::fromStdString("BugSurv keretrendszer elindult."), QString::fromStdString(LOG_DEFAULT_HEADER));
}

// Létrehozza a felületen használható gyorsgombokat
void BugSurv::initShortcuts(){
	shortcuts.push_back(pair<Hotkeys, QShortcut*>(Hotkeys::EXTEND_SHRINK_GUI, new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Plus), this, SLOT(moreBtnClicked()))));
	shortcuts.push_back(pair<Hotkeys, QShortcut*>(Hotkeys::OPEN_REC_FOLDER, new QShortcut(QKeySequence(Qt::ALT + Qt::Key_PageUp), this, SLOT(openRecFolderBtnClicked()))));
	shortcuts.push_back(pair<Hotkeys, QShortcut*>(Hotkeys::OPEN_SNAP_FOLDER, new QShortcut(QKeySequence(Qt::ALT + Qt::Key_PageDown), this, SLOT(openSnapFolderBtnClicked()))));
	shortcuts.push_back(pair<Hotkeys, QShortcut*>(Hotkeys::START_STOP_RECORD, new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_PageUp), this, SLOT(recBtnClicked()))));
	shortcuts.push_back(pair<Hotkeys, QShortcut*>(Hotkeys::TAKE_SNAP, new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_PageDown), this, SLOT(snapBtnClicked()))));

	shortcuts.push_back(pair<Hotkeys, QShortcut*>(Hotkeys::CONN1, new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_1), this, SLOT(switchConnectionPressed()))));
	shortcuts.push_back(pair<Hotkeys, QShortcut*>(Hotkeys::CONN2, new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_2), this, SLOT(switchConnectionPressed()))));
	shortcuts.push_back(pair<Hotkeys, QShortcut*>(Hotkeys::CONN3, new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_3), this, SLOT(switchConnectionPressed()))));
	shortcuts.push_back(pair<Hotkeys, QShortcut*>(Hotkeys::CONN4, new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_4), this, SLOT(switchConnectionPressed()))));
}

// A dinamikusan lefoglalt gyorsgomb objektumokat szabadítja fel
void BugSurv::removeShortcuts(){
	for(list<pair<Hotkeys, QShortcut*>>::iterator it = shortcuts.begin(); it != shortcuts.end(); it++){
		delete it->second;
	}
}

// A GUI elemek alapértelmezett értékeinek eltárolása a kapcsolatváltások kezeléséhez.
void BugSurv::initGuiValues(){
	for(int idxA = 0; idxA < CONNC; idxA++){
		guiValues[idxA].ChannelNumLineEditValue = ui.ChannelNumLineEdit->text();
		guiValues[idxA].connectionComboBoxId = ui.connectionComboBox->currentIndex();
		guiValues[idxA].connectionNameLineEditValue = ui.connectionNameLineEdit->text();
		guiValues[idxA].connectionTypeComboBoxId = ui.connectionTypeComboBox->currentIndex();
		guiValues[idxA].contactTypeComboBoxId = ui.contactTypeComboBox->currentIndex();
		guiValues[idxA].controlPortLineEditValue = ui.controlPortLineEdit->text();
		guiValues[idxA].httpPortLineEditValue = ui.httpPortLineEdit->text();
		guiValues[idxA].ipAddressLineEditValue = ui.ipAddressLineEdit->text();
		guiValues[idxA].mcastPortLineEditValue = ui.mcastPortLineEdit->text();
		guiValues[idxA].mcIpAddressLineEditValue = ui.mcIpAddressLineEdit->text();
		guiValues[idxA].passwordLineEditValue = ui.passwordLineEdit->text();
		guiValues[idxA].ptzFileLineEditValue = ui.ptzFileLineEdit->text();
		guiValues[idxA].ptzFocusCheckBoxValue = ui.ptzFocusCheckBox->isChecked();
		guiValues[idxA].ptzorCheckboxValue = ui.ptzorCheckbox->isChecked();
		guiValues[idxA].ptzpushtoCheckBoxValue = ui.ptzpushtoCheckBox->isChecked();
		guiValues[idxA].ptzZoomCheckBoxValue = ui.ptzZoomCheckBox->isChecked();
		guiValues[idxA].regPortLineEditValue = ui.regPortLineEdit->text();
		guiValues[idxA].rtpmcastaPortLineEditValue = ui.rtpmcastaPortLineEdit->text();
		guiValues[idxA].rtpmcastvPortLineEditValue = ui.rtpmcastvPortLineEdit->text();
		guiValues[idxA].rtspAddressLineEdit = ui.rtspAddressLineEdit->text();
		guiValues[idxA].rtspPortLineEditValue = ui.rtspPortLineEdit->text();
		guiValues[idxA].snapFormatComboBoxId = ui.snapFormatComboBox->currentIndex();
		guiValues[idxA].streamingPortLineEdit = ui.streamingPortLineEdit->text();
		guiValues[idxA].usernameLineEditValue = ui.usernameLineEdit->text();
		guiValues[idxA].videoFormatComboBoxId = ui.videoFormatComboBox->currentIndex();
		guiValues[idxA].videoSizeComboBoxId = ui.videoSizeComboBox->currentIndex();
		guiValues[idxA].snapSizeComboBoxId = ui.snapSizeComboBox->currentIndex();
		guiValues[idxA].usbcamIndex = ui.usbCvIndexLineEdit->text();
		guiValues[idxA].snapshotFolder = ui.snapFolderLineEdit->text();
		guiValues[idxA].recordFolder = ui.recFolderLineEdit->text();
	}
}

// Megnyitja az aktuális kapcsolathoz tartozó FPS állító dialogot
void BugSurv::showChangeFpsDialog(){
	if(connections[connId]){
		connections[connId]->showFpsChangeDialog();
	} else {
		QMessageBox msg;
		msg.setText("Az FPS állítása csak kapcsolódás után lehetséges!");
		msg.setWindowIcon(QIcon(":/files/Resources/ikon.png"));
		msg.setIcon(QMessageBox::Icon::Information);
		msg.exec();
	}
}

// Kapcsolatváltás gomb újtán
void BugSurv::switchConnectionBtnClicked(){
	int id = ((QPushButton*)QObject::sender())->text().toInt()-1;
	switchConnection(id);
}

void BugSurv::switchConnectionPressed(){
	QObject* sender = QObject::sender();
	int key = -1;
	for(list<pair<Hotkeys, QShortcut*>>::iterator it = shortcuts.begin(); it != shortcuts.end(); it++){
		if(sender == it->second)
			key = it->first;
	}

	switch (key)
	{
	case Hotkeys::CONN1:
		switchConnection(0);
		break;
	case Hotkeys::CONN2:
		switchConnection(1);
		break;
	case Hotkeys::CONN3:
		switchConnection(2);
		break;
	case Hotkeys::CONN4:
		switchConnection(3);
		break;
	default:
		break;
	}
}

// GUI elemek átállítása a másik kapcsolaton beállítottakra.
void BugSurv::switchConnection(int id){
	disconnectConnectionSlots();
	// < Widgetek értékeinek mentése
	guiValues[connId].ChannelNumLineEditValue = ui.ChannelNumLineEdit->text();
	guiValues[connId].connectionComboBoxId = ui.connectionComboBox->currentIndex();
	guiValues[connId].connectionNameLineEditValue = ui.connectionNameLineEdit->text();
	guiValues[connId].connectionTypeComboBoxId = ui.connectionTypeComboBox->currentIndex();
	guiValues[connId].contactTypeComboBoxId = ui.contactTypeComboBox->currentIndex();
	guiValues[connId].controlPortLineEditValue = ui.controlPortLineEdit->text();
	guiValues[connId].httpPortLineEditValue = ui.httpPortLineEdit->text();
	guiValues[connId].ipAddressLineEditValue = ui.ipAddressLineEdit->text();
	guiValues[connId].mcastPortLineEditValue = ui.mcastPortLineEdit->text();
	guiValues[connId].mcIpAddressLineEditValue = ui.mcIpAddressLineEdit->text();
	guiValues[connId].passwordLineEditValue = ui.passwordLineEdit->text();
	guiValues[connId].ptzFileLineEditValue = ui.ptzFileLineEdit->text();
	guiValues[connId].ptzFocusCheckBoxValue = ui.ptzFocusCheckBox->isChecked();
	guiValues[connId].ptzorCheckboxValue = ui.ptzorCheckbox->isChecked();
	guiValues[connId].ptzpushtoCheckBoxValue = ui.ptzpushtoCheckBox->isChecked();
	guiValues[connId].ptzZoomCheckBoxValue = ui.ptzZoomCheckBox->isChecked();
	guiValues[connId].regPortLineEditValue = ui.regPortLineEdit->text();
	guiValues[connId].rtpmcastaPortLineEditValue = ui.rtpmcastaPortLineEdit->text();
	guiValues[connId].rtpmcastvPortLineEditValue = ui.rtpmcastvPortLineEdit->text();
	guiValues[connId].rtspAddressLineEdit = ui.rtspAddressLineEdit->text();
	guiValues[connId].rtspPortLineEditValue = ui.rtspPortLineEdit->text();
	guiValues[connId].snapFormatComboBoxId = ui.snapFormatComboBox->currentIndex();
	guiValues[connId].streamingPortLineEdit = ui.streamingPortLineEdit->text();
	guiValues[connId].usernameLineEditValue = ui.usernameLineEdit->text();
	guiValues[connId].videoFormatComboBoxId = ui.videoFormatComboBox->currentIndex();
	guiValues[connId].videoSizeComboBoxId = ui.videoSizeComboBox->currentIndex();
	guiValues[connId].snapSizeComboBoxId = ui.snapSizeComboBox->currentIndex();
	guiValues[connId].usbcamIndex = ui.usbCvIndexLineEdit->text();
	guiValues[connId].recordFolder = ui.recFolderLineEdit->text();
	guiValues[connId].snapshotFolder = ui.snapFolderLineEdit->text();
	// >

	guiSetup = true;
	connId = id;

	// < Widgetek értékeinek betöltése
	ui.ChannelNumLineEdit->setText(guiValues[connId].ChannelNumLineEditValue);
	ui.connectionComboBox->setCurrentIndex(guiValues[connId].connectionComboBoxId );
	ui.connectionNameLineEdit->setText(guiValues[connId].connectionNameLineEditValue);
	ui.connectionTypeComboBox->setCurrentIndex(guiValues[connId].connectionTypeComboBoxId);
	ui.contactTypeComboBox->setCurrentIndex(guiValues[connId].contactTypeComboBoxId);
	ui.controlPortLineEdit->setText(guiValues[connId].controlPortLineEditValue);
	ui.httpPortLineEdit->setText(guiValues[connId].httpPortLineEditValue);
	ui.ipAddressLineEdit->setText(guiValues[connId].ipAddressLineEditValue);
	ui.mcastPortLineEdit->setText(guiValues[connId].mcastPortLineEditValue);
	ui.mcIpAddressLineEdit->setText(guiValues[connId].mcIpAddressLineEditValue);
	ui.passwordLineEdit->setText(guiValues[connId].passwordLineEditValue);
	ui.ptzFileLineEdit->setText(guiValues[connId].ptzFileLineEditValue);
	ui.ptzFocusCheckBox->setChecked(guiValues[connId].ptzFocusCheckBoxValue);
	ui.ptzorCheckbox->setChecked(guiValues[connId].ptzorCheckboxValue);
	ui.ptzpushtoCheckBox->setChecked(guiValues[connId].ptzpushtoCheckBoxValue);
	ui.ptzZoomCheckBox->setChecked(guiValues[connId].ptzZoomCheckBoxValue);
	ui.regPortLineEdit->setText(guiValues[connId].regPortLineEditValue);
	ui.rtpmcastaPortLineEdit->setText(guiValues[connId].rtpmcastaPortLineEditValue);
	ui.rtpmcastvPortLineEdit->setText(guiValues[connId].rtpmcastvPortLineEditValue);
	ui.rtspAddressLineEdit->setText(guiValues[connId].rtspAddressLineEdit);
	ui.rtspPortLineEdit->setText(guiValues[connId].rtspPortLineEditValue);
	ui.snapFormatComboBox->setCurrentIndex(guiValues[connId].snapFormatComboBoxId);
	ui.streamingPortLineEdit->setText(guiValues[connId].streamingPortLineEdit);
	ui.usernameLineEdit->setText(guiValues[connId].usernameLineEditValue);
	ui.videoFormatComboBox->setCurrentIndex(guiValues[connId].videoFormatComboBoxId);
	ui.videoSizeComboBox->setCurrentIndex(guiValues[connId].videoSizeComboBoxId);
	ui.snapSizeComboBox->setCurrentIndex(guiValues[connId].snapSizeComboBoxId);
	ui.usbCvIndexLineEdit->setText(guiValues[connId].usbcamIndex);
	ui.recFolderLineEdit->setText(guiValues[connId].recordFolder);
	ui.snapFolderLineEdit->setText(guiValues[connId].snapshotFolder);
	// >

	// < GUI elemek kapcsolathoz szinkronizálása
	getProcessorsState();
	if(connections[connId] && connections[connId]->isAlive()){
		ui.connBtn->setText("Bontás");
		changeFPS(connections[connId]->getFPS());
		CvSize res = connections[connId]->getImageSize();
		changeResolution(res.width,res.height);
		cvWidget->setOverrideEnabled(true);
		ui.recBtn->setEnabled(true);
		ui.snapBtn->setEnabled(true);
	} else {
		ui.connBtn->setText("Kapcsolódás");
		resetVideoInfo();
		initResolutionSettings();
		cvWidget->setOverrideEnabled(false);
		cvWidget->update();
		ui.recBtn->setEnabled(false);
		ui.snapBtn->setEnabled(false);
	}

	int size = ipListGUI.size();
	if(connections[connId] && connections[connId]->isRecording()){
		ui.recBtn->setText("Feltétel leállítása");

		for(int idxA = 0; idxA < size; idxA++){
			ipListGUI[idxA].recordCB->setEnabled(false);
		}
	} else {
		ui.recBtn->setText("Feltétel indítása");
		for(int idxA = 0; idxA < size; idxA++){
			if(ipListGUI[idxA].enabledCB->isChecked()) ipListGUI[idxA].recordCB->setEnabled(true);
		}
	}
	// >

	guiSetup = false;

	// < A megfelelő gomb számának megvastagítása
	ui.switchBtn0->setStyleSheet((id == 0)?"font-weight: bold;":"font-weight: normal;");
	ui.switchBtn1->setStyleSheet((id == 1)?"font-weight: bold;":"font-weight: normal;");
	ui.switchBtn2->setStyleSheet((id == 2)?"font-weight: bold;":"font-weight: normal;");
	ui.switchBtn3->setStyleSheet((id == 3)?"font-weight: bold;":"font-weight: normal;");
	// >

	colorizeProcTitles();
	connectConnectionSlots();
}

void BugSurv::colorizeProcTitles(){
	int size = ipListGUI.size();
	if(connections[connId] && connections[connId]->isRecording()){
		for(int idxA = 0; idxA < size; idxA++){
				ipListGUI[idxA].titleLabel->setStyleSheet(connections[connId]->isIPRecording(idxA)?"color: green;":"color: black;");
		}
		ui.defProcLabel->setStyleSheet((connections[connId]->isIPRecording(-1))?"color: green;":"color: gray;");
	} else {
		for(int idxA = 0; idxA < size; idxA++){
				ipListGUI[idxA].titleLabel->setStyleSheet("color: black;");
		}
		ui.defProcLabel->setStyleSheet("color: gray;");
	}
}

// A pillanatkép és videorögzítéshez tartozó felbontásbeállító GUI elemek inicializálása.
void BugSurv::initResolutionSettings(){
	// < A korábban kiválaszott elemek indexének eltárolása
	int curSnapSize = ui.snapSizeComboBox->currentIndex();
	int curVideoSize = ui.videoSizeComboBox->currentIndex();
	// >

	// < Listák kiürítése
	ui.snapSizeComboBox->clear();
	ui.videoSizeComboBox->clear();
	// >
	
	// < Képméret (pillanatkép)
	ui.snapSizeComboBox->addItem("Eredeti", 1);
	ui.snapSizeComboBox->addItem("3/4", 0.75);
	ui.snapSizeComboBox->addItem("1/2", 0.5);
	ui.snapSizeComboBox->addItem("1/4", 0.25);
	// >

	// < Képméret (video)
	ui.videoSizeComboBox->addItem("Eredeti", 1);
	ui.videoSizeComboBox->addItem("3/4", 0.75);
	ui.videoSizeComboBox->addItem("1/2", 0.5);
	ui.videoSizeComboBox->addItem("1/4", 0.25);
	// >

	// < A korábban kiválasztott méret újra kiválasztása
	ui.snapSizeComboBox->setCurrentIndex((curSnapSize != -1)?curSnapSize:0);
	ui.videoSizeComboBox->setCurrentIndex((curVideoSize != -1)?curVideoSize:0);
	// >
}

// Feldolgozókhoz létrehozott GUI controlok eltávolítása, memória felszabadítása.
void BugSurv::remoiveIPlist(){
	QObject::disconnect(this, SLOT(renderProcessorChosen()));
	QObject::disconnect(this, SLOT(recordProcessorStateChanged(int)));
	QObject::disconnect(this, SLOT(processorEnableStateChanged(int)));
	QObject::disconnect(this, SLOT(showProcSettingsDialog()));
	QObject::disconnect(this, SLOT(detachStateChanged(int)));
	for(vector<ipListItem>::iterator it = ipListGUI.begin(); it != ipListGUI.end(); it++){
		delete it->renderRB;
		delete it->enabledCB;
		delete it->recordCB;
		delete it->titleLabel;
		delete it->fpsLabel;
		delete it->detachedCB;
	}
	ipListGUI.clear();
}

// Feldolgozókhoz GUI controlok létrehozása.
void BugSurv::initIPlist(){
	int size = Connection::ipCount();
	for(int idxA = 0; idxA < size; idxA++){
		ipListItem item;
		item.renderRB = new QRadioButton();
		item.renderRB->setStyleSheet("margin-left: 12px; height: 14px;");
		item.enabledCB = new QCheckBox();
		item.enabledCB->setStyleSheet("margin-left: 8px;  margin-top: 1px;");
		item.recordCB = new QCheckBox();
		item.recordCB->setStyleSheet("margin-left:8px; margin-top: 1px;");
		item.recordCB->setEnabled(false);
		item.titleLabel = new QLabel(Connection::getIpName(idxA));
		item.titleLabel->setStyleSheet("height: 14px;");
		item.settingsButton = new QPushButton();
		item.settingsButton->setStyleSheet("margin-left: 3px;margin-right:3px;");
		item.settingsButton->setMaximumHeight(14);
		item.settingsButton->setToolTip(QString::fromStdString("Feldolgozó paramétereinek beállítása"));
		item.fpsLabel = new QLabel("-");
		item.fpsLabel->setAlignment(Qt::AlignHCenter);
		item.detachedCB = new QCheckBox();
		item.detachedCB->setStyleSheet("margin-left: 8px; margin-top: 1px;");
		item.actionButton = new QPushButton();
		item.actionButton->setStyleSheet("margin-left: 3px;margin-right:3px;");
		item.actionButton->setMaximumHeight(14);
		item.actionButton->setToolTip(QString::fromStdString("Feldolgozó akcióihoz tartozó gombok"));

		ui.recordCBlist->addWidget(item.recordCB);
		ui.renderRBlist->addWidget(item.renderRB);
		ui.pnameLabelList->addWidget(item.titleLabel);
		ui.enableCBlist->addWidget(item.enabledCB);
		ui.settingsList->addWidget(item.settingsButton);
		ui.fpsLabelList->addWidget(item.fpsLabel);
		ui.detachedCBList->addWidget(item.detachedCB);
		ui.actionsList->addWidget(item.actionButton);

		ipListGUI.push_back(item);


		QObject::connect(item.renderRB,SIGNAL(toggled(bool)),SLOT(renderProcessorChosen()));
		QObject::connect(item.recordCB,SIGNAL(stateChanged(int)),SLOT(recordProcessorStateChanged(int)));
		QObject::connect(item.enabledCB,SIGNAL(stateChanged(int)),SLOT(processorEnableStateChanged(int)));
		QObject::connect(item.settingsButton,SIGNAL(clicked()),this,SLOT(showProcSettingsDialog()));
		QObject::connect(item.detachedCB,SIGNAL(stateChanged(int)),this,SLOT(detachStateChanged(int)));
		QObject::connect(item.actionButton,SIGNAL(clicked()),this,SLOT(showProcActionsDialog()));
	}

}

// GUI QWidgetek módosítása kapcsolattípus váltásnál
void BugSurv::changeConnectionType(int type){
	switch(type){
		case Connection::ACTI:
			ui.opencvConfWidget->hide();
			ui.actiConfWidget->show();
			ui.usbCvConfWidget->hide();
			ui.logoWidget->setStyleSheet("background-image: url(':/files/Resources/acti.png');");
			break;
		case Connection::OPENCV:
			ui.actiConfWidget->hide();
			ui.opencvConfWidget->show();
			ui.usbCvConfWidget->hide();
			ui.logoWidget->setStyleSheet("background-image: url(':/files/Resources/opencv.png');");
			break;
			case Connection::CVUSB:
			ui.actiConfWidget->hide();
			ui.opencvConfWidget->hide();
			ui.usbCvConfWidget->show();
	}
}

// Betölti a GUI-ba a mentett kapcsolatokat az adatbázisból.
void BugSurv::loadConnectionsList(bool keepSelection){
	int currentRealId = ui.connectionComboBox->itemData(ui.connectionComboBox->currentIndex()).toInt();
	ui.connectionComboBox->clear();
	ui.connectionComboBox->addItem(DEFAULT_CONNECTION_NAME, -1);
	for(std::vector<Configurations::ConnectionConf>::const_iterator it = configurations.getConnectionConfs().begin();it!=configurations.getConnectionConfs().end(); it++){
		ui.connectionComboBox->addItem(QString::fromStdString(it->name), it->id);
		if(keepSelection && it->id == currentRealId){
			ui.connectionComboBox->setCurrentText(QString::fromStdString(it->name));
		}
	}
}

// A beállított kapcsolat tesztelése
void BugSurv::connectionTestBtnClicked(){
	int cType = ui.connectionTypeComboBox->itemData(ui.connectionTypeComboBox->currentIndex()).toInt();
	bool result;
	Connection* testConnection = 0;

	if(cType == Connection::ACTI){
		testConnection = new ActiConnection(-1);
		result = testConnection->connect(&(getActiConnectionConfig()), true);
	} else if(cType == Connection::OPENCV) {
		testConnection = new OpencvConnection(-1);
		result = testConnection->connect(&(ui.rtspAddressLineEdit->text()),true);
	} else if (cType == Connection::CVUSB){
		testConnection = new LocalCameraConnection(-1);
		result = testConnection->connect(&(ui.usbCvIndexLineEdit->text()), true);
	}

	if(testConnection->isAlive()) testConnection->disconnect();
	QMessageBox msg;
	msg.setText((result)?"A kapcsolódás sikeres!":"Hiba a kapcsolódáskor!");
	msg.setWindowIcon(QIcon(":/files/Resources/ikon.png"));
	msg.setIcon(QMessageBox::Icon::Information);
	msg.exec();
	if(testConnection) delete testConnection;
}

// GUI widgetek szinkronizációja a kapcsolat feldolgozóbeállításaival. (Enabled, Detached, PreviewIdx, Record, FPS)
void BugSurv::getProcessorsState(int idx){
	bool oldgs = guiSetup;
	guiSetup = true;
	if(idx == -1) idx = connId;

	ui.defProcDetachCB->setChecked((connections[idx])?connections[idx]->getIPDetached(-1):false);
	ui.defProcRecordCB->setChecked((connections[idx])?connections[idx]->isIPRecording(-1):false);
	int size = ipListGUI.size();
	for(int idxA = 0; idxA < size; idxA++){
		ipListGUI[idxA].enabledCB->setChecked((connections[idx])?connections[idx]->getIPEnabled(idxA):false);
		ipListGUI[idxA].detachedCB->setChecked((connections[idx])?connections[idx]->getIPDetached(idxA):false);
		ipListGUI[idxA].recordCB->setChecked((connections[idx])?connections[idx]->isIPRecording(idxA):false);
		ipListGUI[idxA].enabledCB->setEnabled((connections[idx] && connections[idx]->isRecording())?!connections[idx]->isIPRecording(idxA):true);

		ipListGUI[idxA].fpsLabel->setText("-");
	}

	int oIdx = -1;

	if(connections[idx]){
		oIdx = connections[idx]->getOImageIdx();
	}

	if(oIdx == -1){
		ui.defProcRenderRB->setChecked(true);
	} else {
		ipListGUI[oIdx].renderRB->setChecked(true);
	}

	guiSetup = oldgs;
}

//Csatlakozás.
void BugSurv::connBtnClicked(){
	if(connections[connId] == 0)
	{
		// < Ha nincs kapcsolat a jelenlegi kapcsolatindexszel, akkor
		ui.connBtn->setEnabled(false);
		ui.statusBar->showMessage("Kapcsolódás...", 0);
		basicLogger.messageSent(QString::fromStdString("Kapcsolódás..."), LOG_DEFAULT_HEADER);

		int cType = ui.connectionTypeComboBox->itemData(ui.connectionTypeComboBox->currentIndex()).toInt();

		// < Kapcsolattípustól függően létrehoz egy Connection (egyik gyerek)osztály-ból egy példányt és csatlakozik
		if(cType == Connection::ACTI){
			connections[connId] =  new ActiConnection(connId);
			QObject::connect(connections[connId],SIGNAL(FPSChanged(int)),this,SLOT(changeFPS(int)));
			if(connections[connId]){
				connections[connId]->connect(&(getActiConnectionConfig()));
			}
		} else if(cType == Connection::OPENCV){
			connections[connId] = new OpencvConnection(connId);
			if(connections[connId]) QObject::connect(connections[connId],SIGNAL(FPSChanged(int)),this,SLOT(changeFPS(int)));
			connections[connId]->connect(&(ui.rtspAddressLineEdit->text()), false);
		} else if (cType == Connection::CVUSB){
			connections[connId] = new LocalCameraConnection(connId);
			if (connections[connId]) QObject::connect(connections[connId], SIGNAL(FPSChanged(int)), this, SLOT(changeFPS(int)));
			connections[connId]->connect(&(ui.usbCvIndexLineEdit->text()), false);
		}
		// >

		if(connections[connId] && connections[connId]->isAlive()){
			// < Ha a kapcsolódás sikeres volt
			cvWidget->setOverrideEnabled(true);
			connections[connId]->setRecFolder(ui.recFolderLineEdit->text().toStdString());
			connections[connId]->setSnapFolder(ui.snapFolderLineEdit->text().toStdString());
			connectConnectionSlots();

			ui.connBtn->setText("Bontás");
			ui.snapBtn->setEnabled(true);
			ui.recBtn->setEnabled(true);
			ui.statusBar->showMessage("Kapcsolódva...", 0);
			basicLogger.messageSent(QString::fromStdString("Kapcsolódva..."), connections[connId]->getLogHeader());
			ui.ptzorCheckbox->setChecked(false);

			getProcessorsState();
			setRecordSettings();
			// >
		} else {
			disconnectConnectionSlots();
			basicLogger.messageSent(QString::fromStdString("Sikertelen kapcsolódás!"), connections[connId]->getLogHeader());
			connections[connId]->deleteLater();
			connections[connId] = 0;
			ui.statusBar->showMessage("Sikertelen kapcsolódás!", 10000);
			resetVideoInfo();
		}
		// >
	} else {
		// < Ha már létezik kapcsolat a jelenlegi kapcsolatindexszel

		connections[connId]->disconnect();
		cvWidget->setOverrideEnabled(false);
		cvWidget->update();

		resetVideoInfo();
		initResolutionSettings();

		ui.connBtn->setText("Kapcsolódás");
		ui.snapBtn->setEnabled(false);
		ui.recBtn->setEnabled(false);
		ui.statusBar->showMessage("Kapcsolat bontva...", 10000);
		basicLogger.messageSent(QString::fromStdString("Kapcsolat bontva..."), connections[connId]->getLogHeader());
		if(connections[connId]) connections[connId]->deleteLater();
		connections[connId] = 0;
		// >
	}
	ui.connBtn->setEnabled(true);
}

// Szétkapcsolás után a videóinformációk resetelése a GUI-n.
void BugSurv::resetVideoInfo(){
	ui.actualFPSValueLabel->setText("-");
	ui.actualResValueLabel->setText("-");
	ui.defProcFpsLabel->setText("-");
	for(vector<ipListItem>::iterator it = ipListGUI.begin(); it != ipListGUI.end(); it++){
		it->fpsLabel->setText("-");
	}
}

// A kapcsolat jeleinek slot-okkal való összekapcsolása.
void BugSurv::connectConnectionSlots()
{
	if(connections[connId]){
	// $ Ha a kapcsolattípus támogatja a PTZ-t
		if(connections[connId]->hasPTZ()){
			QObject::connect(cControl, SIGNAL(ptzPT(int, byte)),connections[connId],SLOT(performPT(int, byte)));
			QObject::connect(cControl, SIGNAL(ptzZoom(int)),connections[connId],SLOT(performZoom(int)));
			QObject::connect(cControl, SIGNAL(ptzFocus(int)),connections[connId],SLOT(performFocus(int)));
		}
		QObject::connect(connections[connId],SIGNAL(FPSChanged(int)),this,SLOT(changeFPS(int)));
		QObject::connect(connections[connId],SIGNAL(procFPSChanged(int,int)),this,SLOT(changeProcFPS(int,int)));
		QObject::connect(connections[connId],SIGNAL(imageChanged(Mat)),this,SLOT(changeImage(Mat)));
		QObject::connect(connections[connId],SIGNAL(resolutionChanged(int,int)),this,SLOT(changeResolution(int,int)));
		QObject::connect(connections[connId],SIGNAL(recordForceStopped()),this,SLOT(recordForceStopped()));
		QObject::connect(connections[connId],SIGNAL(logMessageSent(QString, QString)), &basicLogger,SLOT(messageSent(QString, QString)));
	}
}

//  A kapcsolat jeleinek slot-okkal való szétkapcsolása.
void BugSurv::disconnectConnectionSlots(){
	if(connections[connId]){
		QObject::disconnect(connections[connId],SIGNAL(FPSChanged(int)),this,SLOT(changeFPS(int)));
		QObject::disconnect(connections[connId],SIGNAL(procFPSChanged(int,int)),this,SLOT(changeProcFPS(int,int)));
		QObject::disconnect(connections[connId],SIGNAL(imageChanged(Mat)),this,SLOT(changeImage(Mat)));
		QObject::disconnect(connections[connId],SIGNAL(resolutionChanged(int,int)),this,SLOT(changeResolution(int,int)));
		QObject::disconnect(connections[connId],SIGNAL(recordForceStopped()),this,SLOT(recordForceStopped()));
		QObject::disconnect(connections[connId],SIGNAL(logMessageSent(QString, QString)),this,SLOT(appendLogBox(QString, QString)));
		// $ Ha a kapcsolattípus támogatja a PTZ-t
		if(connections[connId]->hasPTZ()){
			QObject::disconnect(cControl, SIGNAL(ptzPT(int, byte)),connections[connId],SLOT(performPT(int, byte)));
			QObject::disconnect(cControl, SIGNAL(ptzZoom(int)),connections[connId],SLOT(performZoom(int)));
			QObject::disconnect(cControl, SIGNAL(ptzFocus(int)),connections[connId],SLOT(performFocus(int)));
		}
	}
}

// Kapcsolat szinkronizációja a GUI-n beállított értékekkel.
void BugSurv::setRecordSettings(int idx){
	if(idx == -1) idx = connId;
	if(connections[idx]){
		connections[idx]->setSnapSize(ui.snapSizeComboBox->itemData(ui.snapSizeComboBox->currentIndex()).toDouble());
		connections[idx]->setVideoSize(ui.videoSizeComboBox->itemData(ui.videoSizeComboBox->currentIndex()).toDouble());
		connections[idx]->setVideoFormat(ui.videoFormatComboBox->itemData(ui.videoFormatComboBox->currentIndex()).toInt());
		connections[idx]->setSnapFormat(ui.snapFormatComboBox->itemData(ui.snapFormatComboBox->currentIndex()).toString());
		connections[idx]->setRecFolder(ui.recFolderLineEdit->text().toStdString());
		connections[idx]->setSnapFolder(ui.snapFolderLineEdit->text().toStdString());
	}
}

// A kapcsolat által visszaadott felbontás megjelenítése a GUI-n.
void BugSurv::changeResolution(int w, int h){
	// < A korábban kiválaszott elemek indexének eltárolása
	int curSnapSize = ui.snapSizeComboBox->currentIndex();
	int curVideoSize = ui.videoSizeComboBox->currentIndex();
	// >

	// < Kiírt felbontás változtatása
	QString res = QString("%1x%2").arg(w).arg(h);
	ui.actualResValueLabel->setText(res);
	// >
	
	// < Képméret (pillanatkép)
	ui.snapSizeComboBox->clear();
	ui.snapSizeComboBox->addItem(res, 1.0);
	ui.snapSizeComboBox->addItem(QString("%1x%2").arg(qRound(w*0.75)).arg(qRound(h*0.75)), 0.75);
	ui.snapSizeComboBox->addItem(QString("%1x%2").arg(qRound(w*0.5)).arg(qRound(h*0.5)), 0.5);
	ui.snapSizeComboBox->addItem(QString("%1x%2").arg(qRound(w*0.25)).arg(qRound(h*0.25)), 0.25);
	// >

	// < Képméret (video)
	ui.videoSizeComboBox->clear();
	ui.videoSizeComboBox->addItem(res, 1.0);
	ui.videoSizeComboBox->addItem(QString("%1x%2").arg(qRound(w*0.75)).arg(qRound(h*0.75)), 0.75);
	ui.videoSizeComboBox->addItem(QString("%1x%2").arg(qRound(w*0.5)).arg(qRound(h*0.5)), 0.5);
	ui.videoSizeComboBox->addItem(QString("%1x%2").arg(qRound(w*0.25)).arg(qRound(h*0.25)), 0.25);
	// >

	// < A korábban kiválasztott méret újra kiválasztása
	ui.snapSizeComboBox->setCurrentIndex(curSnapSize);
	ui.videoSizeComboBox->setCurrentIndex(curVideoSize);
	// >
}

// A kapcsolat által visszaadott FPS megjelenítése a GUI-n
void BugSurv::changeFPS(int value){
	QString str = QString::number(value);
	ui.actualFPSValueLabel->setText(str);
	ui.defProcFpsLabel->setText(str);
}

// A kapcsolattól kapott openCV formátumú képkocka átadása a renderelő QWidgetnek
void BugSurv::changeImage(Mat img){
	if(!guiSetup){
		cvWidget->showImage(img);
	}
}

// Bezárás gomb megnyomásakor fut le.
void BugSurv::helpBtnClicked(){
	about.exec();
}

// Pillanatkép kérése
void BugSurv::snapBtnClicked(){
	if(connections[connId] && recordCheckBoxCheck() && connections[connId]->isAlive())
		connections[connId]->snapshot();
}

// Videófelvétel indítása/leállítása
void BugSurv::recBtnClicked(){
	if(connections[connId] && recordCheckBoxCheck()) {
		int size = ipListGUI.size();
		if(!connections[connId]->isRecording()){
			connections[connId]->startRecord();
			ui.recBtn->setText("Felvétel leállítása");
			for(int idxA = 0; idxA < size; idxA++){
				ipListGUI[idxA].recordCB->setEnabled(false);
				if(ipListGUI[idxA].recordCB->isChecked()){
					ipListGUI[idxA].enabledCB->setEnabled(false);
				}
			}
			if(ui.defProcRecordCB->isChecked()){
				ui.defProcEnabledCB->setEnabled(false);
			}
		} else {
			connections[connId]->stopRecord();
			ui.recBtn->setText("Felvétel indítása");
			for(int idxA = 0; idxA < size; idxA++){
				if(ipListGUI[idxA].enabledCB->isChecked())ipListGUI[idxA].recordCB->setEnabled(true);
				if(ipListGUI[idxA].recordCB->isChecked()){
					ipListGUI[idxA].enabledCB->setEnabled(true);
				}
			}
			if(ui.defProcEnabledCB->isChecked()){
				ui.defProcEnabledCB->setEnabled(false);
			}
		}
		colorizeProcTitles();
	}
}

// Visszaadja az élő kapcsolatok számát
int BugSurv::aliveConnections(){
	int r = 0;
	for(int idxA = 0; idxA < CONNC; idxA++){
		if(connections[idxA] && connections[idxA]->isAlive()) r++;
	}
	return r;
}

// A form bezárása előtt fut le.
void BugSurv::closeEvent( QCloseEvent* e){
	//$ Ha van élő kapcsolat (r > 0)
	if(aliveConnections()){
		e->ignore();

		// < Megerősítő üzenet megjelenítése
		QMessageBox msg;
		msg.setWindowTitle("Bezárás megerősítése");
		QAbstractButton *ybtn = msg.addButton("Igen", QMessageBox::ButtonRole::YesRole);
		QAbstractButton *nbtn = msg.addButton("Nem", QMessageBox::ButtonRole::NoRole);
		msg.setIcon(QMessageBox::Question);
		msg.setText("\nÉlő kapcsolatban van egy kamerával. Biztosan kilép?");
		msg.setWindowIcon(QIcon(":/files/Resources/ikon.png"));
		msg.exec();
		// >

		// $ Ha Igen gombra kattintott
		if (msg.clickedButton() == ybtn){
			for(int idxA = 0; idxA < CONNC; idxA++){
				if(connections[idxA] && connections[idxA]->isAlive()){
					connections[idxA]->disconnect();
					connections[idxA]->deleteLater();
				}
			}
			e->accept();
		}
	}
}

// A program hibás működése esetén küldött hibaüzenet.
void BugSurv::CriticalErrorMessage(QString& text){
	QMessageBox msg;
	msg.setWindowTitle("KRITIKUS HIBA");
	QAbstractButton *ybtn = msg.addButton("Igen", QMessageBox::ButtonRole::YesRole);
	QAbstractButton *nbtn = msg.addButton("Nem", QMessageBox::ButtonRole::NoRole);
	msg.setIcon(QMessageBox::Icon::Critical);
	msg.setText("Kritikus hiba!\n" + text + "\nMegszakítja a programot?");
	msg.setWindowIcon(QIcon(":/files/Resources/ikon.png"));
	msg.exec();
	// $ Ha Igenre kattint, akkor leállítjuk a program működését.
	if (msg.clickedButton() == ybtn){
		exit(-10);
	}
}

// CriticalErrorMessage char* megfelelője.
void BugSurv::CriticalErrorMessage(char* text){
	CriticalErrorMessage(QString(text));
}

// A form méretének kibővítése/csökkentése.
void BugSurv::moreBtnClicked(){
	if(!guiExpanded){
		setMaximumWidth(1150);
		resize(QSize(1150,ui.centralWidget->height()));
		setMinimumWidth(1150);
		ui.moreBtn->setText("<<");
	} else {
		setMinimumWidth(720);
		resize(QSize(720,ui.centralWidget->height()));
		setMaximumWidth(720);
		ui.moreBtn->setText(">>");
	}
	guiExpanded = !guiExpanded;
}

// Manuális PTZ ki- vagy bekapcsolásakor fut le.
void BugSurv::ptzorCheckBoxStateChanged(int v){
	if(!guiSetup){
		cControl->changeBG(v);
		cControl->setEnabled(v);
		if(connections[connId])connections[connId]->setManualPTZ(v);
	}
}

// A PTZ joy "Tartsd nyomva" beállítás változásait kezeli.
void BugSurv::ptzptCheckBoxStateChanged(int v){
	if(!guiSetup){
		cControl->setPushControl(v);
	}
}

// A PTZ joy zoomolás engedélyezése beállítás válozásait kezeli.
void BugSurv::ptzZoomCheckBoxStateChanged(int v){
	if(!guiSetup){
		cControl->setZoomEnabled(v);
	}
}

// A PTZ joy fókusz engedélyezése beállítás változásait kezeli.
void BugSurv::ptzFocusCheckBoxStateChanged(int v){
	if(!guiSetup){
		cControl->setFocusEnabled(v);
	}
}

// Mentett kapcsolat kiválasztása.
void BugSurv::connectionComboBoxIndexChanged(int idx){
	if(!guiSetup && ui.connectionComboBox->itemData(idx) != -1){
		Configurations::ConnectionConf* cfgp = configurations.getConnectionConfById(ui.connectionComboBox->itemData(idx).toInt());
		if(cfgp){
			ui.connectionNameLineEdit->setText(QString::fromStdString(cfgp->name));
			ui.ChannelNumLineEdit->setText(QString::number(cfgp->channelNumber));
			ui.contactTypeComboBox->setCurrentIndex(ui.contactTypeComboBox->findData(cfgp->contactType));
			ui.controlPortLineEdit->setText(QString::number(cfgp->controlPort));
			ui.httpPortLineEdit->setText(QString::number(cfgp->httpPost));
			ui.rtspPortLineEdit->setText(QString::number(cfgp->rtspPort));
			ui.mcastPortLineEdit->setText(QString::number(cfgp->multicastPort));
			ui.rtpmcastaPortLineEdit->setText(QString::number(cfgp->rtpMulticastAudioPort));
			ui.rtpmcastvPortLineEdit->setText(QString::number(cfgp->rtpMulticastVideoPort));
			ui.regPortLineEdit->setText(QString::number(cfgp->registerPort));
			ui.streamingPortLineEdit->setText(QString::number(cfgp->streamingPort));
			ui.usernameLineEdit->setText(QString::fromStdString(cfgp->user));
			ui.passwordLineEdit->setText(QString::fromStdString(cfgp->password));
			ui.ipAddressLineEdit->setText(QString::fromStdString(cfgp->unicastIP));
			ui.mcIpAddressLineEdit->setText(QString::fromStdString(cfgp->multicastIP));
			ui.ptzFileLineEdit->setText(QString::fromStdString(cfgp->ptzfile));
			ui.rtspAddressLineEdit->setText(QString::fromStdString(cfgp->rtspAddress));
			ui.connectionTypeComboBox->setCurrentIndex(ui.connectionTypeComboBox->findData(cfgp->connectionType));
			ui.usbCvIndexLineEdit->setText(QString::number(cfgp->usbcamIndex));
			ui.snapFolderLineEdit->setText(QString::fromStdString(cfgp->snapFolder));
			ui.recFolderLineEdit->setText(QString::fromStdString(cfgp->recFolder));
		}
	}
}

// Visszaadja a kapcsolati adatokat struktúrába szedve
Configurations::ConnectionConf BugSurv::getConnectionConf(){
	Configurations::ConnectionConf conf;

	//Alapadatok a mentett kapcsolathoz
	conf.connectionType = ui.connectionTypeComboBox->itemData(ui.connectionTypeComboBox->currentIndex()).toInt();
	conf.name = ui.connectionNameLineEdit->text().toStdString();
	conf.snapFolder = ui.snapFolderLineEdit->text().toStdString();
	conf.recFolder = ui.recFolderLineEdit->text().toStdString();

	//ACTi
	conf.unicastIP				= ui.ipAddressLineEdit->text().toStdString();
	conf.multicastIP			= ui.mcIpAddressLineEdit->text().toStdString();
	conf.contactType			= ui.contactTypeComboBox->itemData(ui.contactTypeComboBox->currentIndex()).toInt();
	conf.channelNumber			= ui.ChannelNumLineEdit->text().toInt();
	conf.registerPort			=  ui.regPortLineEdit->text().toInt();
	conf.controlPort			= ui.controlPortLineEdit->text().toInt();
	conf.streamingPort			= ui.streamingPortLineEdit->text().toInt();
	conf.httpPost				= ui.httpPortLineEdit->text().toInt();
	conf.multicastPort			= ui.mcastPortLineEdit->text().toInt();
	conf.rtspPort				= ui.rtspPortLineEdit->text().toInt();
	conf.rtpMulticastVideoPort	= ui.rtpmcastvPortLineEdit->text().toInt();
	conf.rtpMulticastAudioPort	= ui.rtpmcastaPortLineEdit->text().toInt();
	conf.user = ui.usernameLineEdit->text().toStdString();
	conf.password = ui.passwordLineEdit->text().toStdString();
	conf.ptzfile = ui.ptzFileLineEdit->text().toStdString();
	conf.channelNumber = ui.ChannelNumLineEdit->text().toInt();

	//OpenCV RTSP
	conf.rtspAddress = ui.rtspAddressLineEdit->text().toStdString();

	//OpenCV camera
	conf.usbcamIndex = ui.usbCvIndexLineEdit->text().toInt();

	return conf;
}

// Visszaadja egy (ACTI) MEDIA_CONNECTION_CONFIG struktúrában a kapcsolatbeállításhoz használt controlok értékeit.
ActiConnection::EXT_MEDIA_CONNECTION_CONFIG BugSurv::getActiConnectionConfig(){
	ActiConnection::EXT_MEDIA_CONNECTION_CONFIG mcc;
	memset(&mcc, 0, sizeof ActiConnection::EXT_MEDIA_CONNECTION_CONFIG);
	strcpy(mcc.UniCastIP, ui.ipAddressLineEdit->text().toStdString().c_str());
	strcpy(mcc.MultiCastIP, ui.mcIpAddressLineEdit->text().toStdString().c_str());

	mcc.ContactType		= ui.contactTypeComboBox->itemData(ui.contactTypeComboBox->currentIndex()).toInt();
	mcc.ChannelNumber	= ui.ChannelNumLineEdit->text().toInt();
	mcc.RegisterPort	= ui.regPortLineEdit->text().toInt();
	mcc.ControlPort		= ui.controlPortLineEdit->text().toInt();
	mcc.StreamingPort	= ui.streamingPortLineEdit->text().toInt();
	mcc.HTTPPort		= ui.httpPortLineEdit->text().toInt();
	mcc.MultiCastPort	= ui.mcastPortLineEdit->text().toInt();
	mcc.RTSPPort		= ui.rtspPortLineEdit->text().toInt();
	mcc.VideoRTPOverMCastPort = ui.rtpmcastvPortLineEdit->text().toInt();
	mcc.AudioRTPOverMCastPort = ui.rtpmcastaPortLineEdit->text().toInt();
	mcc.SearchPortC2S	= 6005;
	mcc.SearchPortS2C	= 6006;
	mcc.ConnectTimeOut	= 0;

	strcpy(mcc.UserID, ui.usernameLineEdit->text().toStdString().c_str());
	strcpy(mcc.Password, ui.passwordLineEdit->text().toStdString().c_str());
	mcc.ptzFile = ui.ptzFileLineEdit->text().toStdString();
	/*mcc.rtspAddress = ui.rtspAddressLineEdit->text().toStdString();
	mcc.connectionType = ui.connectionTypeComboBox->itemData(ui.connectionTypeComboBox->currentIndex()).toInt();
	mcc.name = ui.connectionNameLineEdit->text().toStdString();

	mcc.usbcamIndex = ui.usbCvIndexLineEdit->text().toInt();*/
	return mcc;
}

// Mentett kapcsolat törlése.
void BugSurv::delConnectionBtnCliecked(){
	int id = ui.connectionComboBox->itemData(ui.connectionComboBox->currentIndex()).toInt();
	if(ui.connectionComboBox->itemText(ui.connectionComboBox->currentIndex()).compare(DEFAULT_CONNECTION_NAME) != 0 && configurations.delConnectionConfById(id)){
		ui.connectionComboBox->removeItem(ui.connectionComboBox->currentIndex());
	}
}

// Kapcsolati beállítások mentése.
void BugSurv::connectionSaveBtnCliecked(){
	Configurations::ConnectionConf tmp = getConnectionConf();
	
	if(tmp.name.compare(DEFAULT_CONNECTION_NAME) != 0 && tmp.name.length() > 0){
		// < Ha a kapcsolat neve érvényes, akkor tároljuk
		configurations.addConnectionConf(tmp);
		loadConnectionsList();
		ui.connectionComboBox->setCurrentText(QString::fromStdString(tmp.name));
		// >
	} else {
		// < Ha a kapcsolat neve érvénytelen
		QMessageBox msg;
		msg.setText("Ilyen névű kapcsolat nem adható hozzá a listához!");
		msg.setIcon(QMessageBox::Icon::Warning);
		msg.exec();
		// >
	}
}

// PTZ fájl kiválasztása.
void BugSurv::ptzFileBtnClicked(){
	ActiConnection* acp;
	acp = dynamic_cast<ActiConnection*>(connections[connId]);
	QString url = QFileDialog::getOpenFileName(this, "Fájl megnyitása", "", "PTZ (*.ptz)");
	if(url.size()){
		ui.ptzFileLineEdit->setText(url);
		if(acp) acp->setPTZFile(url);
	} else {
		ui.ptzFileLineEdit->setText("");
		if(acp) acp->setPTZFile(url);
	}
}

// Videófelvétel formátumának módosítása.
void BugSurv::videoFormatComboBoxIndexChanged(int index){
	if(!guiSetup){
		if(connections[connId])connections[connId]->setVideoFormat(ui.videoFormatComboBox->itemData(index).toInt());
	}
}

// Pillanatkép formátumának módosítása.
void BugSurv::snapFormatComboBoxIndexChanged(int index){
	if(!guiSetup){
		if(connections[connId])connections[connId]->setSnapFormat(ui.snapFormatComboBox->itemData(index).toString());
	}
}

// Kapcsolattípus módosítása
void BugSurv::connectionTypeComboBoxIndexChanged(int index){
	if(!guiSetup){
		changeConnectionType(ui.connectionTypeComboBox->itemData(index).toInt());
	}
}

// Megjelenített feldolgozó képének kiválasztása.
void BugSurv::renderProcessorChosen(){
	if(!guiSetup){
		int size = ipListGUI.size();
		for(int idxA = 0; idxA < size; idxA++){
			if(ipListGUI[idxA].renderRB->isChecked()){
				if(connections[connId]) connections[connId]->setOImageIdx(idxA);
				return;
			}
		}
		if(connections[connId]) connections[connId]->setOImageIdx(-1);
	}
}

// Feldolgozó engedélyezésének változása.
void BugSurv::processorEnableStateChanged(int value){
	int size = ipListGUI.size();
	QObject* sender = QObject::sender();
	for(int idxA = 0; idxA < size; idxA++){
		if(sender == ipListGUI[idxA].enabledCB){
			if(connections[connId]){
				if(!guiSetup) connections[connId]->setIPEnabled(idxA,value);
				if(!connections[connId]->isRecording()) ipListGUI[idxA].recordCB->setEnabled(value);
			}
			ipListGUI[idxA].recordCB->setChecked(false);
		}
	}
	
}

// Pillanatkép méretének változása.
void BugSurv::snapSizeComboBoxIndexChanged(int idx){
	if(!guiSetup){
		if(connections[connId])connections[connId]->setSnapSize(ui.snapSizeComboBox->itemData(idx).toDouble());
	}
}

// Videófelvétel méretének változása.
void BugSurv::videoSizeComboBoxIndexChanged(int idx){
	if(!guiSetup){
		if(connections[connId])connections[connId]->setVideoSize(ui.snapSizeComboBox->itemData(idx).toDouble());
	}
}

// A felvétel kényszerből leállásnak GUI változásai
void BugSurv::recordForceStopped(){
	ui.recBtn->setText("Felvétel indítása");
	colorizeProcTitles();
	int size = ipListGUI.size();
	for(int idxA = 0; idxA < size; idxA++){
		ipListGUI[idxA].enabledCB->setEnabled(true);
	}
}

// Az aktuális kapcsolat egy feldolgozójának paraméterlistáját megjeleníti.
void BugSurv::showProcSettingsDialog(){
	if(connections[connId]){
		int size = Connection::ipCount();
		QObject* obj = QObject::sender();
		for(int idxA = 0; idxA < size; idxA++){
			if(ipListGUI[idxA].settingsButton == obj)
				connections[connId]->showProcSettingsDialog(idxA);
		}
	} else {
		QMessageBox msg;
		msg.setText("Feldolgozó paraméterek állítása csak kapcsolódás után lehetséges!");
		msg.setWindowIcon(QIcon(":/files/Resources/ikon.png"));
		msg.setIcon(QMessageBox::Icon::Information);
		msg.exec();
	}
}

//A megváltozott feldolgozó FPS megjelenítése.
void BugSurv::changeProcFPS(int idx,int val){
	if(val == -1)
		ipListGUI[idx].fpsLabel->setText("-");
	else 
		ipListGUI[idx].fpsLabel->setText(QString::number(val));
}

// Beállítja az aktuális kapcsolat egy feldolgozójának a külön ablakban való megjelenítést/elrejtést.
void BugSurv::detachStateChanged(int val){
	if(!guiSetup){
		QObject* senderobj = QObject::sender();
		if(connections[connId]){
			if(senderobj == ui.defProcDetachCB){
				connections[connId]->setIPDetached(-1,val);
			} else {
				int size = ipListGUI.size();
				for(int idxA = 0; idxA < size; idxA++){
					if(ipListGUI[idxA].detachedCB == senderobj){
						connections[connId]->setIPDetached(idxA, val);
					}
				}
			}
		}
	}
}

// Felvételhez használt feldolgozó képének kiválasztása.
void BugSurv::recordProcessorStateChanged(int val){
	if(!guiSetup){
		int size = ipListGUI.size();
		QObject* obj = QObject::sender();
		for(int idxA = 0; idxA < size; idxA++){
			if(obj == ipListGUI[idxA].recordCB){
				if(connections[connId]) connections[connId]->setProcRecording(idxA, val);
				return;
			}
		}
		if(connections[connId]) connections[connId]->setProcRecording(-1, val);
	}
}

// Ellenőrzi, hogy van-e felvenni kívánt videostream
bool BugSurv::recordCheckBoxCheck(){
	if(ui.defProcRecordCB->isChecked()) return true;
	int size = ipListGUI.size();
	for(int idxA = 0; idxA < size; idxA++){
		if(ipListGUI[idxA].recordCB->isChecked()) return true;
	}

	QMessageBox msg;
	msg.setText("Egy kimenet sincs kiválasztva felvételre!");
	msg.setWindowIcon(QIcon(":/files/Resources/ikon.png"));
	msg.setIcon(QMessageBox::Icon::Information);
	msg.exec();

	return false;
}

// Destruktor
BugSurv::~BugSurv()
{
	disconnect();
	delete ptzlay;
	delete cControl;
	delete cvWidget;
	remoiveIPlist();
	removeShortcuts();
	Connection::deleteIPlist();
}

// Megnyit egy tallózó ablakot a felvételek mappájához
void BugSurv::browseRecFolder(){
	QFileDialog dialog;
	QString folder = dialog.getExistingDirectory();
	if(folder != ""){
		ui.recFolderLineEdit->setText(folder);
		if(connections[connId])
			connections[connId]->setRecFolder(folder.toStdString());
	}
}

// Megnyit egy tallózó ablakot a pillanatképek mappájához
void BugSurv::browseSnapFolder(){
	QFileDialog dialog;
	QString folder = dialog.getExistingDirectory();
	if(folder != ""){
		ui.snapFolderLineEdit->setText(folder);
		if(connections[connId])
			connections[connId]->setSnapFolder(folder.toStdString());
	}
}

// Megnyitja a felvételek aktuális mentési helyét.
void BugSurv::openRecFolderBtnClicked(){
	QDesktopServices::openUrl(ui.recFolderLineEdit->text());
}

// Megnyitja a pillanatképek aktuális mentési helyét.
void BugSurv::openSnapFolderBtnClicked(){
	QDesktopServices::openUrl(ui.snapFolderLineEdit->text());
}

void BugSurv::appendLogBox(QString message, QString header){
	ui.logBrowser->append(QDateTime::currentDateTime().toString("hh:mm:ss ") + QString::fromStdString("<b>") + header + QString::fromStdString(LOG_HEAD_END) + QString::fromStdString("</b>") + message);
}

void BugSurv::showProcActionsDialog(){
	if(connections[connId]){
		int size = Connection::ipCount();
		QObject* obj = QObject::sender();
		for(int idxA = 0; idxA < size; idxA++){
			if(ipListGUI[idxA].actionButton == obj)
				connections[connId]->showProcActionsDialog(idxA);
		}
	} else {
		QMessageBox msg;
		msg.setText("Feldolgozó akciói csak kapcsolódás után elérhető!");
		msg.setWindowIcon(QIcon(":/files/Resources/ikon.png"));
		msg.setIcon(QMessageBox::Icon::Information);
		msg.exec();
	}
}