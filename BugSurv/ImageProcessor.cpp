/*
** @project			BugSurv Framework
** @file			ImageProcessor.cpp
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Minden képfeldolgozó alapja. Meghatározza a feldolgozók interfészét,
		és a közös részek definiálva vannak az alábbiakban.
*/

#include "ImageProcessor.h"

ImageProcessor::ImageProcessor(const string& _name) :
	enabled(false),
	priority(0),
	paramsChangedFlag(false),
	detached(false),
	name(_name)
{
	// < Alapértelmezett ablaknév beállítása a külön megjelenítéshez
	windowName =  QString("%1 - UNNAMED").arg((uint)this);
	QObject::connect(this,SIGNAL(refreshDetached(Mat)),&dView,SLOT(changeImage(Mat)));
	// >
}

ImageProcessor::ImageProcessor(const ImageProcessor& o):
	enabled(false),
	priority(0),
	paramsChangedFlag(false),
	detached(false),
	name(o.name)
{
	QObject::connect(this,SIGNAL(refreshDetached(Mat)),&dView,SLOT(changeImage(Mat)));
}

// Beállítja a fejléceket.
void ImageProcessor::initHeaders(){
	QString qthis = QString::number((uint)this);

	// < Külön megjelenítõ ablak neve
	windowName = QString::fromStdString(getName()) + " - " + qthis.mid(qthis.length()-8);
	dView.setWindowTitle(windowName);
	aDialog.setWindowTitle(windowName);
	sDialog.setWindowTitle(windowName);
	//>

	// < Naplófejléc beállítása
	curLogHeader = QString::fromStdString(getName()) + " - " + qthis.mid(qthis.length()-8);
	// >
}

// Beállítja az engedélyezettségét.
void ImageProcessor::setEnabled(bool value){
	enabled = value;
}

// Átállítja a prioritást.
void ImageProcessor::setPriority(int p){
	priority = p;
}

// Visszaadja a prioritást.
int ImageProcessor::getPriority(){
	return priority;
}

// Visszaadja, hogy engedélyezett-e.
bool ImageProcessor::isEnabled(){
	return enabled;
}

// Inicializálja a feldolgozó struktúráját.
void ImageProcessor::initIPdata(Mat& original, const unsigned long& counter){
	initMutex.lock();
	original.copyTo(ipdata.frame);
	ipdata.ptzCommand = 0/*Connection::PTZcommand::DONTCARE*/;
	ipdata.ptzSpeed = 0;
	ipdata.absPosition.P = 0;
	ipdata.absPosition.T = 0;
	ipdata.absPosition.Z = 0;
	ipdata.frameCounter = counter;
	ipdata.data = 0;
	initMutex.unlock();
}

// Visszaadja a feldolgozott struktúrát.
ImageProcessor::ipData* ImageProcessor::getResults(){
	return &ipdata;
}

ImageProcessor::~ImageProcessor(void)
{
	setDetached(false);
}

// Int típusú paraméter hozzáadása.
void ImageProcessor::addParam(const std::string paramName, int* paramPointer, bool editable){
	Parameter parameter;
	parameter.type = QVariant::Int;
	parameter.pointer = paramPointer;
	parameter.editable = editable;
	params.push_back(parameter);

	QVariant v = *paramPointer;
	sDialog.addParam(paramName, v, editable);
}

// Double típusú paraméter hozzáadása.
void ImageProcessor::addParam(const std::string paramName, double* paramPointer, bool editable){
	Parameter parameter;
	parameter.type = QVariant::Double;
	parameter.pointer = paramPointer;
	parameter.editable = editable;
	params.push_back(parameter);

	QVariant v = *paramPointer;
	sDialog.addParam(paramName, v, editable);
}

// bool típusú paraméter hozzáadása.
void ImageProcessor::addParam(const std::string paramName, bool* paramPointer, bool editable){
	Parameter parameter;
	parameter.type = QVariant::Bool;
	parameter.pointer = paramPointer;
	parameter.editable = editable;
	params.push_back(parameter);

	QVariant v = *paramPointer;
	sDialog.addParam(paramName, v, editable);
}

// QString típusú paraméter hozzáadása.
void ImageProcessor::addParam(const std::string paramName, QString* paramPointer, bool editable){
	Parameter parameter;
	parameter.type = QVariant::String;
	parameter.pointer = paramPointer;
	parameter.editable = editable;
	params.push_back(parameter);

	QVariant v = *paramPointer;
	sDialog.addParam(paramName, v, editable);
}

