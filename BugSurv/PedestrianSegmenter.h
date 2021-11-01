#pragma once
#include "Segmenter.h"

using namespace cv;

class PedestrianSegmenter :
	public Segmenter
{
	Q_OBJECT
public:
	PedestrianSegmenter(void);
	virtual Mat getForeground(Mat& croppedImage);
	virtual ~PedestrianSegmenter(void);
};

