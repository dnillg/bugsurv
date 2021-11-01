#pragma once
#include <QObject>
#include <opencv2\opencv.hpp>

using namespace cv;

class Segmenter :
	public QObject
{
	Q_OBJECT
public:
	Segmenter(void);
	virtual Mat getForeground(Mat& croppedImage) = 0;
	virtual ~Segmenter(void);
protected:
	static Mat getWatershedMask(Mat& image);
	static Mat NoiseReduction(Mat img, int block=5);

	class WatershedSegmenter{
	private:
		Mat markers;
	public:
		void setMarkers(Mat& markerImage);
		Mat process(Mat &image);
	};
};

