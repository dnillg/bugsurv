/*
** @project			BugSurv Framework
** @file			DLibDetectorProcessor.h
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Példa egy képfeldolgozó deklarációjára.
		Kontúrok kereséset implementálja.
*/

#ifndef DLIBDETECTORPROCESSOR_H
#define DLIBDETECTORPROCESSOR_H

// $ Õsosztály
#include "ImageProcessor.h"

#include <dlib/svm_threaded.h>
#include <dlib/string.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_processing.h>
#include <dlib/data_io.h>
#include <dlib/opencv.h>

#include <iostream>
#include <fstream>

class DLibDetectorProcessor : public ImageProcessor
{
protected:
	Q_OBJECT
public:
	DLibDetectorProcessor(const QString _cascadeFile = QString::null);
	virtual string getName();
	virtual ImageProcessor* clone();
	virtual ~DLibDetectorProcessor(void);
	DLibDetectorProcessor(const DLibDetectorProcessor&);
protected:
	QString svmFile; // : Kaszkád fájl a detektáláshoz
	bool cInitialized;
	void addParams();
	void addActions();
	std::vector<Rect> detect(Mat& image);
	Mat grayscale;
public slots:
	virtual void process();
protected slots:
	void reinitialize();
// *** DLIB
protected:
	unsigned long upsampleAmount;

	typedef dlib::scan_fhog_pyramid<dlib::pyramid_down<6> > image_scanner_type; 
	typedef dlib::hashed_feature_image<dlib::hog_image<3,3,1,4,dlib::hog_signed_gradient,dlib::hog_full_interpolation> > feature_extractor_type;

	image_scanner_type scanner;
	dlib::array2d<unsigned char> image;
    std::vector<std::vector<dlib::rectangle> > object_locations, ignore;

	dlib::object_detector<image_scanner_type> detector;


	template <typename image_array_type>
	void make_simple_test_data (
		image_array_type& images,
		std::vector<std::vector<dlib::rectangle> >& object_locations);
	void pick_best_window_size (
	    const std::vector<std::vector<dlib::rectangle> >& boxes,
		unsigned long& width,
		unsigned long& height,
		const unsigned long target_size);
	bool contains_any_boxes(const std::vector<std::vector<dlib::rectangle> >& boxes);

	
};
#endif // DLIBDETECTORPROCESSOR_H