/*
** @project			BugSurv Framework
** @file			About.cpp
** @author			Giczi D�niel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		A n�vjegy fel�let oszt�ly defin�ci�ja.
*/

#include "About.h"

About::About(void)
{
	ui.setupUi(this);
	QObject::connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(close()));
	this->setWindowFlags(Qt::WindowCloseButtonHint);
}

About::~About(void)
{}
