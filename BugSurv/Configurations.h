/*
** @project			BugSurv Framework
** @file			Configurations.h
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Interfészként szolgáló osztály deklarációja a program többi része és az adatbázis között.
*/

#ifndef CONFIGURATIONS_H
#define CONFIGURATIONS_H

// $ Konfiguráció
#include "cfg.h"

// $ SQLite3
#include "sqlite3.h"

// < ACTi SDK10000
#include "qt_windows.h"
#include "SDK10000.h"
// > 

// $ C++
#include <vector>

using namespace std;

class Configurations
{
public:
	Configurations(void);
	~Configurations(void);
	// < Kapcsolatok adatait tároló struktúra (ömlesztve)
	struct ConnectionConf{
	public:
		//Eltárolt kapcsolat alapadatai
		int id; // : Adatbázisban használt kulcs
		int connectionType; // : Kapcsolat típusa
		std::string name; // : Kapcsolat neve
		std::string snapFolder; // : Pillanatképek mentési könyvtára
		std::string recFolder; // : Felvételek mentési könyvtára
		
		//ACTi SDK
		string ptzfile; // : PTZ fájl címe
		string unicastIP;
		int httpPost;
		int multicastPort;
		int registerPort;
		int rtspPort;
		int controlPort;
		int rtpMulticastVideoPort;
		int streamingPort;
		int rtpMulticastAudioPort;
		int channelNumber;
		std::string multicastIP;
		int contactType;
		std::string user;
		std::string password;


		//RTSP via OpenCV
		std::string rtspAddress; // : RTSP cím

		int usbcamIndex; // : USB kamera indexe (openCV)

	};
	// >
	const std::vector<ConnectionConf>& getConnectionConfs(bool refresh = false);
	ConnectionConf* getConnectionConfById(int id);
	bool delConnectionConfById(int id);
	bool addConnectionConf(ConnectionConf);
	static int sqlcb_connections(void *data, int argc, char **argv, char **azColName);
private:
	std::vector<ConnectionConf> connections; // : Tárolt kapcsolatok tömbje
	sqlite3* database; // : Adatbázispointer
	char* errmsg; // : Lekérdezés utáni SQL error üzene
	char* sql; // : Lekérdezéshez használt segédváltozó
};

#endif // CONFIGURATIONS_H

