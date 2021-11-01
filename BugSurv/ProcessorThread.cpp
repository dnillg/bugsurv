/*
** @project			BugSurv Framework
** @file			ProcessorThread.cpp
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		A feldolgozókat futtató dedikált szál implementációja.
*/

#include "ProcessorThread.h"

// Konstruktor.
ProcessorThread::ProcessorThread(ImageProcessor* _ipp):
	processing(false),
	ipp(_ipp),
	frameCount(0)
{
	mutex.lock();
	end = !ipp;
	fpsMeasureTime.start();
}

// Destruktor
ProcessorThread::~ProcessorThread(void)
{
	mutex.unlock();
}

// A QThread run() függvényének felüldefiniálása.
void ProcessorThread::run(){
	do {
		processing = false;
		wcInitialized.wait(&mutex);
		//exec();
		if(!end){
			processing = true;
			ipp->performProcessing();
			calcFPS();
			ImageProcessor::ipData ipdata;
			ImageProcessor::ipData* pOriginal = ipp->getResults();
			ipp->getResults()->frame.copyTo(ipdata.frame);
			ipdata.ptzCommand = pOriginal->ptzCommand;
			ipdata.ptzSpeed = pOriginal->ptzSpeed;
			ipdata.absPosition = pOriginal->absPosition;
			emit pFinished(ipp, ipdata);
		}
	} while(!end) ;
}

// Visszaadja, hogy van-e épp folyamatban feldolgozás.
bool ProcessorThread::isProcessing(){
	return processing;
}

// Átbillent egy flaget, a következõ feldolgozandó frame helyett kilép.
void ProcessorThread::softStop(){
	end = true;
	wcInitialized.wakeAll();
	//exit();
}

// Feldolgozás elindítása.
void ProcessorThread::process(Mat image, const unsigned long& counter){
	if(!processing){
		mutex2.lock();
		ipp->initIPdata(image, counter);
		wcInitialized.wakeAll();
		//exit();
		mutex2.unlock();
	}
}

// Számolja a másodperc alatt létrejött képkockákat, ha eltelt 1 másodperc, akkor visszajelez.
bool ProcessorThread::calcFPS(){
	if(fpsMeasureTime.elapsed() < 1000){
		frameCount++;
	} else {
		fpsMeasureTime.restart();
		emit procFPSChanged(frameCount);
		frameCount = 0;
		return true;
	}
	return false;
}