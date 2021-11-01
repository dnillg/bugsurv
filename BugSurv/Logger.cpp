/*
** @project			BugSurv Framework
** @file			Logger.cpp
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		A naplózást megvalósító osztály deklarációja.
*/

#include "Logger.h"

Logger::Logger(string _fileName):file(QString::fromStdString(_fileName)),stream(0){
		fileName = _fileName;
		file.open(QIODevice::Append | QIODevice::Text);
		stream = new QTextStream(&file);
}

void Logger::messageSent(QString message, QString header){
	QString line = header + LOG_HEAD_END + message;
	appendMutex.lock();
	*stream << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ") << line << endl;
	appendMutex.unlock();
	emit lineWritten(message, header);
}

Logger::Logger(const Logger& o){}

const QString Logger::extendHeader(const QString& extension, const QString& header){
	if(header == "")
		return extension;

	return extension + LOG_SEP + header;
}

Logger::~Logger(void)
{
	if(stream){
		delete stream;
		stream = 0;
	}
	if(file.isOpen()){
		file.close();
	}
}
