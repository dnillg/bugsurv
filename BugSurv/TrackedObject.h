#pragma once
#include <opencv2\opencv.hpp>

class TrackedObject
{
protected:
	unsigned int identifier;
	CvPoint lastCenter;
	unsigned short points;
	unsigned short targetPoints;
	unsigned long lastFrameNo;
	static unsigned int nextIdentifier;
public:
	TrackedObject(const CvPoint& _center, const unsigned long& _frameCount, const unsigned short& _initPoints = 1, const unsigned short& _targetPoints = 3);
	int hit(const CvPoint& _center, const unsigned long& _frameCount, const unsigned short& _extraPoints = 1);
	double getDistance(const CvPoint& _point);
	~TrackedObject(void);
	const unsigned long& getLastFrameNo();
	unsigned int getIdentifier();
	bool reachedTargetPoints();
};

