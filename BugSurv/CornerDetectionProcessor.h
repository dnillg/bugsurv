/*
** @project			BugSurv Framework
** @file			CornerDetection.h
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Példa egy képfeldolgozó deklarációjára.
		Sarokdetektációst algoritmust valósít meg küszöbértékkel.
*/

#ifndef CORNERDETECTIONPROCESSOR_H
#define CORNERDETECTIONPROCESSOR_H

// $ Õsosztály
#include "ImageProcessor.h"

#include "MorphoFeatures.h"

using namespace std;

class CornerDetectionProcessor : public ImageProcessor
{
protected:
	Q_OBJECT
public:
	CornerDetectionProcessor();
	virtual std::string getName();
	virtual ImageProcessor* clone();
	virtual ~CornerDetectionProcessor(void);
	CornerDetectionProcessor(const CornerDetectionProcessor&);
protected:
	// threshold to produce binary image
	int threshold;
	// structuring elements used in corner detection
	cv::Mat cross;
	cv::Mat diamond;
	cv::Mat square;
	cv::Mat x;
	void addParams();
	void applyThreshold(cv::Mat& result);
	cv::Mat getEdges(const cv::Mat &image);
	cv::Mat getCorners(const cv::Mat &image);
	void drawOnImage(const cv::Mat& binary, cv::Mat& image);
public slots:
	virtual void process();
protected slots:
};
#endif // CORNERDETECTIONPROCESSOR_H