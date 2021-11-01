#pragma once
#include <list>
#include "TrackedObject.h"
using namespace std;

class ObjectTracker
{
public:
	
protected:
	list<TrackedObject> queue; // : Aktu�lisan sz�montartott objektumok
	unsigned short frameLimit; // : Frame-k sz�ma, ameddig az objektumok sz�mon vannak tartva
	unsigned short targetPoints; // : Alap�rt�k, amelyet el kell �rni�k az objektumoknak (pontok)
	unsigned short maxDistance; // : Maxim�lis t�vols�g a k�vet�shez

public:
	ObjectTracker(const unsigned short& _targetPoints, const unsigned short& _maxDistance, const unsigned short& _frameLimit);
	~ObjectTracker(void);
	int hit(const CvPoint& _center, const unsigned long& _frameNo, const unsigned short& _points = 1, const unsigned short& _targetPoints = -1);
	void cleanUp(const unsigned long& _frameNo);
};

