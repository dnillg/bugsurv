/*
** @project			BugSurv Framework
** @file			main.cpp
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		A program belépési pontját tartalmazó fájl. Létrehoz egy QApplication és egy BugSurv (form) objektumot.
		A QT-s függõségek elérési helyéhez hozzáadja a program indítási mappáját, megjeleníti a formot
		és elindítja a QT-s alkalmazást.
*/

#include "cfg.h"
#include <QtWidgets/QApplication>
#include "bugsurv.h"
#include <QLibraryInfo>
#include "ipClasses.h"

// $ OpenCV
#include "opencv2/opencv.hpp" 

// < Memleak test - csak debug módban mûködik
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

	//<Alkalmazás futtatási könyvtárának hozzáadása a külsõ könyvtárak elérési útak listájához
	QString path;
	path = QCoreApplication::applicationDirPath();
	QCoreApplication::addLibraryPath(path);
	//> 

	//Feldolgozók hozzáadása
	initImageProcessors();

	//<Form megnyitása, alkalmazás elindítása
	BugSurv w;
	w.show();
	return a.exec();
	//>
}
