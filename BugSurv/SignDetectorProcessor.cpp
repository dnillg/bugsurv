#include "SignDetectorProcessor.h"

// Feldolgoz� konstruktora
SignDetectorProcessor::SignDetectorProcessor(
	const QString _cascadeFile, double _scaleFactor, int _minNeighbors, SignColor _signColor,
	bool _colorFilterEnabled, int _minComponentSize, int _maxComponentDistance, const QString _annotationFile,
	const unsigned short& _frameLimit, const unsigned short& _maxFollowDistance, const unsigned short& _targetPoints)
	:CascadeDetectorProcessor(_cascadeFile, _scaleFactor, _minNeighbors), signColor(_signColor),
	cfEnabled(_colorFilterEnabled), cfInitialized(false), minComponentSize(_minComponentSize),
	maxComponentDistance(_maxComponentDistance),
	annotationFile(_annotationFile),
	frameLimit(_frameLimit),
	maxFollowDistance(_maxFollowDistance),
	targetPoints(_targetPoints),
	tracker(_targetPoints, _maxFollowDistance, _frameLimit)
{
	setName("Traffic Sign Detector");
	initHeaders();
}

// Teljes m�solatot k�sz�t (a protot�pusr�l)
ImageProcessor* SignDetectorProcessor::clone(){
	return new SignDetectorProcessor(*this);
}

// M�sol� konstruktor
SignDetectorProcessor::SignDetectorProcessor(const SignDetectorProcessor& o)
	: CascadeDetectorProcessor(o) , cFilter(o.cFilter), tracker(o.tracker)
{
	signColor = o.signColor;
	cfEnabled = o.cfEnabled;
	minComponentSize = o.minComponentSize;
	maxComponentDistance = o.maxComponentDistance;
	annotationFile = o.annotationFile;
	frameLimit = o.frameLimit;
	maxFollowDistance = o.maxFollowDistance;
	targetPoints = o.targetPoints;
	tracker = ObjectTracker(targetPoints, maxFollowDistance, frameLimit);
}

// Destruktor
SignDetectorProcessor::~SignDetectorProcessor(void)
{
}

// A detektor komponensei inicializ�latlan �llapotba ker�lnek
void SignDetectorProcessor::reinitialize(){
	CascadeDetectorProcessor::reinitialize();
	cfInitialized = false;
}

// A Bugsurv keretrendszer grafikus param�ter mez�inek hozz�ad�sa
void SignDetectorProcessor::addParams(){
	CascadeDetectorProcessor::addParams();
}

// A Bugsurv keretrendszer grafikus akci�gombok hozz�ad�sa
void SignDetectorProcessor::addActions(){
	CascadeDetectorProcessor::addActions();
}

// Komponenssz�r� inicializ�l�sa sz�n alapj�n
void SignDetectorProcessor::initComponentFilter(){
	vector<pair<Scalar, Scalar>> ranges;

	switch (signColor)
	{
	case SignDetectorProcessor::RED:
		ranges.push_back(pair<Scalar, Scalar>(Scalar(0, 105, 70), Scalar(10, 255, 255)));
		ranges.push_back(pair<Scalar, Scalar>(Scalar(170, 105, 70), Scalar(180, 255, 255)));
		ranges.push_back(pair<Scalar, Scalar>(Scalar(160, 70, 70), Scalar(170, 200, 160)));
		ranges.push_back(pair<Scalar, Scalar>(Scalar(140, 90, 22), Scalar(180, 200, 120)));
		ranges.push_back(pair<Scalar, Scalar>(Scalar(130, 30, 18), Scalar(180, 120, 120)));
		break;
	case SignDetectorProcessor::BLUE:
		ranges.push_back(pair<Scalar, Scalar>(Scalar(211/2-8, 148, 70), Scalar(211/2+11, 255, 255)));
		break;
	case SignDetectorProcessor::YELLOW:
		ranges.push_back(pair<Scalar, Scalar>(Scalar(16, 50, 70), Scalar(33, 255, 255)));
		break;
	case SignDetectorProcessor::WHITE:
		ranges.push_back(pair<Scalar, Scalar>(Scalar(0, 0, 90), Scalar(180, 70, 255)));
		
		break;
	default:
		break;
	}
	cFilter = ComponentFilter(ranges, minComponentSize);
	switch(signColor){
	case SignDetectorProcessor::WHITE:
		cFilter.addExceptionRange(Scalar(0, 40, 0), Scalar(12, 255, 255));
		cFilter.addExceptionRange(Scalar(165, 40, 0), Scalar(180, 255, 255));
		break;
	}

	cfInitialized = true;
}

