/*
** @project			BugSurv Framework
** @file			SampleProcessor.h
** @author			Giczi D�niel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		P�lda egy k�pfeldolgoz� deklar�ci�j�ra.
		Kont�rok keres�set implement�lja.
*/

#ifndef SAMPLEPROCESSOR_H
#define SAMPLEPROCESSOR_H

// $ �soszt�ly
#include "ImageProcessor.h"

#include "ComponentFilter.h"

using namespace std;

class SampleProcessor : public ImageProcessor
{
protected:
	Q_OBJECT
public:
	SampleProcessor();
	virtual std::string getName();
	virtual ImageProcessor* clone();
	virtual ~SampleProcessor(void);
	SampleProcessor(const SampleProcessor&);
	ComponentFilter cFilter;
protected:
	int thresholdVal; // : K�sz�b�rt�k az �lkeres�sn�l
	void addParams();
	void addActions();
public slots:
	virtual void process();
protected slots:
	void set140();
};
#endif // SAMPLEPROCESSOR_H