void ImageProcessor::addAction(const std::string& actionName, const std::string& slotName){
	Action action;
	action.needCall = false;
	action.slotName = slotName;
	actions.push_back(action);

	aDialog.addAction(actionName, slotName);
}

// Elõkészíti a módosított paraméterek betöltését és átbillenti a (paraméter módosult) flaget.
void ImageProcessor::paramsChanged(){
	paramMutex.lock();
	changedParams = sDialog.getValues();
	paramsChangedFlag = true;
	paramMutex.unlock();
}

// Megnyitja a paraméterbeállítások ablakot.
void ImageProcessor::showSettings(){
	sDialog.exec();
}

// Módosítja a paramétereknek megfelelõ változók értékét.
void ImageProcessor::loadParams(){
	paramMutex.lock();
	int size = changedParams.size();
	if(paramsChangedFlag){
		for(int idxA = 0; idxA < size; idxA++){
			if(params[idxA].editable){
				switch(params[idxA].type){
					case QVariant::Int:
						*((int*)params[idxA].pointer) = changedParams[idxA].toInt();
						break;
					case QVariant::Bool:
						*((bool*)params[idxA].pointer) = changedParams[idxA].toBool();
						break;
					case QVariant::String:
						*((QString*)params[idxA].pointer) = changedParams[idxA].toString();
						break;
					case QVariant::Double:
						*((double*)params[idxA].pointer) = changedParams[idxA].toDouble();
						break;
				}
			}
		}
		paramsChangedFlag = false;
		changedParams.clear();
	}
	paramMutex.unlock();
}

// Feldolgozás végrehajtása.
void ImageProcessor::performProcessing(){
	loadParams();
	runActions();
	try{
	process();
	}
	catch(Exception ex){ writeLog(QString::fromStdString(ex.what())); }
	catch(exception ex){ writeLog(QString::fromStdString(ex.what())); }
	catch(int ex){ writeLog(QStringLiteral("Hiba kód: ") + QString::number(ex)); }
	if(detached){
		emit refreshDetached(ipdata.frame.clone());
	}
}

// Örököltetett feldolgozók itt adhatnak hozzá paramétereket.
void ImageProcessor::addParams(){}

void ImageProcessor::addActions(){}

// Paraméterbeállítások ablak inicializálása.
void ImageProcessor::initParams(){
	addParams();
	QObject::connect(&sDialog, SIGNAL(accepted()), this, SLOT(paramsChanged()));
}

// Az akciók inicializálása.
void ImageProcessor::initActions(){
	addActions();
	QObject::connect(&aDialog, SIGNAL(actionEmitted(int)), this, SLOT(setAction(int)));
}

// Frissíti a paraméter GUI-ban szereplõ értékét az aktuális értékre.
void ImageProcessor::updateParam(const void* const pvar){
	int size = params.size();
	for(int idxA = 0; idxA < size; idxA++){
		if(params[idxA].pointer == pvar){
			sDialog.updateParam(idxA, pvar);
			return;
		}
	}
}

// Megnyitjja/bezárja a külön ablakot.
void ImageProcessor::setDetached(bool val){
	if(detached && !val)
		dView.forceClose();
	else if(!detached && val){
		dView.show();
	}
	
	detached = val;
}

// Visszaadja, hogy külön ablakban meg van-e jelenítve.
bool ImageProcessor::getDetached(){
	return detached;
}

// Naplóba üzenetet küld.
void ImageProcessor::writeLog(QString message, QString prevHeader){
	emit logMessageSent(message, Logger::extendHeader(curLogHeader, prevHeader));
}

// Az akciógombokhoz tartozó ablakot jeleníti meg.
void ImageProcessor::showActions(){
	aDialog.show();
}

// Akciógomb megnyomásakor átbillenti a flag-et.
void ImageProcessor::setAction(int idx){
	actions[idx].needCall = true;
}

// Az aktivált akciógombokhoz rendelt eljárásokat futtatja.
void ImageProcessor::runActions(){
	int size = actions.size();
	for(int i = 0; i < size; i++){
		if(actions[i].needCall){
			QMetaObject::invokeMethod(this, actions[i].slotName.c_str());
			actions[i].needCall = false;
		}
	}
}

std::string ImageProcessor::getName(){
	return name;
}

void ImageProcessor::setName(const string _name){
	name = _name;
}