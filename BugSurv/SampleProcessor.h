/*
** @project			BugSurv Framework
** @file			SampleProcessor.h
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Példa egy képfeldolgozó deklarációjára.
		Kontúrok kereséset implementálja.
*/

#ifndef SAMPLEPROCESSOR_H
#define SAMPLEPROCESSOR_H

// $ Õsosztály
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
	int thresholdVal; // : Küszöbérték az élkeresésnél
	void addParams();
	void addActions();
public slots:
	virtual void process();
protected slots:
	void set140();
};
#endif // SAMPLEPROCESSOR_H