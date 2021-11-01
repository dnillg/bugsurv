#pragma once
#include "imageprocessor.h"
class MyProcessor :
	public ImageProcessor
{
protected:
	Q_OBJECT
public:
	MyProcessor(void);
	virtual ~MyProcessor(void);
	MyProcessor(const MyProcessor&);

	virtual string getName() = 0;
	virtual ImageProcessor* clone() = 0;

public slots:
	virtual void process() = 0;
};

