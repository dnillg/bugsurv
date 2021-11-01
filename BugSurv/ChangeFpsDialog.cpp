/*
** @project			BugSurv Framework
** @file			ChangeFpsDialog.cpp
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		A felvételi FPS érték explicit beállításához használt felület implementációja.
*/

#include "ChangeFpsDialog.h"

ChangeFpsDialog::ChangeFpsDialog(void)
{
	ui.setupUi(this);
	this->setWindowFlags(Qt::WindowCloseButtonHint);
	QObject::connect(ui.okBtn,SIGNAL(clicked()),this,SLOT(okBtnClicked()));
	QObject::connect(ui.cancelBtn,SIGNAL(clicked()),this,SLOT(cancelBtnClicked()));
}

// A dialog elfogadását és a mező értékének vizsálatát követően visszaküldjük az értéket.
void ChangeFpsDialog::okBtnClicked(){
	int parsedValue = ui.fpsValueLineEdit->text().toInt();
	if(parsedValue > 0 && parsedValue <= 120){
		emit setFps(parsedValue);
		close();
	} else {
		QMessageBox msg;
		msg.setText("A beírt FPS érték nem megfelelő!\n(pozitív egész szám)");
		msg.setWindowIcon(QIcon(":/files/Resources/ikon.png"));
		msg.setIcon(QMessageBox::Icon::Information);
		msg.exec();
	}
}

// A dialogot elutasítjuk.
void ChangeFpsDialog::cancelBtnClicked(){
	close();
}

ChangeFpsDialog::~ChangeFpsDialog(void)
{
}
