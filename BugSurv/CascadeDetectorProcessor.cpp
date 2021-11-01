/*
** @project			BugSurv Framework
** @file			CascadeDetectorProcessor.cpp
** @author			Giczi D�niel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		P�lda egy k�pfeldolgoz� defin�ci�j�ra.
*/

// $ Implement�lt oszt�ly
#include "CascadeDetectorProcessor.h"

// Konstruktor.
CascadeDetectorProcessor::CascadeDetectorProcessor(const QString _cascadeFile, double _scaleFactor, int _minNeighbors):
	ImageProcessor("Cascade Detector"),
	scaleFactor(_scaleFactor),
	minNeighbors(_minNeighbors),
	cascadeFile(_cascadeFile),
	cInitialized(false),
	segmenter(new PedestrianSegmenter())
{
	initHeaders();
}

//Detektor inicializ�l�sa (pl. m�dos�t�s eset�n)
void CascadeDetectorProcessor::initDetector(){
	if(!cascadeFile.isEmpty() && !cInitialized){
		classifier.load(cascadeFile.toStdString());
		cInitialized = true;
	}
}

// Feldolgoz�s implement�l�sa.
void CascadeDetectorProcessor::process(){
	initDetector();
	cvtColor(ipdata.frame,grayscale,CV_BGR2GRAY);
	vector<Rect>rects=detect(grayscale);
	vector<Rect>largerRects = extendRects(rects, 1.33);
	drawRects(ipdata.frame, largerRects, Scalar(0,0,255));
	drawSegments(ipdata.frame, largerRects);
}

void CascadeDetectorProcessor::drawRects(Mat& image, const vector<Rect>& rects, Scalar color){
	int size = rects.size();
	for(int i = 0; i < size; i++){
		rectangle(ipdata.frame, rects[i], color, 2);
	}
}

void CascadeDetectorProcessor::drawSegments(Mat& image, vector<Rect> rects){
	if(rects.size() > 0){
		Mat segment = segmenter->getForeground(Mat(image, rects[0]));
		segment.copyTo(image(Rect(0, 0, segment.size().width, segment.size().height)));
	}
}

// Vissza kell adni az objektum saj�t m�solat�nak mutat�j�t.
ImageProcessor* CascadeDetectorProcessor::clone(){
	return new CascadeDetectorProcessor(*this);
}

// M�sol� konstruktor.
CascadeDetectorProcessor::CascadeDetectorProcessor(const CascadeDetectorProcessor& o)
	: ImageProcessor(o){
	// ! VIGY�ZZ : az adattagokn�l gondoskodj a megfelel� adatm�sol�sr�l!
	cascadeFile = o.cascadeFile;
	scaleFactor= o.scaleFactor;
	minNeighbors = o.minNeighbors;
	cInitialized = o.cInitialized;
	grayscale = o.grayscale;
	segmenter = new PedestrianSegmenter();
	initHeaders();
	initParams();
	initActions();
}

// Param�tereket hozza l�tre.
void CascadeDetectorProcessor::addParams(){
	// ! VIGY�ZZ: ne add hozz� t�bbsz�r ugyan azt a v�ltoz�t!
	addParam("Cascade file", &cascadeFile);
	addParam("Scale factor", &scaleFactor);
	addParam("Minimum neighbor", &minNeighbors);
}

// Hozz�adja az akci�gombokhoz f�ggv�nyeket
void CascadeDetectorProcessor::addActions(){
	addAction("Reinit", "reinitialize()");
}

void CascadeDetectorProcessor::reinitialize(){
	cInitialized = false;
}

//Detekt�ci�
vector<Rect> CascadeDetectorProcessor::detect(const Mat& image)
{
	//Results
	vector<int> rejectLevels;
	vector<double> weightLevels;
	vector<Rect> rects;
        
	classifier.detectMultiScale(image, rects, rejectLevels, weightLevels, scaleFactor, minNeighbors, 0, Size(18,18), Size(200, 200)); //HAAR
    return rects;
}

// Destruktor.
CascadeDetectorProcessor::~CascadeDetectorProcessor(void)
{
	if(segmenter)
		segmenter->deleteLater();
}

vector<Rect> CascadeDetectorProcessor::extendRects(const vector<Rect>& rects, const double factor){
	vector<Rect> largerRects;
	int newWidth;
	int newHeight;
	for(int i=0; i<rects.size(); i++){
		newWidth = rects[i].width*factor;
		newHeight = rects[i].height*factor;
		largerRects.push_back(Rect(rects[i].x-(newWidth-rects[i].width)/2, rects[i].y-(newHeight-rects[i].height)/2, newWidth, newHeight));
	}
	return largerRects;
}