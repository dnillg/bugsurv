/*
** @project			BugSurv Framework
** @file			CascadeDetectorProcessor.h
** @author			Giczi D�niel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		P�lda egy k�pfeldolgoz� deklar�ci�j�ra.
		Kont�rok keres�set implement�lja.
*/

#ifndef CASCADEDETECTORPROCESSOR_H
#define CASCADEDETECTORPROCESSOR_H

// $ �soszt�ly
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
	QString cascadeFile; // : Kaszk�d f�jl a detekt�l�shoz
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