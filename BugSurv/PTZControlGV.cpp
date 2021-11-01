/*
** @project			BugSurv Framework
** @file			PTZControlGV.cpp
** @author			Giczi D�niel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		A manu�lis ir�ny�t�shoz haszn�lhat� joystick control defin�c�ja.
*/

#include "PTZControlGV.h"

PTZControlGV::PTZControlGV():
	lastCommandTime(0), pushToControl(true), zoomEnabled(true), lastFocusOp(FOCUS_STOP), focusEnabled(0)
{
	lastPTCommand.speed = -1;
	lastZoomCommand = ZOOM_STOP;
	setStyleSheet("background-image: url(':/files/Resources/joyd.png');");
	resize(QSize(400,400));
	setMaximumSize(QSize(400,400));
	setInteractive(true);
	setEnabled(false);
}

// Visszaadja, hogy milyen zoom m�velet hely�n van az eg�r poz�ci�ja.
PTZControlGV::ZoomInstruction PTZControlGV::getZoomdata(int y){
	int speed = abs(y-200)/30;
	if(speed){
		return (y-200<0)?ZOOM_IN:ZOOM_OUT;
	}
	return ZOOM_STOP;
}

// MouseMoveEvent fel�ldefini�l�sa (eg�r mozgat�sa).
void PTZControlGV::mouseMoveEvent(QMouseEvent *mouseEvent){
	if(!ptz_zoom){
		// $ Ha a legutols� kattint�s nem zoom(jobb) volt
		performControl(getPTdata(mouseEvent->x(),mouseEvent->y()));
	} else if(zoomEnabled) {
		// $ Ha a legutols� kattint�s zoom(jobb) volt �s a zoom enged�lyezett
		performControl(getZoomdata(mouseEvent->y()));
	}
}

// MousePressEvent fel�ldefini�l�sa (gomb lenyom�sa).
void PTZControlGV::mousePressEvent(QMouseEvent *mouseEvent){
	if(mouseEvent->button() == Qt::LeftButton){
		ptz_zoom = false;
		performControl(getPTdata(mouseEvent->x(),mouseEvent->y()));
	} else if(mouseEvent->button() == Qt::RightButton && zoomEnabled){
		ptz_zoom = true;
		setStyleSheet("background-image: url(':/files/Resources/joyz.png');");
		performControl(getZoomdata(mouseEvent->y()));
	}
}

// MouseReleaseEvent fel�ldefini�l�sa (gomb felenged�se).
void PTZControlGV::mouseReleaseEvent(QMouseEvent *mouseEvent){
	if(mouseEvent->button() == Qt::LeftButton){
		// < Pan-tilt m�vleet
		ptz_zoom = false;
		PTdata r;
		if(pushToControl){
			// $ Ha nyomva kell tartani az ir�ny�t�shoz, akkor meg�ll
			r.dir=PTdirection::STOP;
			r.speed=0;
		} else {
			r = getPTdata(mouseEvent->x(),mouseEvent->y());
		}
		performControl(r, true);
		// >
	} else if(mouseEvent->button() == Qt::RightButton && zoomEnabled){
		// < Zoom m�velet
		ptz_zoom = true;
		setStyleSheet("background-image: url(':/files/Resources/joy.png');");
		ZoomInstruction r = getZoomdata(mouseEvent->y());
		if(pushToControl) r = ZOOM_STOP;
		performControl(r,true);
		// >
	}
}

// V�grehajt�sra k�ldi vagy elnyeli a Zoom m�veletet.
bool PTZControlGV::performControl(ZoomInstruction r, bool forced){
	if((GetTickCount() - lastCommandTime > 66 && lastZoomCommand != r) || forced){
		lastCommandTime = GetTickCount();
		lastZoomCommand = r;
		emit ptzZoom(r);			
	}
	return false;
}

