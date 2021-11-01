/*
** @project			BugSurv Framework
** @file			HistogramEqProcessor.h
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Példa egy képfeldolgozó deklarációjára.
		Szükeskálás kép hisztorgramkiegyenlítését implementálja.
*/

#ifndef HISTOGRAMEQPROCESSOR_H
#define HISTOGRAMEQPROCESSOR_H

// $ Õsosztály
#include "ImageProcessor.h"

using namespace std;

class HistogramEqProcessor : public ImageProcessor
{
protected:
	Q_OBJECT
public:
	HistogramEqProcessor();
	virtual std::string getName();
	virtual ImageProcessor* clone();
	virtual ~HistogramEqProcessor(void);
	HistogramEqProcessor(const HistogramEqProcessor&);
protected:
public slots:
	virtual void process();
protected slots:
};
#endif // HISTOGRAMEQPROCESSOR_H