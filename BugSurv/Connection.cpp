/*
** @project			BugSurv Framework
** @file			Connection.cpp
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Minden kapcsolattípus alapja. Egy absztrakt osztály, nem példányosítható.
		A specifikus részeknek csak az interfésze van megadva, míg a közös részek
		definíciója alább látható.
*/

#include "Connection.h"

// < Statikus változó inicializálása
vector<ImageProcessor*> Connection::initIplist;
QString Connection::logHeader = QString::fromStdString("Kapcsolat");
// >

Connection::Connection(int _connID):
	connID(_connID),
	snapFormat("jpg"),
	videoFormat(CV_FOURCC('D','I','V','3')),
	fps(-1),
	processors(0),
	iplSize(0),
	oImageIdx(-1),
	rImageIdx(-1),
	snapSizeMultiplier(1),
	videoSizeMultiplier(1),
	manualPTZ(false),
	recording(false),
	defDetached(false),
	lastPTZCommand(QDateTime::currentDateTime()),
	counter(0UL)
{
	// < Aktuális képméret az első képkocka olvasása után változni fog
	imageSize.width = 0;
	imageSize.height = 0;
	// >

	// < Az inicializáló képfeldolgozó példányokból másolatot készítünk
	iplSize = initIplist.size();
	if(iplSize > 0){
		int idx = 0;
		processors = new ImageProcessor*[iplSize];
		for(std::vector<ImageProcessor*>::iterator it = initIplist.begin(); it != initIplist.end(); it++){
			// $ Prototípus másolás
			processors[idx] = (*it)->clone();

			QObject::connect(processors[idx], SIGNAL(logMessageSent(QString, QString)), SLOT(writeLog(QString, QString)));
			idx++;
		}
	}
	// >

	// < Feldolgozószálak inicalizálása
	processorThreads = new ProcessorThread*[iplSize];
	for(int idxA = 0; idxA < iplSize; idxA++){
		processorThreads[idxA] = new ProcessorThread(processors[idxA]);
		QObject::connect(processorThreads[idxA], SIGNAL(pFinished(ImageProcessor*, ImageProcessor::ipData)), this, SLOT(handleResult(ImageProcessor*,ImageProcessor::ipData)));
		QObject::connect(processorThreads[idxA], SIGNAL(procFPSChanged(int)), this, SLOT(passProcFPSChanged(int)));
		processorThreads[idxA]->start();
	}
	// >

	// $ Manuális FPS beállításhoz kapcsolódó slot csatlakoztatása
	QObject::connect(&fpsDialog, SIGNAL(setFps(int)), this, SLOT(setManualFPS(int)));

	// < Felvételt lebonyolító adatszerkezetek inicalizálása
	recorders = new Recorder[iplSize+1];
	for(int idxA = 0; idxA <= iplSize; idxA++){
		recorders[idxA].endRecord = false;
		recorders[idxA].frameExists = false;
		recorders[idxA].ifrecord = false;
		recorders[idxA].nf_snapshot = false;
	}
	// >

	// $ Napló fejléc kiegészítése
	curLogHeader = logHeader + (QString::fromStdString("[%1]").arg(connID+1));
}

// A feldolgozók törlése
void Connection::deleteIPlist(){
	for(std::vector<ImageProcessor*>::iterator it = initIplist.begin(); it != initIplist.end(); it++){
		delete *it;
		*it = 0;
	}
}

// FPS változásának kezelése
void Connection::setFPS(int value){
	fps = value;
	emit FPSChanged(value);
}

// Visszaadja, hogy az adott indexű feldolgozó engedélyezett-e.
bool Connection::getIPEnabled(int idx){
	return (idx >= 0 && idx < iplSize) ? processors[idx]->isEnabled() : false;
}

