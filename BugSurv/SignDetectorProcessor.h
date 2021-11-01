#pragma once
#include "cascadedetectorprocessor.h"
#include "ComponentFilter.h"
#include "ObjectTracker.h"

class SignDetectorProcessor : public CascadeDetectorProcessor
{
public:
	enum SignColor {RED, BLUE, YELLOW, WHITE, NONE};
	SignDetectorProcessor(const QString _cascadeFile = QString::null, double _scaleFactor = 1.3,
		int _minNeighbors = 3, SignColor _signColor = NONE, bool _colorFilterEnabled = false,
		int _minComponentSize = 30, int _maxComponentDistance = 50, const QString _annotationFile = QString::null,
		const unsigned short& _frameLimit = 12, const unsigned short& _maxFollowDistance = 165, 
		const unsigned short& targetPoints = 8);
	virtual ImageProcessor* clone();
	SignDetectorProcessor(const SignDetectorProcessor&);
	virtual ~SignDetectorProcessor(void);
	static SignColor parseColor(string color);
protected:
	SignColor signColor;
	bool cfEnabled;
	ComponentFilter cFilter;
	bool cfInitialized;
	int minComponentSize;
	int maxComponentDistance;
	ObjectTracker tracker;
	QMutex annotationMutex;
	QString annotationFile;
	unsigned short frameLimit;
	unsigned short maxFollowDistance;
	unsigned short targetPoints;
	virtual void addParams();
	virtual void addActions();
	void initComponentFilter();
	vector<bool> filterByComponents(const vector<Rect>& rects, const vector<ComponentFilter::Component>& components);
protected slots:
		virtual void reinitialize();
public slots:
	virtual void process();
};

