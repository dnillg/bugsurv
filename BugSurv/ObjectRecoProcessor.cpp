/*
** @project			BugSurv Framework
** @file			ObjectRecoProcessor.cpp
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Példa egy képfeldolgozó definíciójára.
*/

// $ Implementált osztály
#include "ObjectRecoProcessor.h"

// $ PTZcommands struktúra miatt szükséges
#include "Connection.h"

// Konstruktor.
ObjectRecoProcessor::ObjectRecoProcessor(string _address, string _database, int psize, int px, int py):
	ImageProcessor(),
	sendFlag(false),
	manager(0),
	respWidth(200),
	respHeight(200),
	percentSize(80),
	percentYPos(py),
	percentXPos(px),
	database(QString::fromStdString(_database)),
	address(QString::fromStdString(_address))
{
	initHeaders();
}

cv::Rect ObjectRecoProcessor::getRect(cv::Size& size){
	int ps = (percentSize <= 100 && percentSize > 0)?percentSize:100;
	double rx = (percentXPos-ps/2.0)/100;
	double ry = (percentYPos-ps/2.0)/100;
	double esize = (1-(1-ps/100.0));

	if(ry < 0)
		ry = 0;
	if(ry + esize > 1)
		ry = 1-esize;

	if(rx < 0)
		rx = 0;
	if(rx + esize > 1)
		rx = 1-esize;

	return cv::Rect((int)(size.width*rx),(int)(size.height*ry),(int)(size.width*esize),(int)(size.height*esize));
}

void ObjectRecoProcessor::drawResult(Mat& result, Mat& dst, int x){
	if(result.type() != dst.type() || result.size() != cv::Size(80,80)){
			cv::resize(result, result, cv::Size(80,80));
		}
		Rect r = Rect(x,0,80,80);
		Mat dst_ = dst(r);
		result.copyTo(dst_);
}

// Feldolgozás implementálása.
void ObjectRecoProcessor::process(){
	if(sendFlag){
		sendFlag = false;
		sendLookup(ipdata.frame);
	}

	resultMutex.lock();
	if(result1.data){
		drawResult(result1, ipdata.frame, 0);
		drawResult(result2, ipdata.frame, 80);
		drawResult(result3, ipdata.frame, 160);
	}
	resultMutex.unlock();

	cv::Rect rect = getRect(ipdata.frame.size());
	cv::rectangle(ipdata.frame, rect, cv::Scalar(100,255,100));
}

// Visszaadja a feldolgozó nevét.
string ObjectRecoProcessor::getName(){
	return "Object Recognition";
}

// Vissza kell adni az objektum saját másolatának mutatóját.
ImageProcessor* ObjectRecoProcessor::clone(){
	return new ObjectRecoProcessor(*this);
}

// Másoló konstruktor.
ObjectRecoProcessor::ObjectRecoProcessor(const ObjectRecoProcessor& o){
	// ! VIGYÁZZ : az adattagoknál gondoskodj a megfelelő adatmásolásról!
	manager = 0;
	sendFlag = false;
	respHeight = o.respHeight;
	respWidth = o.respWidth;
	percentSize = o.percentSize;
	percentYPos = o.percentYPos;
	percentXPos = o.percentXPos;
	database = o.database;
	address = o.address;
	initHeaders();
	initParams();
	initActions();
}

// Paramétereket hozza létre.
void ObjectRecoProcessor::addParams(){
	// ! VIGYÁZZ: ne add hozzá többször ugyan azt a változót!
	addParam("Szolgáltatás címe", &address);
	addParam("Adatbázis", &database);
	addParam("Méret(%)", &percentSize);
	addParam("Közép X(%)", &percentXPos);
	addParam("Közép Y(%)", &percentYPos);
}

// Hozzáadja az akciógombokhoz a küldést.
void ObjectRecoProcessor::addActions(){
	addAction("Send Image", "sendImage");
}

// Kép küldéséhez a flag beállítása.
void ObjectRecoProcessor::sendImage(){
	sendFlag = true;
}

// Destruktor.
ObjectRecoProcessor::~ObjectRecoProcessor(void)
{
	if(manager)
		delete manager;
}

// Mat képet alakít át BASE64 karakterlánccá.
QString ObjectRecoProcessor::matToBase64(Mat& image){
	vector<uchar> buffer;
	vector<int> vals;
	imencode(".jpg", image, buffer, vals);
	QByteArray qba((char*)buffer.data(), buffer.size());
	qba = qba.toBase64();
	return QString::fromLocal8Bit(qba.data());
}

// Elküld egy kérést.
void ObjectRecoProcessor::sendLookup(Mat& image){
	if(!manager){
		manager = new QNetworkAccessManager();
		QObject::connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(showLookupAnswer(QNetworkReply *)));
	}

	//BASE64
	Size size = image.size();
	//double sizeMult = (1-2*percentSize/100.0);
	cv::Rect rect = getRect(ipdata.frame.size());
	Mat cropped(image, rect);
	QString str = matToBase64(cropped);

	//JSON
	QJsonObject json;
	json.insert("database", database);
	json.insert("respWidth", QString::number(respWidth));
	json.insert("respHeight", QString::number(respWidth));
	json.insert("imageData", str);
	QJsonDocument doc(json);

	//Request
	QUrl url(address + "lookup");
	QNetworkRequest request;
	request.setUrl(url);
	request.setRawHeader("Accept", "application/json");
	request.setRawHeader("Content-type", "application/json");
	manager->post(request, doc.toJson());

	//writeLog(doc.toJson());
	writeLog(address + "lookup");
}

// Kérés válaszának kezelése.
void ObjectRecoProcessor::showLookupAnswer(QNetworkReply* reply){
	QString str = reply->readAll();
	if(str.length() > 0){
		writeLog("Lookup result arrived!");
		QByteArray data = str.toLocal8Bit();
		QJsonDocument replyDoc = QJsonDocument::fromJson(data);
		QJsonObject replyObj = replyDoc.object();
		QString response = "<br>Image IDs:";
		//writeLog(str);
		writeLog(response
			+ "<br>" + replyObj.find("imagePath1").value().toString()
			+ "<br>" + replyObj.find("imagePath2").value().toString()
			+ "<br>" + replyObj.find("imagePath3").value().toString()			
			);
		resultMutex.lock();
		result1 = base64ToMat(replyObj.find("imageData1").value().toString());
		result2 = base64ToMat(replyObj.find("imageData2").value().toString());
		result3 = base64ToMat(replyObj.find("imageData3").value().toString());
		resultMutex.unlock();
	} else {
		writeLog("Web request failed!");
	}
}

// BASE64 karakterláncot alakít Mat képpé.
cv::Mat ObjectRecoProcessor::base64ToMat(QString& base64){
		QByteArray rqba = base64.toLocal8Bit();
		rqba = QByteArray::fromBase64(rqba);
		const unsigned char* begin = reinterpret_cast<unsigned char*>(rqba.data());
		const unsigned char* end = begin + rqba.length();
		vector<uchar> rbuffer(begin, end);
		return imdecode(rbuffer, 1);
}