// Feldolgoz�st foglalja mag�ba
void SignDetectorProcessor::process(){
	try{
		initDetector();
		initComponentFilter();

		if(ipdata.frame.type() != CV_8UC3){
			ipdata.frame.convertTo(ipdata.frame, CV_8UC3);
		}

		// < Detekt�l�s �s t�glalapok kiterjeszt�se
		cvtColor(ipdata.frame, grayscale, COLOR_BGR2GRAY);
		grayscale.resize(ipdata.frame.size().height/5*3);
		grayscale = ipdata.frame;
		vector<Rect> rects = extendRects(detect(ipdata.frame), 1.08);

		vector<ComponentFilter::Component> components;
		vector<bool> rectFilter;

		// < Ha volt tal�lat, akkor komponensek keres�se �s �sszehasonl�t�sa a tal�latokkal
		if(rects.size() && cfEnabled){
			components = cFilter.getComponents(ipdata.frame, 1);
			rectFilter = filterByComponents(rects, components);
		}
		// >

		int followId = -1;
		Scalar rectColor = Scalar(255,255,255);

		// < Iter�ci� a detekt�lt objektumokon
		for(int i = 0; i < rects.size(); i++){
			// < K�vet�si azonos�t� meghat�roz�sa (-1: m�g nem k�vetett)
			followId = -1;
			followId = tracker.hit(cvPoint(rects[i].x+rects[i].width/2, rects[i].y+rects[i].height/2), ipdata.frameCounter,(cfEnabled && rectFilter[i]?3:1));
			// >

			// < Kirajzol�s sz�n�nek meghat�roz�sa �s kirajzol�s
			if(followId >= 0)
				rectColor = Scalar(40,255,40);
			else if(cfEnabled && rectFilter[i])
				rectColor = Scalar(255,0,0);
			else
				rectColor = Scalar(0,0,255);
			rectangle(ipdata.frame, rects[i], rectColor, 2);
			// >

			// < Eredm�nyek �r�sa az annot�ci�s objektumba
			if(followId >= 0){
				//emit writeLog("Hit!");
				//qDebug("Hit!");
			
				if(ipdata.frameCounter > 0){
					annotationMutex.lock();
					//annotation.addObject(rects[i], followId, ipdata.frameCounter);
					annotationMutex.unlock();
				}
			}
			// >
		}

		// Komponensek jelz�se
		for(int i = 0; i < components.size(); i++){
			circle(ipdata.frame, cvPoint(components[i].x, components[i].y), 3, Scalar(0,255,0), 2);
		}
	} catch (cv::Exception ex){
		cout << ex.err;
	}
}

vector<bool> SignDetectorProcessor::filterByComponents(const vector<Rect>& rects, const vector<ComponentFilter::Component>& components){
	vector<bool> hits;
	for(int i = 0; i < rects.size(); i++){
		bool closeEnough = false;
		for(int j = 0; j < components.size(); j++){
			if(ComponentFilter::getDistanceFromRect(rects[i], components[j]) < maxComponentDistance){
				closeEnough = true;
				break;
			}	
		}
		hits.push_back(closeEnough);
	}
	return hits;
}

SignDetectorProcessor::SignColor SignDetectorProcessor::parseColor(string color){
	if(color == "red")
		return RED;
	else if(color == "white")
		return WHITE;
	else if(color == "yellow")
		return YELLOW;
	else if(color == "blue"){
		return BLUE;
	} else {
		return NONE;
	}
}