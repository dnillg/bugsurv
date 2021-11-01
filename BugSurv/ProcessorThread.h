/*
** @project			BugSurv Framework
** @file			ProcessorThread.cpp
** @author			Giczi D�niel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		A feldolgoz�kat futtat� dedik�lt sz�l deklar�ci�ja.
*/

#ifndef PROCESSORTHREAD_H
#define PROCESSORTHREAD_H

// $ Konfigur�ci�
#include "cfg.h"

// $ OpenCV
#include "opencv2/opencv.hpp" 

// < Saj�t oszt�lyok
#include "ImageProcessor.h"
// >

// < QT
#include <QThread>
#include <QWaitCondition>
#include <QTime>
// >

using namespace cv;
using namespace std;

class ProcessorThread :
	public QThread
{
	Q_OBJECT
public:
	ProcessorThread(ImageProcessor* ipp);
	virtual ~ProcessorThread(void);
	void softStop();
	bool isProcessing();
	void process(Mat image, const unsigned long&);
private:
	QMutex mutex; // : Feldolgoz�s ir�ny�t�s�ra haszn�lt mutex - wcInitialized haszn�lja
	QMutex mutex2; // : Egyszerre csak inicializ�l�s t�rt�nhet
	unsigned short frameCount; // : El�z� m�sodperc �ta eltelt k�pkock�k sz�ma
	QWaitCondition wcInitialized; // : Meg�ll�tja a feldolgoz�st �s inicializ�ci� ut�n folytatja
	bool end; // : feldolgoz� meg�ll�t�s�hoz haszn�lt flag
	ImageProcessor* ipp; // A feldolgoz� objektumra mutat� pointer
	QTime fpsMeasureTime; // : FPS m�r�shez haszn�lt QT-s id�m�r�
	bool processing; // : Van-e �pp folyamatban feldolgoz�s
	void run() Q_DECL_OVERRIDE;
	bool calcFPS();
signals:
	void pFinished(ImageProcessor*, ImageProcessor::ipData);
	void procFPSChanged(int);
};

#endif //PROCESSORTHREAD_H