// Adott indexű feldolgozó engedélyezettségét állítja be.
void Connection::setIPEnabled(int idx, bool enabled){
	if(initIplist.size() > idx && idx >= 0){
		processors[idx]->setEnabled(enabled);
		if(!enabled)
			emit procFPSChanged(idx, -1);
	}
}

// Visszaadja az adott indexű feldolgozó nevét.
QString Connection::getIpName(int idx){
	return (initIplist.size() > idx && idx >= 0 && initIplist[idx])?QString::fromStdString(initIplist[idx]->getName()):"UNDEFINED";
}

// Visszaadja a feldolgozók számát.
int Connection::ipCount(){
	return initIplist.size();
}

// Feldolgozó hozzáadása az inicializálási listához.
bool Connection::addIpClass(ImageProcessor* ipp, int p){
	if(!ipp) return false;
	ipp->setPriority(p);
	initIplist.push_back(ipp);
	return true;
}

// Videófelvétel kezdése.
bool Connection::startRecord(){
	if(!isRecording()){
		emit logMessageSent("Felvétel megkezdése...", curLogHeader);
		writeLog("Felvétel megkezdése...");
		QDateTime dt = QDateTime::currentDateTime();
		QDir dir(QString::fromStdString(recFolder).append("/"));
		if(!dir.exists()) dir.mkdir(".");
		QString filename = QString::fromStdString(recFolder) + "/" + dt.toString("yyyy_M_d_h_m_s_zzz-%1") + ".avi";
		recordingSize.width = qRound(imageSize.width*videoSizeMultiplier);
		recordingSize.height = qRound(imageSize.height*videoSizeMultiplier);
		for(int idxA = 0; idxA <= iplSize; idxA++){
			QString postfix = (idxA == iplSize)?"input":QString::number(idxA);
			if(recorders[idxA].ifrecord){
				QString rFileName = filename.arg(postfix);
				writeLog(QString::fromStdString("Felvétel elindult a következő fájlba: ") + rFileName);
				recorders[idxA].vWriter.open(rFileName.toStdString(),videoFormat,(fps != -1)?fps:25,recordingSize);
			}
		}
		recording = true;
		return true;
	}
	return false;
}

// Visszaadja az aktuális képkocka méretet.
CvSize Connection::getImageSize(){
	return imageSize;
}

// Leállítja az aktuális felvételt.
bool Connection::stopRecord(){
	if(isRecording()){
		/*if(recorders[iplSize].ifrecord){
		recorders[iplSize].recordMutex.lock();
		recorders[iplSize].vWriter.release();
		recorders[iplSize].recordMutex.unlock();
		}*/
		for(int idxA = 0; idxA <= iplSize; idxA++){
			if(recorders[idxA].ifrecord){
				recorders[idxA].endRecord = true;
			}
		}
		recording = false;
		writeLog("Felvétel(ek) befejezve!");
		return true;
	}
	return false;
}

// Átállítja igazra a pillanatkép flaget.
void Connection::snapshot(){
	// < Ha még nem létezik a mappa, akkor létrehozza
	QDir dir(QString::fromStdString(snapFolder).append("/"));
	if(!dir.exists()) dir.mkdir(".");
	// >

	for(int idxA = 0; idxA < iplSize; idxA++){
		if(recorders[idxA].ifrecord && processors[idxA]->isEnabled()){
			recorders[idxA].nf_snapshot = true;
		}
	}

	if(recorders[iplSize].ifrecord){
		recorders[iplSize].nf_snapshot = true;
	}
}

// Visszaadja, hogy épp rögzít videót vagy sem.
bool Connection::isRecording(){
	return recording;
}

// Beállítja a pillanatkép formátumát.
void Connection::setSnapFormat(QString format){
	snapFormat = format;
}

// Beállítja a videófelvétel formátumát.
void Connection::setVideoFormat(int format){
	videoFormat = format;
}

// Visszaadja az aktuális FPS-t.
int Connection::getFPS(){
	return fps;
}

