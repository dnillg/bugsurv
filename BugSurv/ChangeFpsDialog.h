/*
** @project			BugSurv Framework
** @file			ChangeFpsDialog.h
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		A felvételi FPS érték explicit beállításához használt felület osztálydeklarációja
*/

#ifndef CHANGEFPSDIALOG_H
#define CHANGEFPSDIALOG_H

// $ Konfiguráció
#include "cfg.h"

// $ Generált GUI
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
	Ui::FpsChangeDialog ui; // : GUI QT által generált GUI elemek
private slots:
	void okBtnClicked();
	void cancelBtnClicked();
signals:
	void setFps(int); // Megváltozott az FPS
};

#endif