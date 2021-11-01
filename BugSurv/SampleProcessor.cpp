/*
** @project			BugSurv Framework
** @file			SampleProcessor.cpp
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Példa egy képfeldolgozó definíciójára.
*/

// $ Implementált osztály
#include "SampleProcessor.h"

// $ PTZcommands struktúra miatt szükséges
#include "Connection.h"


// Konstruktor.
SampleProcessor::SampleProcessor():
	ImageProcessor(),
	thresholdVal(180),
	cFilter(Scalar(211/2-8, 125, 60), Scalar(211/2+11, 255, 255), 25)
{
	initHeaders();
}

// Feldolgozás implementálása.
void SampleProcessor::process(){
	//cvtColor(ipdata.frame, ipdata.frame, CV_BGR2GRAY);
	/*cv::threshold(ipdata.frame, ipdata.frame, thresholdVal, 255, CV_THRESH_BINARY);

	std::vector<std::vector<cv::Point> > contours;
	cv::Mat contourOutput = ipdata.frame.clone();
	cv::findContours( contourOutput, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE );
	ipdata.frame = Mat(ipdata.frame.size(), CV_8UC3, cv::Scalar(0,0,0));

	for (size_t idx = 0; idx < contours.size(); idx++) {
		cv::drawContours(ipdata.frame, contours, idx, cv::Scalar(255, 255, 255));
	}*/
	//normalize(ipdata.frame, ipdata.frame, 1,0,NORM_MINMAX);
	vector<ComponentFilter::Component> components = cFilter.getComponents(ipdata.frame, 3);
	for(int i = 0; i<components.size(); i++){
		circle(ipdata.frame, cvPoint(components[i].x, components[i].y), 5, Scalar(0,255,255), 2);
	}
}

// Visszaadja a feldolgozó nevét.
string SampleProcessor::getName(){
	return "MintaFeldolgozó";
}

// Vissza kell adni az objektum saját másolatának mutatóját.
ImageProcessor* SampleProcessor::clone(){
	return new SampleProcessor(*this);
}

// Másoló konstruktor.
SampleProcessor::SampleProcessor(const SampleProcessor& o)
	:cFilter(o.cFilter){
	// ! VIGYÁZZ : az adattagoknál gondoskodj a megfelelő adatmásolásról!
	thresholdVal = o.thresholdVal;
	initHeaders();
	initParams();
	initActions();
}

// Paramétereket hozza létre.
void SampleProcessor::addParams(){
	// ! VIGYÁZZ: ne add hozzá többször ugyan azt a változót!
	addParam("Küszöbérték", &thresholdVal);
}

// Hozzáadja az akciógombokhoz a set140 függvényt.
void SampleProcessor::addActions(){
	addAction("Beállít 140", "set140");
}

// Beállítja a küszöbértéket 140-re.
void SampleProcessor::set140(){
	thresholdVal = 140;
	updateParam(&thresholdVal);
}

// Destruktor.
SampleProcessor::~SampleProcessor(void)
{
}