// V�grehajt�sra k�ldi vagy elnyeli a pan-tilt m�veletet.
bool PTZControlGV::performControl(PTdata r, bool forced){
	
	if((GetTickCount() - lastCommandTime > 66 && (lastPTCommand.dir != r.dir || lastPTCommand.speed != r.speed)) || forced){
		lastCommandTime = GetTickCount();
		lastPTCommand.dir = r.dir;
		lastPTCommand.speed = r.speed;
		if(!r.speed) r.dir = PTdirection::STOP;
		emit ptzPT(r.dir, r.speed);
		return true;
	}
	return false;
}

// H�tt�rk�p v�lt�sa enged�lyezetts�g szerint.
void PTZControlGV::changeBG(bool en){
	if(en){
		setStyleSheet("background-image: url(':/files/Resources/joy.png');");
	} else {
		setStyleSheet("background-image: url(':/files/Resources/joyd.png');");
	}
}

// S�kgeomertria seg�ts�g�vel megmondja, hogy melyik szeletben van az eg�r.
PTZControlGV::PTdata PTZControlGV::getPTdata(int x, int y){
	PTdata rval;
	double distance;
	int disX, disY;
	disX = x-200;
	disY = y-200;
	bool posiX = (disX < 0)?false:true;
	bool posiY = (disY < 0)?false:true;
	disY = abs(disY);
	disX = abs(disX);

	distance = sqrt(pow(disX,2)+pow(disY,2));
	if(distance > 180) distance = 0;
	rval.speed = (int)(distance/30);

	int degree = -1;
	if(posiX){
		if(posiY){
			degree = 180+180/PI*atan(((double)disX)/disY);
		} else {
			degree = 270+180/PI*atan(((double)disY)/disX);
		}
	} else {
		if(posiY){
			degree = 90+180/PI*atan(((double)disY)/disX);
		} else {
			degree = 180/PI*atan(((double)disX)/disY);
		}
	}

	if(degree > 337.5){
		rval.dir = UP;
	} else if(degree > 292.5){
		rval.dir = UPRIGHT;
	} else if(degree > 247.5){
		rval.dir = RIGHT;
	} else if(degree > 202.5){
		rval.dir = DOWNRIGHT;
	} else if(degree > 157.5){
		rval.dir = DOWN;
	} else if(degree > 112.5){
		rval.dir = DOWNLEFT;
	} else if(degree > 67.5){
		rval.dir = LEFT;
	} else if(degree > 22.5){
		rval.dir = UPLEFT;
	} else {
		rval.dir = UP;
	}

	return rval;
}

// WheelEvent fel�ldefini�l�sa (f�kusz)
void PTZControlGV::wheelEvent(QWheelEvent *wheelEvent){
	QPoint pnt = wheelEvent->angleDelta();
	if(focusEnabled){
		if(lastFocusOp == FOCUS_STOP && pnt.y() > 0){
			emit ptzFocus(FOCUS_IN);
			lastFocusOp=FOCUS_IN;
		} else if((lastFocusOp == FOCUS_IN && pnt.y() < 0) || (lastFocusOp == FOCUS_OUT && pnt.y() > 0)){
			emit ptzFocus(FOCUS_STOP);
			lastFocusOp=FOCUS_STOP;
		} else if(lastFocusOp == FOCUS_STOP && pnt.y() < 0){
			emit ptzFocus(FOCUS_OUT);
			lastFocusOp=FOCUS_OUT;
		}
	}
}

// Be�ll�tja, hogy tartani kell-e a gombot az ir�ny�t�shoz (igaz: tartani kell).
void PTZControlGV::setPushControl(bool value){
	pushToControl = value;
}

// Be�ll�tja, hogy enged�lyezett-e a manu�lis zoom (igaz: igen).
void PTZControlGV::setZoomEnabled(bool value){
	zoomEnabled = value;
}

// Be�ll�tja, hogy enged�lyezett-e a f�kusz (igaz: igen).
void PTZControlGV::setFocusEnabled(bool value){
	focusEnabled = value;
}


PTZControlGV::~PTZControlGV(void)
{}
