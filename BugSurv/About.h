/*
** @project			BugSurv Framework
** @file			About.h
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		A névjegy felület osztály deklarációja.
*/

#ifndef ABOUT_H
#define ABOUT_H

// $ Generált GUI
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
	Ui::About ui; // : GUI QT által generált GUI elemek
};

#endif // ABOUT_H