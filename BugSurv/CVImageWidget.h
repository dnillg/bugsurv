/*
** @project			BugSurv Framework
** @file			CVImageWidget.h
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		QT QWidget osztályának egy származtatott oszály deklarációja, amely
		képes openCV formátumban kapott képet megjeleníteni.
*/

#ifndef CVIMAGEWIDGET_H
#define CVIMAGEWIDGET_H

// $ Konfiguráció
#include "cfg.h"

// < QT
#include <QWidget>
#include <QImage>
#include <QPainter>
// >

// $ OpenCV
#include <opencv2/opencv.hpp>

class CVImageWidget : public QWidget
{
    Q_OBJECT
public:
	CVImageWidget(QWidget *parent = 0);

    QSize sizeHint() const;
    QSize minimumSizeHint() const;
	void setOverrideEnabled(bool value);
	void forceRecalc();
public slots:
	void showImage(QImage& image);
    void showImage(cv::Mat& image);
protected:
	QSize targetSize; // : rendereléshez számított méret
	QRect targetRect; // : a rendereléshez használt téglalap
    QImage _qimage; // : akutális kép QT formátumban
    cv::Mat _tmp; // : aktuális kép openCV formátumban
	bool forceRecalcFlag; // : rendereléshez számított téglalap újraszámolását kérését jelzõ flag
	QMutex mutex; // : több szál ne tudjon egyszerre képet megjeleníteni
	bool imageChanged; // : a megjelenített kép megváltozott
    bool overrideEnabled; // : a widget eredeti képe helyett megjelenhet más
	void calcRectangle(int w, int h);
    void paintEvent(QPaintEvent*);
};

#endif //CVIMAGEWIDGET_H