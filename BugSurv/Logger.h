/*
** @project			BugSurv Framework
** @file			Logger.h
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		A naplózást megvalósító osztály deklarációja.
*/

#ifndef LOGGER_H
#define LOGGER_H

// < QT
#include <QObject>
#include <QMutex>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
// >

// < DEFINE
#define LOG_SEPARATOR "#"
#define LOG_SEP "#"
#define LOG_HEAD_END " > "
#define LOG_DEFAULT_HEADER "Rendszer"
// >

using namespace std;

class Logger : public QObject
{
	Q_OBJECT
public:
	Logger(std::string _fileName);
	virtual ~Logger(void);
	static const QString extendHeader(const QString& extension, const QString& header);
protected:
	std::string fileName;
	QMutex appendMutex;
	QFile file;
	QTextStream* stream;
	Logger(const Logger& o);
signals:
	void lineWritten(QString message, QString header);
public slots:
	void messageSent(QString message, QString header);
};

#endif // LOGGER_H