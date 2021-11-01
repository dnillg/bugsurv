/*
** @project			BugSurv Framework
** @file			HistogramEqProcessor.h
** @author			Giczi D�niel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		P�lda egy k�pfeldolgoz� deklar�ci�j�ra.
		Sz�kesk�l�s k�p hisztorgramkiegyenl�t�s�t implement�lja.
*/

#ifndef HISTOGRAMEQPROCESSOR_H
#define HISTOGRAMEQPROCESSOR_H

// $ �soszt�ly
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