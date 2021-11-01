/*
** @project			BugSurv Framework
** @file			ObjectRecoProcessor.h
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Példa egy képfeldolgozó deklarációjára.
		Webes interfészû objektumfelismerõhöz kliens.
*/

#ifndef ObjectRecoProcessor_H
#define ObjectRecoProcessor_H

#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QString>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>

// $ Õsosztály
#include "ImageProcessor.h"

using namespace std;

class ObjectRecoProcessor : public ImageProcessor
{
protected:
	Q_OBJECT
public:
	ObjectRecoProcessor(std::string _address, std::string _database, int psize = 80, int px = 50, int py = 50);
	virtual std::string getName();
	virtual ImageProcessor* clone();
	virtual ~ObjectRecoProcessor(void);
	ObjectRecoProcessor(const ObjectRecoProcessor&);
protected:
	QNetworkAccessManager *manager;
	bool sendFlag;
	QString database;
	int percentSize;
	int percentYPos;
	int percentXPos;
	int respWidth;
	int respHeight;
	QString address;
	Mat result1;
	Mat result2;
	Mat result3;
	QMutex resultMutex;
	void addParams();
	void addActions();
	void sendLookup(Mat& image);
	QString matToBase64(Mat& image);
	cv::Mat base64ToMat(QString& base64);
	cv::Rect getRect(cv::Size& size);
	void drawResult(Mat& result, Mat& dst, int x);
public slots:
	virtual void process();
protected slots:
	void sendImage();
	void showLookupAnswer(QNetworkReply* );
};
#endif // ObjectRecoProcessor_H
