/*
** @project			BugSurv Framework
** @file			ProcessorThread.cpp
** @author			Giczi D�niel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		A feldolgoz�kat futtat� dedik�lt sz�l implement�ci�ja.
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

// A QThread run() f�ggv�ny�nek fel�ldefini�l�sa.
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

// Visszaadja, hogy van-e �pp folyamatban feldolgoz�s.
bool ProcessorThread::isProcessing(){
	return processing;
}

// �tbillent egy flaget, a k�vetkez� feldolgozand� frame helyett kil�p.
void ProcessorThread::softStop(){
	end = true;
	wcInitialized.wakeAll();
	//exit();
}

// Feldolgoz�s elind�t�sa.
void ProcessorThread::process(Mat image, const unsigned long& counter){
	if(!processing){
		mutex2.lock();
		ipp->initIPdata(image, counter);
		wcInitialized.wakeAll();
		//exit();
		mutex2.unlock();
	}
}

// Sz�molja a m�sodperc alatt l�trej�tt k�pkock�kat, ha eltelt 1 m�sodperc, akkor visszajelez.
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