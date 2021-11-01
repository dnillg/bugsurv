/*
** @project			BugSurv Framework
** @file			CVImageWidget.cpp
** @author			Giczi D�niel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		QT QWidget oszt�ly�nak egy sz�rmaztatott osz�ly defin�ci�ja, amely
		k�pes openCV form�tumban kapott k�pet megjelen�teni.
*/

#include "CVImageWidget.h"

CVImageWidget::CVImageWidget(QWidget *parent) : QWidget(parent), overrideEnabled(false), imageChanged(false), forceRecalcFlag(false) {
	// < Az els� megjelen�tett k�pkock�n�l v�ltozni fognak
	targetSize = QSize(INT_MIN,INT_MAX);
	targetRect = QRect(QPoint(0,0),targetSize);
	// >
}

// QT aj�nlott m�retre adott becsl�s�nek fel�ldefini�l�sa
QSize CVImageWidget::sizeHint() const { 
	return _qimage.size(); 
}

// QT minim�lis m�retre adott becsl�s�nek fel�ldefini�l�sa
QSize CVImageWidget::minimumSizeHint() const {
	return _qimage.size(); 
}

// Eredeti vagy a _qimage-ben szerepl� k�pet jelen�tse meg (igaz: _qimage)
void CVImageWidget::setOverrideEnabled(bool value){
	overrideEnabled = value;
}

// K�p megjelen�t�se QT form�tumb�l
void CVImageWidget::showImage(QImage& image){
	_qimage = QImage(image);
	calcRectangle(image.width(), image.height());
	#ifdef LOWRESOURCES
		imageChanged = true;
	#endif
	update();
}

// K�p megjelen�t�se openCV form�tumb�l
void CVImageWidget::showImage(cv::Mat& image) {
	mutex.lock();
    // < �tkonvert�l�s RGB888 form�tumra (QT)
    switch (image.type()) {
    case CV_8UC1:
        cvtColor(image, _tmp, CV_GRAY2RGB);
        break;
    case CV_8UC3:
        cvtColor(image, _tmp, CV_BGR2RGB);
        break;
    }
	// >

    // QImage needs the data to be stored continuously in memory
    assert(_tmp.isContinuous());

    _qimage = QImage(_tmp.data, _tmp.cols, _tmp.rows, _tmp.cols*3, QImage::Format_RGB888);

	calcRectangle(_qimage.width(), _qimage.height());

	#ifdef LOWRESOURCES
		imageChanged = true;
	#endif
	update();
	mutex.unlock();
}

// Kisz�m�tja, hogyan f�r bele a legjobban a QWidgetbe a megjelen�tend� k�p
void CVImageWidget::calcRectangle(int w, int h){
	if( forceRecalcFlag || qAbs((double)w/h - (double)targetSize.width()/targetSize.height()) > DBL_EPSILON){
		forceRecalcFlag = false;
		if(w != targetSize.width() || h != targetSize.height()){
			targetSize = QSize(w,h);
			if(targetSize.height()*((double)this->width())/this->height() < targetSize.width()){
				double ratio = ((double)targetSize.width())/targetSize.height();
				targetSize.setWidth(this->width());
				targetSize.setHeight(qRound(this->width()/ratio));
			} else {
				double ratio = ((double)targetSize.width())/targetSize.height();
				targetSize.setHeight(this->height());
				targetSize.setWidth(qRound(this->height()*ratio));
			}
			QPoint point;
			point.setX((this->width()-targetSize.width())/2);
			point.setY((this->height()-targetSize.height())/2);
			targetRect = QRect(point, targetSize);
		}
	}
}

// PaintEvent fel�ldefini�l�sa (k�pfriss�t�s)
void CVImageWidget::paintEvent(QPaintEvent* event) {
        // Display the image
		if(overrideEnabled 
			#ifdef LOWRESOURCES
				&& imageChanged
			#endif
			){
			#ifdef LOWRESOURCES
				imageChanged = false;
			#endif
			QPainter painter(this);
			painter.drawImage(targetRect, _qimage);
			painter.end();
		}
    }

// Explicit k�r�s a renderel�shez haszn�lt t�glalap �jrasz�mol�s�ra.
void CVImageWidget::forceRecalc(){
	forceRecalcFlag = true;
}