/*
** @project			BugSurv Framework
** @file			SettingsDialog.cpp
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		A paraméterezés interfészét megvalósító osztály implementáció.
*/

#include "SettingsDialog.h"

SettingsDialog::SettingsDialog(QWidget* parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::WindowCloseButtonHint);
}


SettingsDialog::~SettingsDialog(void)
{
	int size = guiParams.size();
	for(int idxA = 0; idxA < size; idxA++){
		delete guiParams[idxA].paramNameLabel;
		delete guiParams[idxA].paramValueControl;
		delete guiParams[idxA].paramTypeLabel;
	}
}

void SettingsDialog::addParam(const std::string& name, QVariant value, bool editable){
	GuiParam newParam;
	newParam.paramNameLabel = new QLabel(QString::fromStdString(name));
	newParam.paramNameLabel->setMinimumHeight(20);
	newParam.type = value.type();
	newParam.paramTypeLabel = new QLabel();
	newParam.paramTypeLabel->setMinimumHeight(20);
	switch (value.type()){
		case QVariant::Bool:
			newParam.paramValueControl = new QCheckBox();	
			newParam.paramValueControl->setMinimumHeight(20);
			((QCheckBox*)newParam.paramValueControl)->setChecked(value.toBool());
			newParam.paramTypeLabel->setText("Bool");
			break;
		case QVariant::String:
			newParam.paramValueControl = new QLineEdit();
			((QLineEdit*)newParam.paramValueControl)->setText(value.toString());
			newParam.paramTypeLabel->setText("String");
			break;
		case QVariant::Int:
			newParam.paramValueControl = new QLineEdit();
			((QLineEdit*)newParam.paramValueControl)->setText(QString::number(value.toInt()));
			newParam.paramTypeLabel->setText("Int");
			break;
		case QVariant::Double:
			newParam.paramValueControl = new QLineEdit();
			((QLineEdit*)newParam.paramValueControl)->setText(QString::number(value.toDouble()));
			newParam.paramTypeLabel->setText("Double");
			break;
	}
	if(!editable) newParam.paramValueControl->setEnabled(false);
	ui.paramNameList->addWidget(newParam.paramNameLabel);
	ui.paramTypeList->addWidget(newParam.paramTypeLabel);
	ui.paramValueList->addWidget(newParam.paramValueControl);
	guiParams.push_back(newParam);
	
	QRect rect = ui.dialogButtons->geometry();
	ui.dialogButtons->setGeometry(rect.left(), rect.top()+26,rect.width(),rect.height());

	setMaximumHeight(height()+26);
	setMinimumHeight(height()+26);
}

std::vector<QVariant> SettingsDialog::getValues(){
	std::vector<QVariant> values;
	for(std::vector<GuiParam>::iterator it = guiParams.begin(); it != guiParams.end(); it++){
		QVariant v;
		switch(it->type){
			case QVariant::Bool:
				v = ((QCheckBox*)it->paramValueControl)->isChecked();
				values.push_back(v);
				break;
			case QVariant::Int:
				v = ((QLineEdit*)it->paramValueControl)->text().toInt();
				values.push_back(v);
				break;
			case QVariant::Double:
				v = ((QLineEdit*)it->paramValueControl)->text().toDouble();
				values.push_back(v);
				break;
			case QVariant::String:
				v = ((QLineEdit*)it->paramValueControl)->text();
				values.push_back(v);
				break;
		}
	}
	return values;
}

void SettingsDialog::updateParam(int idx, const void* const pvar){
	QCheckBox* checkBoxPointer = 0;
	QLineEdit* lineEditPointer = 0;
	switch(guiParams[idx].type){
		case QVariant::Bool:
			checkBoxPointer = (QCheckBox*) guiParams[idx].paramValueControl;
			checkBoxPointer->setChecked(*((bool*)pvar));
			break;
		case QVariant::Int:
			lineEditPointer = (QLineEdit*) guiParams[idx].paramValueControl;
			lineEditPointer->setText(QString::number(*((int*)pvar)));
			break;
		case QVariant::Double:
			lineEditPointer = (QLineEdit*) guiParams[idx].paramValueControl;
			lineEditPointer->setText(QString::number(*((double*)pvar)));
			break;
		case QVariant::String:
			lineEditPointer = (QLineEdit*) guiParams[idx].paramValueControl;
			lineEditPointer->setText(*((QString*)pvar));
			break;
	}
}