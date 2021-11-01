/*
** @project			BugSurv Framework
** @file			CascadeDetectorProcessor.cpp
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Példa egy képfeldolgozó definíciójára.
*/

// $ Implementált osztály
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

//Detektor inicializálása (pl. módosítás esetén)
void CascadeDetectorProcessor::initDetector(){
	if(!cascadeFile.isEmpty() && !cInitialized){
		classifier.load(cascadeFile.toStdString());
		cInitialized = true;
	}
}

// Feldolgozás implementálása.
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

// Vissza kell adni az objektum saját másolatának mutatóját.
ImageProcessor* CascadeDetectorProcessor::clone(){
	return new CascadeDetectorProcessor(*this);
}

// Másoló konstruktor.
CascadeDetectorProcessor::CascadeDetectorProcessor(const CascadeDetectorProcessor& o)
	: ImageProcessor(o){
	// ! VIGYÁZZ : az adattagoknál gondoskodj a megfelelõ adatmásolásról!
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

// Paramétereket hozza létre.
void CascadeDetectorProcessor::addParams(){
	// ! VIGYÁZZ: ne add hozzá többször ugyan azt a változót!
	addParam("Cascade file", &cascadeFile);
	addParam("Scale factor", &scaleFactor);
	addParam("Minimum neighbor", &minNeighbors);
}

// Hozzáadja az akciógombokhoz függvényeket
void CascadeDetectorProcessor::addActions(){
	addAction("Reinit", "reinitialize()");
}

void CascadeDetectorProcessor::reinitialize(){
	cInitialized = false;
}

//Detektáció
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