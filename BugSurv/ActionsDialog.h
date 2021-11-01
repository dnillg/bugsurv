/*
** @project			BugSurv Framework
** @file			ActionsDialog.cpp
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Az akciógombok felületéhez tartozó osztálydeklaráció.
*/

#ifndef ACTIONSDIALOG_H
#define ACTIONSDIALOG_H

// < QT
#include <QObject>
#include <QWidget>
#include <QPushButton>
// >

// $ Generált GUI
#include "ui_actions.h"

using namespace std;

class ActionsDialog : public QWidget
{
	Q_OBJECT

public:
	ActionsDialog();
	~ActionsDialog();
	void addAction(const std::string&, const std::string&);

private:
	Ui::ActionsDialog ui; // : GUI QT által generált GUI elemek
	std::vector<QPushButton*> objects;
signals:
	void actionEmitted(int);
private slots:
	void buttonClicked();
};

#endif // ACTIONSDIALOG_H
