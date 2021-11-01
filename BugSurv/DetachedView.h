/*
** @project			BugSurv Framework
** @file			DetachedView.h
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		A külön ablakban megjelenítéshez használt felület osztálydeklarációja.
*/

#ifndef DETACHEDVIEW_H
#define DETACHEDVIEW_H

// $ Generált UI
#include "ui_detachedview.h"

// < QT
#include <QCloseEvent>
#include <QResizeEvent>
#include <QWidget>
// >

// < Saját osztályok
#include "CVImageWidget.h"
// >

// < OpenCV
#include "opencv2/opencv.hpp" 
// >

using namespace cv;

class DetachedView :
	public QWidget
{
	Q_OBJECT
public:
	DetachedView(void);
	~DetachedView(void);
	void forceClose();
protected:
	Ui::detachedView ui;
	CVImageWidget cvWidget; // : Ebben jelenik meg a kép
	bool closeAble; // : bezárható az ablak, vagy sem
	double lastAR; // : elõzõ frame képaránya
	void closeEvent( QCloseEvent* e);
	void DetachedView::resizeEvent(QResizeEvent *i_pEvent);
public slots:
	void changeImage(Mat img);
};

#endif // DETACHEDVIEW_H