// Képkocka feldolgozásának kezelése.
void Connection::processFrame(Mat& frame){
	frameMutex.lock();
	counter++;
	// <Képkocka méretének ellenőrzése
	CvSize frameSize = frame.size();
	if(imageSize.width != frameSize.width || imageSize.height != frameSize.height){
		imageSize.width = frameSize.width;
		imageSize.height = frameSize.height;
		emit resolutionChanged(imageSize.width, imageSize.height);
	}
	// >

	// < Felvételhez képkockaírás
	recordFrame(frame, iplSize);

	for(int idxA = 0; idxA < iplSize; idxA++){
		if(recorders[idxA].frameExists){
			recordFrame(recorders[idxA].lastframe, idxA);
		}
	}
	// >

	// < Eredeti stream külön ablakban megjelenítése
	if(defDetached){
		defDetachedView.changeImage(frame);
	}
	// >


	// < Feldolgozók futtatása
	for(int idxA = 0; idxA < iplSize; idxA++){
		if(processors[idxA]->isEnabled()){
			processorThreads[idxA]->process(frame, counter);
		}
	}
	// >	

	// < Ha az előnézet feldolgozás nélküli, akor megjelenítjük a képkockát
	if(oImageIdx == -1 || !processors[oImageIdx]->isEnabled()){
		emit imageChanged(frame);
	}
	// >
	frameMutex.unlock();
}

// Beállítja a megjelenítendő feldolgozó képének indexét (-1: eredeti).
void Connection::setOImageIdx(int idx){
	oImageIdx = (idx < -1 || idx >= iplSize)?-1:idx;
}

// Visszaadja a megjelenítendő feldolgozó indexét
int Connection::getOImageIdx(){
	return oImageIdx;
}

// Beállítja a rögzítendő feldolgozó képének indexét (Tartmonányon kívül: alapstream).
void Connection::setProcRecording(int idx, bool val){
	if(idx > -1 && idx < iplSize){
		recorders[idx].frameExists = false;
		recorders[idx].ifrecord = val;
	} else {
		recorders[iplSize].frameExists = false;
		recorders[iplSize].ifrecord = val;
	}
}

// Visszaadja a rögzítendő feldolgozó indexét.
int Connection::getProcRecording(int idx){
	return recorders[idx].ifrecord;
}

// Feldolgozó által visszaadott eredményt értelmezése, megfelelő PTZ művelet végrehajtása.
void Connection::handlePTZ(const ImageProcessor::ipData* const ipdata){
	if(lastPTZCommand.addMSecs(100) <= QDateTime::currentDateTime()){
		if(ipdata->ptzCommand < PTZcommand::ZOOM_STOP){
			performPT(ipdata->ptzCommand, ipdata->ptzSpeed);
		} else if(ipdata->ptzCommand < PTZcommand::FOCUS_STOP){
			performZoom(ipdata->ptzCommand-PTZcommand::ZOOM_STOP);
		} else if(ipdata->ptzCommand < PTZcommand::DONTCARE){
			performFocus(ipdata->ptzCommand-PTZcommand::FOCUS_STOP);
		} else if(ipdata->ptzCommand == PTZcommand::ABSPOS){
			performAbsolute(ipdata->absPosition.P, ipdata->absPosition.T, ipdata->absPosition.Z);
		}
		/* DONTCARE jelen esetben azonos a DONOTHING-al,
		** előbbinek a prioritáskezelésben van jelentősége.
		*/
		lastPTZCommand = QDateTime::currentDateTime();
	}
}

// Visszaadja, hogy a kapcsolattípus támogatja-e a PTZ-t.
bool Connection::hasPTZ(){
	return false;
}

// Beállítja a PTZ kezelési módját (igaz: manuális)
void Connection::setManualPTZ(bool value){
	manualPTZ = value;
	if(value){
		performPT(PTZcommand::PT_STOP, 0);
	}
}

