#include "TrackedObject.h"

TrackedObject::TrackedObject(const CvPoint& _center, const unsigned long& _frameCount, const unsigned short& _initPoints, const unsigned short& _targetPoints)
	:lastFrameNo(_frameCount),points(_initPoints),targetPoints(_targetPoints),lastCenter(_center),identifier(nextIdentifier++)
{}

int TrackedObject::hit(const CvPoint& _center, const unsigned long& _frameCount, const unsigned short& _extraPoints){
	lastCenter = _center;
	lastFrameNo = _frameCount;
	points += _extraPoints;
	return (points >= targetPoints)?identifier:-1;
}

TrackedObject::~TrackedObject(void)
{
}

double TrackedObject::getDistance(const CvPoint& _point){
	CvPoint coordDistance = cvPoint(lastCenter.x-_point.x, lastCenter.y-_point.y);
	return sqrt((coordDistance.x*coordDistance.x)+(coordDistance.y*coordDistance.y));
}

const unsigned long& TrackedObject::getLastFrameNo(){
 	return lastFrameNo;
}

unsigned int TrackedObject::getIdentifier(){
	return identifier;
}

bool TrackedObject::reachedTargetPoints(){
	return (targetPoints <= points);
}

unsigned int TrackedObject::nextIdentifier = 0;