/*
** @project			BugSurv Framework
** @file			CVImageWidget.h
** @author			Giczi D�niel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		QT QWidget oszt�ly�nak egy sz�rmaztatott osz�ly deklar�ci�ja, amely
		k�pes openCV form�tumban kapott k�pet megjelen�teni.
*/

#ifndef CVIMAGEWIDGET_H
#define CVIMAGEWIDGET_H

// $ Konfigur�ci�
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
	QSize targetSize; // : renderel�shez sz�m�tott m�ret
	QRect targetRect; // : a renderel�shez haszn�lt t�glalap
    QImage _qimage; // : akut�lis k�p QT form�tumban
    cv::Mat _tmp; // : aktu�lis k�p openCV form�tumban
	bool forceRecalcFlag; // : renderel�shez sz�m�tott t�glalap �jrasz�mol�s�t k�r�s�t jelz� flag
	QMutex mutex; // : t�bb sz�l ne tudjon egyszerre k�pet megjelen�teni
	bool imageChanged; // : a megjelen�tett k�p megv�ltozott
    bool overrideEnabled; // : a widget eredeti k�pe helyett megjelenhet m�s
	void calcRectangle(int w, int h);
    void paintEvent(QPaintEvent*);
};

#endif //CVIMAGEWIDGET_H