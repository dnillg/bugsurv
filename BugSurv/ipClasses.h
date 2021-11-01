/*
** @project			BugSurv Framework
** @file			ipClasses.h
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		A feldolgozóosztályok összegyûjtve egy header fájlba, ill.
		az inicializáló függvény deklarációja.
*/

#ifndef IPCLASSES_H
#define IPCLASSES_H

// < Itt add hozzá a feldolgozók headerjeit
#include "SampleProcessor.h"
#include "HistogramEqProcessor.h"
#include "CornerDetectionProcessor.h"
#include "ObjectRecoProcessor.h"
#include "CascadeDetectorProcessor.h"
#include "DLibDetectorProcessor.h"
#include "SignDetectorProcessor.h"
// >

//A függvény definícójában hozz létre egy példányt a feldolgozóból
void initImageProcessors();

#endif //IPCLASSES_H