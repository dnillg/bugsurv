/*
** @project			BugSurv Framework
** @file			CornerDetectionProcessor.cpp
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Példa egy képfeldolgozó definíciójára.
		Sarokdetektációst algoritmust valósít meg küszöbértékkel.
*/

/*------------------------------------------------------------------------------------------*\
   This file contains material supporting chapter 5 of the cookbook:  
   Computer Vision Programming using the OpenCV Library. 
   by Robert Laganiere, Packt Publishing, 2011.

   This program is free software; permission is hereby granted to use, copy, modify, 
   and distribute this source code, or portions thereof, for any purpose, without fee, 
   subject to the restriction that the copyright notice may not be removed 
   or altered from any source or altered source distribution. 
   The software is released on an as-is basis and without any warranties of any kind. 
   In particular, the software is not guaranteed to be fault-tolerant or free from failure. 
   The author disclaims all warranties with regard to this software, any use, 
   and any consequent failure, is purely the responsibility of the user.
 
   Copyright (C) 2010-2011 Robert Laganiere, www.laganiere.name
\*------------------------------------------------------------------------------------------*/

// $ Implementált osztály
#include "CornerDetectionProcessor.h"

// $ PTZcommands struktúra miatt szükséges
#include "Connection.h"

// Konstruktor.
CornerDetectionProcessor::CornerDetectionProcessor():
	ImageProcessor(),threshold(40)
{
	initHeaders();
}

// Visszaadja a feldolgozó nevét.
string CornerDetectionProcessor::getName(){
	return "Sarokdetekció";
}

// Vissza kell adni az objektum saját másolatának mutatóját.
ImageProcessor* CornerDetectionProcessor::clone(){
	return new CornerDetectionProcessor(*this);
}

// Másoló konstruktor.
CornerDetectionProcessor::CornerDetectionProcessor(const CornerDetectionProcessor& o){
	// ! VIGYÁZZ : az adattagoknál gondoskodj a megfelelő adatmásolásról!
	threshold = o.threshold;
	initHeaders();
	initParams();
}

// Paramétereket hozza létre.
void CornerDetectionProcessor::addParams(){
	// ! VIGYÁZZ: ne add hozzá többször ugyan azt a változót!
	addParam("Küszöbérték", &threshold);
}

// Feldolgozás implementálása.
void CornerDetectionProcessor::process(){
	cvtColor(ipdata.frame, ipdata.frame, CV_BGR2GRAY);
	equalizeHist( ipdata.frame, ipdata.frame );
	Mat image = ipdata.frame;

	MorphoFeatures morpho;
	morpho.setThreshold(threshold);

	// Get the edges
	cv::Mat edges;
	edges= morpho.getEdges(image);

	cv::Mat corners;
	corners= morpho.getCorners(image);

	cvtColor(image, image, COLOR_GRAY2BGR);
	// Display the corner on the image
	morpho.drawOnImage(corners,image);
	ipdata.frame = image;
}

void CornerDetectionProcessor::applyThreshold(cv::Mat& result) {
		// Apply threshold on result
		if (threshold>0)
		cv::threshold(result, result,
		threshold, 255, cv::THRESH_BINARY);
	}

	cv::Mat CornerDetectionProcessor::getEdges(const cv::Mat &image) {
		// Get the gradient image
		cv::Mat result;
		cv::morphologyEx(image,result,
		cv::MORPH_GRADIENT,cv::Mat());
		// Apply threshold to obtain a binary image
		applyThreshold(result);
		return result;
	}

	cv::Mat CornerDetectionProcessor::getCorners(const cv::Mat &image) {
		cv::Mat result;
		// Dilate with a cross
		cv::dilate(image,result,cross);
		// Erode with a diamond
		cv::erode(result,result,diamond);
		cv::Mat result2;
		// Dilate with a X
		cv::dilate(image,result2,x);
		// Erode with a square
		cv::erode(result2,result2,square);
		// Corners are obtained by differencing
		// the two closed images
		cv::absdiff(result2,result,result);
		// Apply threshold to obtain a binary image
		applyThreshold(result);
		return result;
	}

	void CornerDetectionProcessor::drawOnImage(const cv::Mat& binary,
		cv::Mat& image) {
		cv::Mat_<uchar>::const_iterator it=
		binary.begin<uchar>();
		cv::Mat_<uchar>::const_iterator itend=binary.end<uchar>();
		// for each pixel
		for (int i=0; it!= itend; ++it,++i) {
			if (!*it)
				cv::circle(image, cv::Point(i%image.step,i/image.step),	5,cv::Scalar(255,0,0));
		}
	}

// Destruktor.
CornerDetectionProcessor::~CornerDetectionProcessor(void)
{
}
