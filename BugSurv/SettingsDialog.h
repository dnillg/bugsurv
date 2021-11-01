/*
** @project			BugSurv Framework
** @file			SettingsDialog.h
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		A paraméterezés interfészét megvalósító osztály deklarációja.
*/

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

// $ Generált GUI
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
	Ui::SettingsDialog ui; // : GUI QT által generált GUI elemek
	typedef struct {
		QLabel* paramNameLabel; // : Paraméternév Label
		QLabel* paramTypeLabel; // : Paramétertípus Label
		QWidget* paramValueControl; // : A Widget, amiben a paraméter értéke van
		QVariant::Type type; // : Paraméter típusa (enum)
		bool editable; // : A paraméter értéke módosítható
	} GuiParam;
	std::vector<GuiParam> guiParams;
};

#endif