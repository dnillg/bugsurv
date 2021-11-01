#include "Segmenter.h"


Segmenter::Segmenter(void)
{
}


Segmenter::~Segmenter(void)
{
}

void Segmenter::WatershedSegmenter::setMarkers(Mat& markerImage)
{
	markerImage.convertTo(markers, CV_32S);
}

Mat Segmenter::WatershedSegmenter::process(Mat &image)
{
	cv::watershed(image, markers);
	markers.convertTo(markers,CV_8U);
	return markers;
}

Mat Segmenter::NoiseReduction(Mat img, int block) //block=5 is our threshold for blob size. Less than that is noise
{
	Mat noise_reduced(img.rows,img.cols,CV_8UC1); //stores final corrected image
	int i,j,m,n,r = img.rows, c = img.cols, white, black;
 
	for(i=0;i<r-block;i+=block){
		for(j=0;j<c-block;j+=block){
 
			white = black = 0;
			//to check for large enough blob.
			for(m=i;m<i+block;m++){
				for(n=j;n<j+block;n++){
 
					if(img.at<uchar>(m,n)==0)
						black++;
					else
						white++;
				}
			}
			//to remove the identified noise from the image
			int val;
			if(black>=white) 
				val = 0;
			else
				val = 255;
 
			for(m=i;m<i+block;m++){
				for(n=j;n<j+block;n++){
					noise_reduced.at<uchar>(m,n) = val;
				}
			}
		}
	}
 
	return noise_reduced;
}

Mat Segmenter::getWatershedMask(Mat& image){
	Mat mask;
    // Create markers image
    cv::Mat markers(image.size(),CV_8U,cv::Scalar(-1));
    //top rectangle
    markers(Rect(0,0,image.cols, 5)) = Scalar::all(1);
    //bottom rectangle
    markers(Rect(0,image.rows-5,image.cols, 5)) = Scalar::all(1);
    //left rectangle
    markers(Rect(0,0,5,image.rows)) = Scalar::all(1);
    //right rectangle
    markers(Rect(image.cols-5,0,5,image.rows)) = Scalar::all(1);
    //centre rectangle
    int centreW = image.cols/4;
    int centreH = image.rows/4;
    markers(Rect((image.cols/2)-(centreW/2),(image.rows/2)-(centreH/2), centreW, centreH)) = Scalar::all(2);
    markers.convertTo(markers,CV_BGR2GRAY);
    //imshow("markers", markers);

    //Create watershed segmentation object
    WatershedSegmenter segmenter;
    segmenter.setMarkers(markers);
    cv::Mat wshedMask = segmenter.process(image);
    convertScaleAbs(wshedMask, mask, 1, 0);
    double thresh = threshold(mask, mask, 1, 255, THRESH_BINARY);
	return mask;
}