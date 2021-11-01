#include "ComponentFilter.h"

// Maszk készítése színtartományok alapján
Mat ComponentFilter::getMask(Mat img){
	Mat mask, imgHSV, subMask, negMask;
	cvtColor(img, imgHSV, COLOR_BGR2HSV);

	// < Tartományok összeadása
	unsigned int boundsSize = hsvLowerLimits.size();
	vector<Mat> subMasks(boundsSize);
	for(int i = 0; i < boundsSize; i++){
		Mat subMask;
		inRange(imgHSV, hsvLowerLimits[i], hsvUpperLimits[i], subMask);
		subMasks[i] = subMask;
	}
	mask = subMasks[0];
	for(int i = 1; i < boundsSize; i++){
		bitwise_or(mask, subMasks[i], mask);
	}
	// >

	// < Kivételek kivonása
	int exSize = exHsvLowerLimits.size();
	for(int i = 0; i < exSize; i++){
		inRange(imgHSV, exHsvLowerLimits[i], exHsvUpperLimits[i], subMask);
		bitwise_not(subMask, negMask);
		bitwise_and(mask, negMask, mask);
	}
	// >
	return mask;
}

ComponentFilter::ComponentFilter(){}

ComponentFilter::ComponentFilter(Scalar _hsvLowerLimit, Scalar _hsvUpperLimit, unsigned int _minCompSize)
	:minCompSize(_minCompSize)
{
	hsvUpperLimits.push_back(_hsvUpperLimit);
	hsvLowerLimits.push_back(_hsvLowerLimit);
}

ComponentFilter::ComponentFilter(vector<pair<Scalar, Scalar> > ranges, unsigned int _minCompSize)
	:minCompSize(_minCompSize)
{
	unsigned int size = ranges.size();
	for(unsigned int i = 0; i < size; i++){
		hsvUpperLimits.push_back(ranges[i].second);
		hsvLowerLimits.push_back(ranges[i].first);
	}
}

// Kivétel hozzáadása
void ComponentFilter::addExceptionRange(Scalar _hsvLowerLimit, Scalar _hsvUpperLimit){
	exHsvUpperLimits.push_back(_hsvUpperLimit);
	exHsvLowerLimits.push_back(_hsvLowerLimit);
}

// Hisztogramszéthúzás
Mat ComponentFilter::histogramEqualization(Mat& image){
	vector<Mat> channels; 
	Mat eqImage;
	cvtColor(image, eqImage, CV_BGR2YCrCb); //change the color image from BGR to YCrCb format
	split(eqImage,channels); //split the image into channels
	equalizeHist(channels[0], channels[0]); //equalize histogram on the 1st channel (Y)
	merge(channels,eqImage); //merge 3 channels including the modified 1st channel into one image
	cvtColor(eqImage, eqImage, CV_YCrCb2BGR); //change the color image from YCrCb to BGR format (to display image properly)
	return eqImage;
}

// Komponensek keresése és szûrése
vector<ComponentFilter::Component> ComponentFilter::getComponents(Mat& image, unsigned int div){
	vector<Component> components;
	if(hsvLowerLimits.size() == 0)
		return components;

	Mat img;
	Mat imgHSV;
	Mat mask;
	if(div != 1)
		resize(image, img, Size(image.cols/div,image.rows/div));
	else
		img = image;

	Mat labeled(img.size(), CV_32S);

	Mat eqImage = histogramEqualization(img);
	
	mask = getMask(eqImage);

	// < Komponensek keresése
	int nLabels = _connectedComponents(mask, labeled);
	nLabels--;
	// >

	// < Komponensekhez tartozó struktúrák elkészítése
	std::vector<Component> allComponents(nLabels);
	Component nullComp = {0, 0, 0};
    for(int i = 0; i < nLabels; ++i){
		allComponents.push_back(nullComp);
    }
	// >

	// < Komponensek jellemzõinek kiszámítása
    Mat dst(img.size(), CV_8UC3);
    for(int r = 0; r < img.rows; ++r){
        for(int c = 0; c < img.cols; ++c){
            int label = labeled.at<int>(r, c);
			if(label != 0){
				label--;
				allComponents[label].x+=c;
				allComponents[label].y+=r;
				allComponents[label].size++;
			}
         }
     }
	// >

	// < Komponensek szûrése méret alapján
	for(int label = 0; label < nLabels; ++label){
		if(allComponents[label].size >= minCompSize){
			Component cc;
			cc.x=allComponents[label].x/allComponents[label].size*div;
			cc.y=allComponents[label].y/allComponents[label].size*div;
			cc.size = allComponents[label].size;
			components.push_back(cc);
		}
	}
	// >

#if 0
	for(int i = 0; i<components.size(); i++){
		circle(img, cvPoint(components[i].x/div, components[i].y/div), 2, Scalar(0,255,0));
	}

	imshow("maskY", mask);
	waitKey(1);
#endif
	return components;
}


ComponentFilter::~ComponentFilter(void)
{
}

// Visszaadja egy téglalap közepéhez vett távolságot
double ComponentFilter::getDistanceFromRect(Rect rect, Point point){
	Point rectCenter = Point(rect.x+rect.width/2, rect.y+rect.height/2);
	Point axisDistance = Point(rectCenter.x-point.x, rectCenter.y-point.y);
	return sqrt(axisDistance.x*axisDistance.x+axisDistance.y*axisDistance.y);
}

// Visszaadja egy téglalap közepéhez vett távolságot
double ComponentFilter::getDistanceFromRect(Rect rect, Component component){
	return getDistanceFromRect(rect, Point(component.x, component.y));
}