// Beállítja a videófelvételhez használt méretszorzót
void Connection::setVideoSize(double mult){
	videoSizeMultiplier = mult;
}

// Beállítja a pillanatképhez használt méretszorzót
void Connection::setSnapSize(double mult){
	snapSizeMultiplier = mult;
}

// Zoom művelet végrehajtása
void Connection::performZoom(int r){
	//Ez egy alapértelmezett definíció, származtatott osztályban felüldefiniálható.
}

// Pan-Tilt művelet végrehajtása
void Connection::performPT(int direction, byte speed){
	//Ez egy alapértelmezett definíció, származtatott osztályban felüldefiniálható.
}

// Fókusz művelet végrehajtása
void Connection::performFocus(int r){
	//Ez egy alapértelmezett definíció, származtatott osztályban felüldefiniálható.
}

// Abszolút pozícióba állás
void Connection::performAbsolute(const double P, const double T, const double Z){
	//Ez egy alapértelmezett definíció, származtatott osztályban felüldefiniálható.
}

// Megjeleníti a feldolgozó beállításait.
void Connection::showProcSettingsDialog(int idx){
	if(idx >= 0 && idx < iplSize)
		processors[idx]->showSettings();
}

// Feldolgozó eredményének kezelése.
void Connection::handleResult(ImageProcessor* ipp, ImageProcessor::ipData ipdata){
	if(processors[oImageIdx] == ipp){
		emit imageChanged(ipdata.frame);
	}

	for(int idxA = 0; idxA < iplSize; idxA++){
		if(processors[idxA] == ipp){
			recorders[idxA].recordMutex.lock();
			recorders[idxA].lastframe = ipdata.frame;
			recorders[idxA].frameExists = true;
			recorders[idxA].recordMutex.unlock();
		}
	}

	// $ Ha a kapcsolattípus támogatja a PTZ-t és nincs manuális PTZ
	if(hasPTZ() && !manualPTZ){
		// < Eredmény tekintetében PTZ utasítás kiadása ha szükséges
		bool execute = true;
		for(int idxA = 0; idxA < iplSize; idxA++){
			if(processors[idxA]->isEnabled()){
				if(ipp != processors[idxA] && ipp->getPriority() < processors[idxA]->getPriority())
					execute = false;
			}
		}

		if(execute)
			handlePTZ(ipp->getResults());
		// >
	}	
}

// Képkockra rögzítésének rutinjai.
void Connection::recordFrame(Mat& frame, int idx){
	recorders[idx].recordMutex.lock();
	CvSize frameSize = frame.size();
	QString postfix = (idx > -1 && idx < iplSize)?QString::number(idx):"input";

	// $ Ha a snapshot flag igaz, akkor menti a képkockát.
	if(recorders[idx].nf_snapshot){
		recorders[idx].nf_snapshot = false;
		QDateTime dt = QDateTime::currentDateTime();
		QString fn = QString::fromStdString(snapFolder) + "/" + dt.toString("yyyy_M_d_h_m_s_zzz-%1") + "." + snapFormat;
		if(snapSizeMultiplier == 1){
			imwrite(fn.arg(postfix).toStdString(), frame);
		} else {
			Mat resizedFrame;
			resize(frame,resizedFrame,cvSize(frameSize.width*snapSizeMultiplier,frameSize.height*snapSizeMultiplier),0,0,INTER_LINEAR);
			imwrite(fn.arg(postfix).toStdString(),resizedFrame);
		}
		writeLog("Képernyőkép elkészült a következő helyre: " + fn);
	}

	// $ Ha felveszünk, akkor hozzáírja a képkcokát
	if(recorders[idx].vWriter.isOpened()){
		// $ Ha van a felvétel vége flag igaz, akkor lezárja a felvételt
		if(recorders[idx].endRecord){
			recorders[idx].endRecord=false;
			recorders[idx].vWriter.release(); 
		} else if(recorders[idx].ifrecord) {
			if(recordingSize.width != frameSize.width || recordingSize.height != frameSize.height){
				Mat resizedFrame;
				resize(frame,resizedFrame,recordingSize,0,0,INTER_LINEAR);
				recorders[idx].vWriter.write(resizedFrame);
			} else {
				recorders[idx].vWriter.write(frame);
			}
		}
	}
	recorders[idx].recordMutex.unlock();
}

