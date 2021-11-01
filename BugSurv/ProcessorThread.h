/*
** @project			BugSurv Framework
** @file			ProcessorThread.cpp
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		A feldolgozókat futtató dedikált szál deklarációja.
*/

#ifndef PROCESSORTHREAD_H
#define PROCESSORTHREAD_H

// $ Konfiguráció
#include "cfg.h"

// $ OpenCV
#include "opencv2/opencv.hpp" 

// < Saját osztályok
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
	QMutex mutex; // : Feldolgozás irányítására használt mutex - wcInitialized használja
	QMutex mutex2; // : Egyszerre csak inicializálás történhet
	unsigned short frameCount; // : Elõzõ másodperc óta eltelt képkockák száma
	QWaitCondition wcInitialized; // : Megállítja a feldolgozást és inicializáció után folytatja
	bool end; // : feldolgozó megállításához használt flag
	ImageProcessor* ipp; // A feldolgozó objektumra mutató pointer
	QTime fpsMeasureTime; // : FPS méréshez használt QT-s idõmérõ
	bool processing; // : Van-e épp folyamatban feldolgozás
	void run() Q_DECL_OVERRIDE;
	bool calcFPS();
signals:
	void pFinished(ImageProcessor*, ImageProcessor::ipData);
	void procFPSChanged(int);
};

#endif //PROCESSORTHREAD_H