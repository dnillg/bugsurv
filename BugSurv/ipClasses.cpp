/*
** @project			BugSurv Framework
** @file			ipClasses.cpp
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Az inicializáló függvény definíciója.
*/

#include "ipClasses.h"
#include "Connection.h"

//A kapcsolatok az itt hozzáadott(példányosított) feldolgozókkal lesznek inicializálva.
void initImageProcessors(){
	/* Példa:
	** 1. Connection::addIpClass<SampleProcessor>(0);
	** 2. SampleProcessor* = new SampleProcessor(param1, param2);
	**    Connection::addIpClass(object,0);
	** Megjegyzés:
	** A második esetben tudsz a feldolgozónak konstruktor paramétert átadni.
	*/

	/*Connection::addIpClass<SampleProcessor>(0);
	Connection::addIpClass<HistogramEqProcessor>(0);
	Connection::addIpClass<CornerDetectionProcessor>(0);

	Connection::addIpClass<SampleProcessor>(0);
	Connection::addIpClass<HistogramEqProcessor>(0);
	Connection::addIpClass<CornerDetectionProcessor>(0);

	Connection::addIpClass<SampleProcessor>(0);
	Connection::addIpClass<HistogramEqProcessor>(0);
	Connection::addIpClass<CornerDetectionProcessor>(0);

	Connection::addIpClass<SampleProcessor>(0);
	Connection::addIpClass<HistogramEqProcessor>(0);
	Connection::addIpClass<CornerDetectionProcessor>(0);

	Connection::addIpClass<SampleProcessor>(0);
	Connection::addIpClass<HistogramEqProcessor>(0);
	Connection::addIpClass<CornerDetectionProcessor>(0);
	*/
	/*Connection::addIpClass<SampleProcessor>(0);
	Connection::addIpClass<HistogramEqProcessor>(0);
	Connection::addIpClass<CornerDetectionProcessor>(0);*/

	CascadeDetectorProcessor* pedDetector = new CascadeDetectorProcessor("pedestrian.xml", 1.04, 1);
	Connection::addIpClass(pedDetector, 0);
	/*CascadeDetectorProcessor* pedDetector2 = new CascadeDetectorProcessor("yield.xml", 1.04, 1);
	Connection::addIpClass(pedDetector2, 0);
	CascadeDetectorProcessor* pedDetector3 = new CascadeDetectorProcessor("prior.xml", 1.04, 1);
	Connection::addIpClass(pedDetector3, 0);
	CascadeDetectorProcessor* stopDetector = new CascadeDetectorProcessor("stop.xml", 1.04, 1);
	Connection::addIpClass(stopDetector, 0);*/

	SignDetectorProcessor* pedSignDetector = new SignDetectorProcessor("pedestrian.xml", 1.09, 1, SignDetectorProcessor::BLUE, true, 25);
	pedSignDetector->setName("Pedestrian Sign Detector");
	Connection::addIpClass(pedSignDetector, 0);

	SignDetectorProcessor* yieldSignDetector = new SignDetectorProcessor("yield.xml", 1.04, 1, SignDetectorProcessor::WHITE, true, 50, 30);
	yieldSignDetector->setName("Yield Sign Detector");
	Connection::addIpClass(yieldSignDetector, 0);

	SignDetectorProcessor* priorSignDetector = new SignDetectorProcessor("prior.xml", 1.065, 1, SignDetectorProcessor::YELLOW, true, 25);
	priorSignDetector->setName("Priority Road Sign Detector");
	Connection::addIpClass(priorSignDetector, 0);

	SignDetectorProcessor* stopSignDetector = new SignDetectorProcessor("stop.xml", 1.065, 1, SignDetectorProcessor::RED, true, 25, 50 );
	stopSignDetector->setName("Stop Sign Detector");
	Connection::addIpClass(stopSignDetector, 0);

	SignDetectorProcessor* pedSignDetector2 = new SignDetectorProcessor("pedestrian.xml", 1.09, 1, SignDetectorProcessor::BLUE, true, 25);
	pedSignDetector2->setName("Pedestrian Sign Detector2");
	Connection::addIpClass(pedSignDetector2, 0);

	SignDetectorProcessor* yieldSignDetector2 = new SignDetectorProcessor("ger_yield.xml", 1.3, 3, SignDetectorProcessor::WHITE, true, 70, 20);
	yieldSignDetector2->setName("Yield Sign Detector2");
	Connection::addIpClass(yieldSignDetector2, 0);

	SignDetectorProcessor* priorSignDetector2 = new SignDetectorProcessor("prior.xml", 1.3, 3, SignDetectorProcessor::YELLOW, true, 25);
	priorSignDetector2->setName("Priority Road Sign Detector2");
	Connection::addIpClass(priorSignDetector2, 0);

	SignDetectorProcessor* stopSignDetector2 = new SignDetectorProcessor("ger_stop.xml", 1.3, 3, SignDetectorProcessor::RED, true, 25, 50 );
	stopSignDetector2->setName("Stop Sign Detector2");
	Connection::addIpClass(stopSignDetector2, 0);

	/*DLibDetectorProcessor* dlStopDetector = new DLibDetectorProcessor("stop.svm");
	Connection::addIpClass(dlStopDetector, 0);*/
}