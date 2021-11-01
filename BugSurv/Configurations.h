/*
** @project			BugSurv Framework
** @file			Configurations.h
** @author			Giczi D�niel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Interf�szk�nt szolg�l� oszt�ly deklar�ci�ja a program t�bbi r�sze �s az adatb�zis k�z�tt.
*/

#ifndef CONFIGURATIONS_H
#define CONFIGURATIONS_H

// $ Konfigur�ci�
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
	// < Kapcsolatok adatait t�rol� strukt�ra (�mlesztve)
	struct ConnectionConf{
	public:
		//Elt�rolt kapcsolat alapadatai
		int id; // : Adatb�zisban haszn�lt kulcs
		int connectionType; // : Kapcsolat t�pusa
		std::string name; // : Kapcsolat neve
		std::string snapFolder; // : Pillanatk�pek ment�si k�nyvt�ra
		std::string recFolder; // : Felv�telek ment�si k�nyvt�ra
		
		//ACTi SDK
		string ptzfile; // : PTZ f�jl c�me
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
		std::string rtspAddress; // : RTSP c�m

		int usbcamIndex; // : USB kamera indexe (openCV)

	};
	// >
	const std::vector<ConnectionConf>& getConnectionConfs(bool refresh = false);
	ConnectionConf* getConnectionConfById(int id);
	bool delConnectionConfById(int id);
	bool addConnectionConf(ConnectionConf);
	static int sqlcb_connections(void *data, int argc, char **argv, char **azColName);
private:
	std::vector<ConnectionConf> connections; // : T�rolt kapcsolatok t�mbje
	sqlite3* database; // : Adatb�zispointer
	char* errmsg; // : Lek�rdez�s ut�ni SQL error �zene
	char* sql; // : Lek�rdez�shez haszn�lt seg�dv�ltoz�
};

#endif // CONFIGURATIONS_H

