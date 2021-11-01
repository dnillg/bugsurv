#include "ObjectTracker.h"

ObjectTracker::ObjectTracker(const unsigned short& _targetPoints, const unsigned short& _maxDistance, const unsigned short& _frameLimit)
	:targetPoints(_targetPoints), maxDistance(_maxDistance), frameLimit(_frameLimit)
{
}

ObjectTracker::~ObjectTracker(void)
{
}

// Tal�lat jelz�se
int ObjectTracker::hit(const CvPoint& _center, const unsigned long& _frameNo, const unsigned short& _points, const unsigned short& _targetPoints){
	cleanUp(_frameNo);
	unsigned short minDistance = (unsigned short)-1;
	unsigned short distance;

	// < Legk�zelebbi objektum keres�se
	list<TrackedObject>::iterator minIt;
	for(list<TrackedObject>::iterator it = queue.begin(); it != queue.end(); it++){
		distance = it->getDistance(_center);
		if(distance < minDistance && it->getLastFrameNo() != _frameNo){
			minIt = it;
			minDistance = distance;
		}
	}
	// >

	if(minDistance < maxDistance){
		// $ Ha el�g k�zel van, akkor pont hozz�ad�sa a legk�zelebbi objektumhoz
		return minIt->hit(_center, _frameNo, _points);
	} else {
		// < �j Objektum hozz�ad�sa
		queue.push_front(TrackedObject(_center, _frameNo, _points, (_targetPoints == (unsigned short)-1)?targetPoints:_targetPoints));
		TrackedObject& obj = *(queue.begin());
		return obj.reachedTargetPoints()?obj.getIdentifier():-1;
		// >
	}
}

// T�rlend� k�vetett objektumok t�rl�se
void ObjectTracker::cleanUp(const unsigned long& _frameNo){
	list<TrackedObject>::reverse_iterator it = queue.rbegin();
	while(!queue.empty() && _frameNo - (it)->getLastFrameNo() > frameLimit){
		queue.pop_back();
		it = queue.rbegin();
	}
}