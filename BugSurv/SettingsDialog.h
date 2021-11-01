/*
** @project			BugSurv Framework
** @file			SettingsDialog.h
** @author			Giczi D�niel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		A param�terez�s interf�sz�t megval�s�t� oszt�ly deklar�ci�ja.
*/

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

// $ Gener�lt GUI
#include "ui_settings.h"

// < QT
#include <QDialog> 
#include <QLineEdit>
#include <QCheckBox>
// >

class SettingsDialog : public QDialog
{
public:
	SettingsDialog(QWidget* parent = 0);
	~SettingsDialog(void);
	void addParam(const std::string& name, QVariant value, bool editable = true);
	std::vector<QVariant> getValues();
	void updateParam(int, const void* const);
protected:
	Ui::SettingsDialog ui; // : GUI QT �ltal gener�lt GUI elemek
	typedef struct {
		QLabel* paramNameLabel; // : Param�tern�v Label
		QLabel* paramTypeLabel; // : Param�tert�pus Label
		QWidget* paramValueControl; // : A Widget, amiben a param�ter �rt�ke van
		QVariant::Type type; // : Param�ter t�pusa (enum)
		bool editable; // : A param�ter �rt�ke m�dos�that�
	} GuiParam;
	std::vector<GuiParam> guiParams;
};

#endif