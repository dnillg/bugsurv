/*
** @project			BugSurv Framework
** @file			ipClasses.h
** @author			Giczi D�niel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		A feldolgoz�oszt�lyok �sszegy�jtve egy header f�jlba, ill.
		az inicializ�l� f�ggv�ny deklar�ci�ja.
*/

#ifndef IPCLASSES_H
#define IPCLASSES_H

// < Itt add hozz� a feldolgoz�k headerjeit
#include "SampleProcessor.h"
#include "HistogramEqProcessor.h"
#include "CornerDetectionProcessor.h"
#include "ObjectRecoProcessor.h"
#include "CascadeDetectorProcessor.h"
#include "DLibDetectorProcessor.h"
#include "SignDetectorProcessor.h"
// >

//A f�ggv�ny defin�c�j�ban hozz l�tre egy p�ld�nyt a feldolgoz�b�l
void initImageProcessors();

#endif //IPCLASSES_H