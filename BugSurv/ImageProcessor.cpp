/*
** @project			BugSurv Framework
** @file			ImageProcessor.cpp
** @author			Giczi D�niel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Minden k�pfeldolgoz� alapja. Meghat�rozza a feldolgoz�k interf�sz�t,
		�s a k�z�s r�szek defini�lva vannak az al�bbiakban.
*/

#include "ImageProcessor.h"

ImageProcessor::ImageProcessor(const string& _name) :
	enabled(false),
	priority(0),
	paramsChangedFlag(false),
	detached(false),
	name(_name)
{
	// < Alap�rtelmezett ablakn�v be�ll�t�sa a k�l�n megjelen�t�shez
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

// Be�ll�tja a fejl�ceket.
void ImageProcessor::initHeaders(){
	QString qthis = QString::number((uint)this);

	// < K�l�n megjelen�t� ablak neve
	windowName = QString::fromStdString(getName()) + " - " + qthis.mid(qthis.length()-8);
	dView.setWindowTitle(windowName);
	aDialog.setWindowTitle(windowName);
	sDialog.setWindowTitle(windowName);
	//>

	// < Napl�fejl�c be�ll�t�sa
	curLogHeader = QString::fromStdString(getName()) + " - " + qthis.mid(qthis.length()-8);
	// >
}

// Be�ll�tja az enged�lyezetts�g�t.
void ImageProcessor::setEnabled(bool value){
	enabled = value;
}

// �t�ll�tja a priorit�st.
void ImageProcessor::setPriority(int p){
	priority = p;
}

// Visszaadja a priorit�st.
int ImageProcessor::getPriority(){
	return priority;
}

// Visszaadja, hogy enged�lyezett-e.
bool ImageProcessor::isEnabled(){
	return enabled;
}

// Inicializ�lja a feldolgoz� strukt�r�j�t.
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

// Visszaadja a feldolgozott strukt�r�t.
ImageProcessor::ipData* ImageProcessor::getResults(){
	return &ipdata;
}

ImageProcessor::~ImageProcessor(void)
{
	setDetached(false);
}

// Int t�pus� param�ter hozz�ad�sa.
void ImageProcessor::addParam(const std::string paramName, int* paramPointer, bool editable){
	Parameter parameter;
	parameter.type = QVariant::Int;
	parameter.pointer = paramPointer;
	parameter.editable = editable;
	params.push_back(parameter);

	QVariant v = *paramPointer;
	sDialog.addParam(paramName, v, editable);
}

// Double t�pus� param�ter hozz�ad�sa.
void ImageProcessor::addParam(const std::string paramName, double* paramPointer, bool editable){
	Parameter parameter;
	parameter.type = QVariant::Double;
	parameter.pointer = paramPointer;
	parameter.editable = editable;
	params.push_back(parameter);

	QVariant v = *paramPointer;
	sDialog.addParam(paramName, v, editable);
}

// bool t�pus� param�ter hozz�ad�sa.
void ImageProcessor::addParam(const std::string paramName, bool* paramPointer, bool editable){
	Parameter parameter;
	parameter.type = QVariant::Bool;
	parameter.pointer = paramPointer;
	parameter.editable = editable;
	params.push_back(parameter);

	QVariant v = *paramPointer;
	sDialog.addParam(paramName, v, editable);
}

// QString t�pus� param�ter hozz�ad�sa.
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

// El�k�sz�ti a m�dos�tott param�terek bet�lt�s�t �s �tbillenti a (param�ter m�dosult) flaget.
void ImageProcessor::paramsChanged(){
	paramMutex.lock();
	changedParams = sDialog.getValues();
	paramsChangedFlag = true;
	paramMutex.unlock();
}

// Megnyitja a param�terbe�ll�t�sok ablakot.
void ImageProcessor::showSettings(){
	sDialog.exec();
}

// M�dos�tja a param�tereknek megfelel� v�ltoz�k �rt�k�t.
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

// Feldolgoz�s v�grehajt�sa.
void ImageProcessor::performProcessing(){
	loadParams();
	runActions();
	try{
	process();
	}
	catch(Exception ex){ writeLog(QString::fromStdString(ex.what())); }
	catch(exception ex){ writeLog(QString::fromStdString(ex.what())); }
	catch(int ex){ writeLog(QStringLiteral("Hiba k�d: ") + QString::number(ex)); }
	if(detached){
		emit refreshDetached(ipdata.frame.clone());
	}
}

// �r�k�ltetett feldolgoz�k itt adhatnak hozz� param�tereket.
void ImageProcessor::addParams(){}

void ImageProcessor::addActions(){}

// Param�terbe�ll�t�sok ablak inicializ�l�sa.
void ImageProcessor::initParams(){
	addParams();
	QObject::connect(&sDialog, SIGNAL(accepted()), this, SLOT(paramsChanged()));
}

// Az akci�k inicializ�l�sa.
void ImageProcessor::initActions(){
	addActions();
	QObject::connect(&aDialog, SIGNAL(actionEmitted(int)), this, SLOT(setAction(int)));
}

// Friss�ti a param�ter GUI-ban szerepl� �rt�k�t az aktu�lis �rt�kre.
void ImageProcessor::updateParam(const void* const pvar){
	int size = params.size();
	for(int idxA = 0; idxA < size; idxA++){
		if(params[idxA].pointer == pvar){
			sDialog.updateParam(idxA, pvar);
			return;
		}
	}
}

// Megnyitjja/bez�rja a k�l�n ablakot.
void ImageProcessor::setDetached(bool val){
	if(detached && !val)
		dView.forceClose();
	else if(!detached && val){
		dView.show();
	}
	
	detached = val;
}

// Visszaadja, hogy k�l�n ablakban meg van-e jelen�tve.
bool ImageProcessor::getDetached(){
	return detached;
}

// Napl�ba �zenetet k�ld.
void ImageProcessor::writeLog(QString message, QString prevHeader){
	emit logMessageSent(message, Logger::extendHeader(curLogHeader, prevHeader));
}

// Az akci�gombokhoz tartoz� ablakot jelen�ti meg.
void ImageProcessor::showActions(){
	aDialog.show();
}

// Akci�gomb megnyom�sakor �tbillenti a flag-et.
void ImageProcessor::setAction(int idx){
	actions[idx].needCall = true;
}

// Az aktiv�lt akci�gombokhoz rendelt elj�r�sokat futtatja.
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