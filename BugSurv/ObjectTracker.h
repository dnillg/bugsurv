#pragma once
#include <list>
#include "TrackedObject.h"
using namespace std;

class ObjectTracker
{
public:
	
protected:
	list<TrackedObject> queue; // : Aktuálisan számontartott objektumok
	unsigned short frameLimit; // : Frame-k száma, ameddig az objektumok számon vannak tartva
	unsigned short targetPoints; // : Alapérték, amelyet el kell érniük az objektumoknak (pontok)
	unsigned short maxDistance; // : Maximális távolság a követéshez

public:
	ObjectTracker(const unsigned short& _targetPoints, const unsigned short& _maxDistance, const unsigned short& _frameLimit);
	~ObjectTracker(void);
	int hit(const CvPoint& _center, const unsigned long& _frameNo, const unsigned short& _points = 1, const unsigned short& _targetPoints = -1);
	void cleanUp(const unsigned long& _frameNo);
};

