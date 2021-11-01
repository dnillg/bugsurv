#pragma once
#include <opencv2\opencv.hpp>
using namespace cv;

int _connectedComponents(InputArray _img, OutputArray _labels, int connectivity = 8, int ltype = 4);
int _connectedComponentsWithStats(InputArray _img, OutputArray _labels, OutputArray statsv, OutputArray centroids, int connectivity, int ltype);