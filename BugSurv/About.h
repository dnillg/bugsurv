/*
** @project			BugSurv Framework
** @file			About.h
** @author			Giczi D�niel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		A n�vjegy fel�let oszt�ly deklar�ci�ja.
*/

#ifndef ABOUT_H
#define ABOUT_H

// $ Gener�lt GUI
#include "ui_about.h"

// < QT
#include <QDialog>
// >

class About :
	public QDialog
{
	Q_OBJECT
public:
	About(void);
	~About(void);
private:
	Ui::About ui; // : GUI QT �ltal gener�lt GUI elemek
};

#endif // ABOUT_H