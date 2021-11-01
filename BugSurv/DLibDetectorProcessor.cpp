/*
** @project			BugSurv Framework
** @file			DLibDetectorProcessor.cpp
** @author			Giczi D�niel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
P�lda egy k�pfeldolgoz� defin�ci�j�ra.
*/

// $ Implement�lt oszt�ly
#include "DLibDetectorProcessor.h"

// $ PTZcommands strukt�ra miatt sz�ks�ges
#include "Connection.h"


// Konstruktor.
DLibDetectorProcessor::DLibDetectorProcessor(const QString _svmFile):
	ImageProcessor(),
	svmFile(_svmFile),
	cInitialized(false),
	upsampleAmount(0)
{
	initHeaders();
}

// Feldolgoz�s implement�l�sa.
void DLibDetectorProcessor::process(){
	//Detektor inicializ�l�sa (ak�r m�dos�t�s eset�n)
	if(!svmFile.isEmpty() && !cInitialized){
		ifstream fin(svmFile.toStdString(), ios::binary);
		deserialize(detector, fin);
		fin.close();
		cInitialized = true;
	}

	cvtColor(ipdata.frame,grayscale,CV_BGR2GRAY);
	std::vector<Rect>rects=detect(grayscale);
	for(int i=0; i<rects.size(); i++){
		cv::rectangle(ipdata.frame, rects[i], cvScalar(100,100,255));
	}
}

// Visszaadja a feldolgoz� nev�t.
string DLibDetectorProcessor::getName(){
	return "DLIB Detector";
}

// Vissza kell adni az objektum saj�t m�solat�nak mutat�j�t.
ImageProcessor* DLibDetectorProcessor::clone(){
	return new DLibDetectorProcessor(*this);
}

// M�sol� konstruktor.
DLibDetectorProcessor::DLibDetectorProcessor(const DLibDetectorProcessor& o){
	// ! VIGY�ZZ : az adattagokn�l gondoskodj a megfelel� adatm�sol�sr�l!
	svmFile = o.svmFile;
	cInitialized = o.cInitialized;
	grayscale = o.grayscale;
	upsampleAmount = o.upsampleAmount;
	initHeaders();
	initParams();
	initActions();
}

// Param�tereket hozza l�tre.
void DLibDetectorProcessor::addParams(){
	// ! VIGY�ZZ: ne add hozz� t�bbsz�r ugyan azt a v�ltoz�t!
	addParam("SVM", &svmFile);
}

// Hozz�adja az akci�gombokhoz f�ggv�nyeket
void DLibDetectorProcessor::addActions(){
	addAction("Reinit", "reinitialize()");
}

void DLibDetectorProcessor::reinitialize(){
	cInitialized = false;
}

//Detekt�ci�
std::vector<Rect> DLibDetectorProcessor::detect(Mat& image)
{
	//Eredm�nystrukt�r�k
	
	//DLIB k�p a CV k�pb�l
	dlib::cv_image<uchar> dcvImg(image);

	//Detekt�l�s v�grehajt�sa
	const vector<dlib::rectangle> rects_d = detector(dcvImg);

	//Eredm�ny m�sol�sa CV strukt�r�ba
	vector<Rect> rects;
	for(int i = 0; i < rects_d.size(); i++){
		//rects.push_back(Rect(rects_d[i].left(), rects_d[i].top(), rects_d[i].width(), rects_d[i].height()));
		rects.push_back(Rect(10, 10, 10, 10));
	}

	return rects;
}

// Destruktor.
DLibDetectorProcessor::~DLibDetectorProcessor(void)
{}

// *** DLIB

void DLibDetectorProcessor::pick_best_window_size (
	const std::vector<std::vector<dlib::rectangle> >& boxes,
	unsigned long& width,
	unsigned long& height,
	const unsigned long target_size
	)
	/*!
	ensures
	- Finds the average aspect ratio of the elements of boxes and outputs a width
	and height such that the aspect ratio is equal to the average and also the
	area is equal to target_size.  That is, the following will be approximately true:
	- #width*#height == target_size
	- #width/#height == the average aspect ratio of the elements of boxes.
	!*/
{
	// find the average width and height
	dlib::running_stats<double> avg_width, avg_height;
	for (unsigned long i = 0; i < boxes.size(); ++i)
	{
		for (unsigned long j = 0; j < boxes[i].size(); ++j)
		{
			avg_width.add(boxes[i][j].width());
			avg_height.add(boxes[i][j].height());
		}
	}

	// now adjust the box size so that it is about target_pixels pixels in size
	double size = avg_width.mean()*avg_height.mean();
	double scale = std::sqrt(target_size/size);

	width = (unsigned long)(avg_width.mean()*scale+0.5);
	height = (unsigned long)(avg_height.mean()*scale+0.5);
	// make sure the width and height never round to zero.
	if (width == 0)
		width = 1;
	if (height == 0)
		height = 1;
}

bool DLibDetectorProcessor::contains_any_boxes(const std::vector<std::vector<dlib::rectangle> >& boxes)
{
	for (unsigned long i = 0; i < boxes.size(); ++i)
	{
		if (boxes[i].size() != 0)
			return true;
	}
	return false;
}

