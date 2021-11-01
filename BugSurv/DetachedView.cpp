/*
** @project			BugSurv Framework
** @file			DetachedView.cpp
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		A külön ablakban megjelenítéshez használt felület implementációja.
*/

#include "DetachedView.h"

DetachedView::DetachedView(void):
	closeAble(false),
	lastAR(0)
{
	ui.setupUi(this);
	cvWidget.setParent(ui.widget);
	cvWidget.setFixedWidth(480);
	cvWidget.setFixedHeight(384);
	cvWidget.setOverrideEnabled(true);
}


DetachedView::~DetachedView(void)
{

}

// Kép beállítása, szüksége esetén átméretezés.
void DetachedView::changeImage(Mat img){
	CvSize size = img.size();
	double aspectRatio = ((double)size.width)/size.height;
	if( aspectRatio - lastAR > DBL_EPSILON){
		lastAR = aspectRatio;
		int newHeight = 480/aspectRatio;
		setMinimumHeight(newHeight);
		setFixedHeight(newHeight);
		setMaximumHeight(480);
		setMaximumWidth(640);
		cvWidget.setFixedHeight(newHeight);
		cvWidget.forceRecalc();
	}
	cvWidget.showImage(img);

}

// Bezárás blokkolásához felülbírált closeEvent.
void DetachedView::closeEvent( QCloseEvent* e){
	if(!closeAble)
		e->ignore();
	else
		e->accept();
}

// Átméretezéskor a belsõ widget is méretezõdik.
void DetachedView::resizeEvent(QResizeEvent *e){
	cvWidget.setOverrideEnabled(false);
	ui.widget->setFixedSize(e->size());
	cvWidget.setFixedSize(e->size());
	cvWidget.forceRecalc();
	cvWidget.setOverrideEnabled(true);
}

// Ablak tényleges bezárása.
void DetachedView::forceClose(){
	bool old = closeAble;
	closeAble = true;
	close();
	closeAble = old;

}