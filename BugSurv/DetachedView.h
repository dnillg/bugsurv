/*
** @project			BugSurv Framework
** @file			DetachedView.h
** @author			Giczi D�niel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		A k�l�n ablakban megjelen�t�shez haszn�lt fel�let oszt�lydeklar�ci�ja.
*/

#ifndef DETACHEDVIEW_H
#define DETACHEDVIEW_H

// $ Gener�lt UI
#include "ui_detachedview.h"

// < QT
#include <QCloseEvent>
#include <QResizeEvent>
#include <QWidget>
// >

// < Saj�t oszt�lyok
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
	CVImageWidget cvWidget; // : Ebben jelenik meg a k�p
	bool closeAble; // : bez�rhat� az ablak, vagy sem
	double lastAR; // : el�z� frame k�par�nya
	void closeEvent( QCloseEvent* e);
	void DetachedView::resizeEvent(QResizeEvent *i_pEvent);
public slots:
	void changeImage(Mat img);
};

#endif // DETACHEDVIEW_H
