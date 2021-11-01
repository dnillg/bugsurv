/*
** @project			BugSurv Framework
** @file			PTZControlGV.h
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		A manuális irányításhoz használható joystick control deklarációja.
*/

#ifndef PTZCONTROLGV_H
#define PTZCONTROLGV_H

// $ Konfiguráció
#include "cfg.h"

// < QT
#include <qgraphicsview.h>
#include <QMouseEvent>
#include <qmessagebox.h>
#include <qt_windows.h>
//>

#include <cmath>

#define PI 3.14159265

class PTZControlGV : public QGraphicsView
{
	Q_OBJECT
public:
	PTZControlGV();
	~PTZControlGV(void);
	void changeBG(bool en);
	void setPushControl(bool value);
	void setZoomEnabled(bool value);
	void setFocusEnabled(bool value);

	typedef enum {STOP, UP, UPLEFT, LEFT, DOWNLEFT, DOWN, DOWNRIGHT, RIGHT, UPRIGHT} PTdirection;
	typedef struct {
		PTdirection dir;
		byte speed;
	} PTdata;
	typedef enum {  FOCUS_STOP=0, FOCUS_IN=1, FOCUS_OUT=2 } FocusInstruction;
	typedef enum {  ZOOM_STOP=0, ZOOM_IN=1, ZOOM_OUT=2 } ZoomInstruction;
private:
	bool focusEnabled;
	short lastFocusOp;
	bool zoomEnabled;
	bool ptz_zoom;
	bool pushToControl;

	ZoomInstruction getZoomdata(int y);
	uint lastCommandTime;
	PTdata lastPTCommand;
	ZoomInstruction lastZoomCommand;

	PTdata getPTdata(int x, int y);
	bool performControl(PTdata r, bool forced = false);
	bool performControl(ZoomInstruction r, bool forced = false);
	void mouseMoveEvent(QMouseEvent *mouseEvent);
	void mousePressEvent(QMouseEvent *mouseEvent);
	void mouseReleaseEvent(QMouseEvent *mouseEvent);
	void wheelEvent(QWheelEvent *wheelEvent);
signals:
	void ptzPT(int direction, byte speed);
	void ptzFocus(int);
	void ptzZoom(int);
};
#endif //PTZCONTROLGV_H