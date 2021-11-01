/*
** @project			BugSurv Framework
** @file			main.cpp
** @author			Giczi D�niel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		A program bel�p�si pontj�t tartalmaz� f�jl. L�trehoz egy QApplication �s egy BugSurv (form) objektumot.
		A QT-s f�gg�s�gek el�r�si hely�hez hozz�adja a program ind�t�si mapp�j�t, megjelen�ti a formot
		�s elind�tja a QT-s alkalmaz�st.
*/

#include "cfg.h"
#include <QtWidgets/QApplication>
#include "bugsurv.h"
#include <QLibraryInfo>
#include "ipClasses.h"

// $ OpenCV
#include "opencv2/opencv.hpp" 

// < Memleak test - csak debug m�dban m�k�dik
 #if defined(WIN32) && defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
// >

int main(int argc, char *argv[])
{
	#if defined(_MSC_VER) && defined(_DEBUG)
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif
	qRegisterMetaType<ImageProcessor::ipData>("ImageProcessor::ipData");
	qRegisterMetaType<cv::Mat>("Mat");
	QApplication a(argc, argv);

	//<Alkalmaz�s futtat�si k�nyvt�r�nak hozz�ad�sa a k�ls� k�nyvt�rak el�r�si �tak list�j�hoz
	QString path;
	path = QCoreApplication::applicationDirPath();
	QCoreApplication::addLibraryPath(path);
	//> 

	//Feldolgoz�k hozz�ad�sa
	initImageProcessors();

	//<Form megnyit�sa, alkalmaz�s elind�t�sa
	BugSurv w;
	w.show();
	return a.exec();
	//>
}
