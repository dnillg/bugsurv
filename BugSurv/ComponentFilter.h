#pragma once
#include <opencv2\opencv.hpp>
#include "Cv3Functions.h"

using namespace std;
using namespace cv;

class ComponentFilter
{
protected:
	vector<Scalar> hsvLowerLimits;
	vector<Scalar> hsvUpperLimits;
	vector<Scalar> exHsvLowerLimits;
	vector<Scalar> exHsvUpperLimits;
	unsigned int minCompSize;
	Mat getMask(Mat image);
	static Mat histogramEqualization(Mat& image);
public:
	struct Component{
		unsigned int x;
		unsigned int y;
		unsigned int size;
	};
	ComponentFilter(Scalar _hsvLowerLimit, Scalar _hsvUpperLimit, unsigned int _minCompSize);
	ComponentFilter(vector<pair<Scalar, Scalar> > ranges, unsigned int _minCompSize);
	ComponentFilter();
	void addExceptionRange(Scalar _hsvLowerLimit, Scalar _hsvUpperLimit);
	vector<Component> getComponents(Mat& img, unsigned int div = 1);
	~ComponentFilter(void);
	static double getDistanceFromRect(Rect rect, Point point);
	static double getDistanceFromRect(Rect rect, Component component);
};

