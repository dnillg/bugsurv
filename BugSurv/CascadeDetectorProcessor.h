/*
** @project			BugSurv Framework
** @file			CascadeDetectorProcessor.h
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Példa egy képfeldolgozó deklarációjára.
		Kontúrok kereséset implementálja.
*/

#ifndef CASCADEDETECTORPROCESSOR_H
#define CASCADEDETECTORPROCESSOR_H

// $ Õsosztály
#include "ImageProcessor.h"

#include "Cv3Functions.h"
#include "PedestrianSegmenter.h"

class CascadeDetectorProcessor : public ImageProcessor
{
protected:
	Q_OBJECT
public:
	CascadeDetectorProcessor(const QString _cascadeFile = QString::null, double _scaleFactor = 1.3, int _minNeighbors = 3);
	virtual ImageProcessor* clone();
	virtual ~CascadeDetectorProcessor(void);
	CascadeDetectorProcessor(const CascadeDetectorProcessor&);
protected:
	QString cascadeFile; // : Kaszkád fájl a detektáláshoz
	double scaleFactor;
	bool cInitialized;
	CascadeClassifier classifier;
	Segmenter* segmenter;
	int minNeighbors;
	virtual void addParams();
	virtual void addActions();
	vector<Rect> CascadeDetectorProcessor::detect(const Mat& image);
	vector<Rect> extendRects(const vector<Rect>& rects, const double factor);
	void initDetector();
	void drawRects(Mat& image, const vector<Rect>& rects, const Scalar color);
	void CascadeDetectorProcessor::drawSegments(Mat& image, vector<Rect> rects);
	Mat grayscale;
public slots:
	virtual void process();
protected slots:
	void reinitialize();
};
#endif // CASCADEDETECTORPROCESSOR_H