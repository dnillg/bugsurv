/*
** @project			BugSurv Framework
** @file			ActionsDialog.cpp
** @author			Giczi D�niel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Az akci�gombok fel�let�hez tartoz� oszt�lydeklar�ci�.
*/

#ifndef ACTIONSDIALOG_H
#define ACTIONSDIALOG_H

// < QT
#include <QObject>
#include <QWidget>
#include <QPushButton>
// >

// $ Gener�lt GUI
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
	Ui::ActionsDialog ui; // : GUI QT �ltal gener�lt GUI elemek
	std::vector<QPushButton*> objects;
signals:
	void actionEmitted(int);
private slots:
	void buttonClicked();
};

#endif // ACTIONSDIALOG_H
