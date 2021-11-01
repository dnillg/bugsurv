/*
** @project			BugSurv Framework
** @file			HistogramEqProcessor.cpp
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Példa egy képfeldolgozó definíciójára.
		Szükeskálás kép hisztorgramkiegyenlítését implementálja.
*/

// $ Implementált osztály
#include "HistogramEqProcessor.h"

// $ PTZcommands struktúra miatt szükséges
#include "Connection.h"


// Konstruktor.
HistogramEqProcessor::HistogramEqProcessor():
	ImageProcessor()
{
	initHeaders();
}

// Feldolgozás implementálása.
void HistogramEqProcessor::process(){
	cvtColor(ipdata.frame, ipdata.frame, CV_BGR2GRAY);
	equalizeHist( ipdata.frame, ipdata.frame);
}

// Visszaadja a feldolgozó nevét.
string HistogramEqProcessor::getName(){
	return "Hisztogram kiegyenlítő";
}

// Vissza kell adni az objektum saját másolatának mutatóját.
ImageProcessor* HistogramEqProcessor::clone(){
	return new HistogramEqProcessor(*this);
}

// Másoló konstruktor.
HistogramEqProcessor::HistogramEqProcessor(const HistogramEqProcessor& o){
	// ! VIGYÁZZ : az adattagoknál gondoskodj a megfelelő adatmásolásról!
	initHeaders();
}

// Destruktor.
HistogramEqProcessor::~HistogramEqProcessor(void)
{
}
