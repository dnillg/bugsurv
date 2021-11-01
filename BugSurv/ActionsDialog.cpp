/*
** @project			BugSurv Framework
** @file			ActionsDialog.cpp
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Az akciógombok felületének implementációja.
*/

#include "actionsdialog.h"

ActionsDialog::ActionsDialog()
{
	ui.setupUi(this);
}

ActionsDialog::~ActionsDialog()
{

}

void ActionsDialog::addAction(const string& name, const string& slotName){
	QPushButton* btn = new QPushButton();
	btn->setText(QString::fromStdString(name));

	objects.push_back(btn);
	ui.layout->addWidget(btn);

	setMaximumHeight(height()+29);
	setMinimumHeight(height()+29);

	QObject::connect(btn, SIGNAL(clicked()), this, SLOT(buttonClicked()));
}

void ActionsDialog::buttonClicked(){
	int size = objects.size();
	QObject* sender = QObject::sender();
	for(int i = 0; i < size; i++){
		if(objects[i] == sender){
			emit actionEmitted(i);
			i = size;
		}
	}
}