// Meghatározza a feldolgozó indexét és átpasszolja a guinak a feldolgozó fps-ét.
void Connection::passProcFPSChanged(int fps){
	QObject* objp = QObject::sender();
	for(int idxA = 0; idxA < iplSize; idxA++){
		if(objp == processorThreads[idxA]){
			emit procFPSChanged(idxA, fps);
		}
	}
}

//  Beállítja, hogy megjeleníti-e külön ablakban a feldolgozó kimenetét.
void Connection::setIPDetached(int idx, bool val){
	if(idx == -1){
		if(!defDetached && val){
			defDetached = true;
			defDetachedView.show();
		} else if(defDetached && !val){
			defDetachedView.forceClose();
			defDetached = false;
		}
	} else{
		processors[idx]->setDetached(val);
	}
}

// Visszaadja, hogy meg van-e jelenítve külön ablakban a feldolgozó kimenete.
bool Connection::getIPDetached(int idx){
	if(idx == -1)
		return defDetached;
	else
		return processors[idx]->getDetached();
}

// Manuális FPS állító dialog megjelenítése.
void Connection::showFpsChangeDialog(){
	fpsDialog.exec();
}

// A dialogtól visszakapott FPS-t beállítja.
void Connection::setManualFPS(int value){
	setFPS(value);
}

// Visszaadja, hogy a paraméterben átadott indexű feldolgozó felvesz-e vagy sem. (Tartományon kívül: alapstream)
bool Connection::isIPRecording(int idx){
	if(idx > -1 && idx < iplSize){
		return recorders[idx].ifrecord;
	} else {
		return recorders[iplSize].ifrecord;
	}
}

// Beállítja a felvétel könyvtárát
void Connection::setRecFolder(string folder){
	recFolder = folder;
}

// Beállítja a pillanatképek könyvtárát
void Connection::setSnapFolder(string folder){
	snapFolder = folder;
}

// Visszaadja a kapcsolat napló fejlécét
const QString& Connection::getLogHeader(){
	return curLogHeader;
}

void Connection::writeLog(QString message, QString prevHeader){
	emit logMessageSent(message, Logger::extendHeader(curLogHeader, prevHeader));
}

void Connection::showProcActionsDialog(int idx){
	if(idx >= 0 && idx < iplSize)
		processors[idx]->showActions();
}

Connection::~Connection(void)
{
	for(int idxA = 0; idxA < iplSize; idxA++){
		processorThreads[idxA]->softStop();
		QObject::disconnect(processorThreads[idxA], SIGNAL(pFinished(ImageProcessor*, ImageProcessor::ipData)), this, SLOT(handleResult(ImageProcessor*,ImageProcessor::ipData)));
		QObject::disconnect(processorThreads[idxA], SIGNAL(procFPSChanged(int)), this, SLOT(passProcFPSChanged(int)));
	}

	QThread::msleep(100);

	// < Feldolgozók felszabadítása
	for(int idxA = 0; idxA < iplSize; idxA++){
		processorThreads[idxA]->deleteLater();
		if(processors[idxA]) processors[idxA]->deleteLater();
	}
	delete [] processors;
	// >

	delete [] processorThreads;

	// < Ha van futó felvétel, akkor azt befejezi
	if(isRecording()){
		for(int idxA = 0; idxA < iplSize; idxA++){
			recorders[idxA].vWriter.release();
		}
		recording = false;
		emit recordForceStopped();
	}
	delete [] recorders;
	// >
}