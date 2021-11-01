/*
** @project			BugSurv Framework
** @file			ChangeFpsDialog.h
** @author			Giczi D�niel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		A felv�teli FPS �rt�k explicit be�ll�t�s�hoz haszn�lt fel�let oszt�lydeklar�ci�ja
*/

#ifndef CHANGEFPSDIALOG_H
#define CHANGEFPSDIALOG_H

// $ Konfigur�ci�
#include "cfg.h"

// $ Gener�lt GUI
#include "ui_changefps.h"

// < QT
#include <QDialog>
#include <QMessageBox>
// >

class ChangeFpsDialog :
	public QDialog
{
	Q_OBJECT
public:
	ChangeFpsDialog(void);
	~ChangeFpsDialog(void);
private:
	Ui::FpsChangeDialog ui; // : GUI QT �ltal gener�lt GUI elemek
private slots:
	void okBtnClicked();
	void cancelBtnClicked();
signals:
	void setFps(int); // Megv�ltozott az FPS
};

#endif