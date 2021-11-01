#include "PedestrianSegmenter.h"


PedestrianSegmenter::PedestrianSegmenter(void)
{
}


PedestrianSegmenter::~PedestrianSegmenter(void)
{
}

Mat PedestrianSegmenter::getForeground(Mat& image){
	Mat mask = getWatershedMask(image);
	Mat dest, imgHSV, maskCol;

	cvtColor(image, imgHSV, COLOR_BGR2HSV);

	//kék
	inRange(imgHSV, Scalar(95, 120, 70), Scalar(115, 255, 255), maskCol);

	//Generate Result
	bitwise_or(mask, maskCol, mask);
    bitwise_and(image, image, dest, mask);
	dest.convertTo(dest,CV_8UC3);

	#if 0
	imshow("img", image);
	imshow("mask", dest);
	//imshow("shed", shed);
	waitKey();
	#endif
	return dest;
}
