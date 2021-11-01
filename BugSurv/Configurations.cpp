/*
** @project			BugSurv Framework
** @file			Configurations.cpp
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Interfészként szolgáló osztály definíciója a program többi része és az adatbázis között.
*/

#include "Configurations.h"
#include "bugsurv.h"

// Connections tábla lekérdezés eredményének feldolgozása rekordonként (CALLBACK).
int Configurations::sqlcb_connections(void* data, int argc, char** argv, char** azColName)
{
	std::vector<ConnectionConf>* configs = (std::vector<ConnectionConf>*)data;
	ConnectionConf tmp;
	tmp.id = atoi(argv[0]);
	tmp.name = argv[1];
	tmp.httpPost = atoi(argv[2]);
	tmp.registerPort = atoi(argv[3]);
	tmp.controlPort = atoi(argv[4]);
	tmp.streamingPort = atoi(argv[5]);
	tmp.multicastPort = atoi(argv[6]);
	tmp.rtspPort = atoi(argv[7]);
	tmp.rtpMulticastVideoPort = atoi(argv[8]);
	tmp.rtpMulticastAudioPort = atoi(argv[9]);
	tmp.user = argv[10];
	tmp.password = argv[11];
	tmp.unicastIP = argv[12];
	tmp.multicastIP = argv[13];
	tmp.channelNumber = atoi(argv[14]);
	tmp.contactType = atoi(argv[15]);
	tmp.ptzfile = argv[16];
	tmp.connectionType = atoi(argv[17]);
	tmp.rtspAddress = argv[18];
	tmp.usbcamIndex = atoi(argv[19]);
	tmp.snapFolder = argv[20];
	tmp.recFolder = argv[21];
	configs->push_back(tmp);
	return 0;
}

Configurations::Configurations(void)
{
	// < Adatbázis megnyitása
	if(sqlite3_open("bugsurv.db", &database)){
		BugSurv::CriticalErrorMessage("Adatbázis nem nyitható meg!");
	}
	// >

	// < Ha még nem létezik a CONNECTIONS adattábla, akkor létrehozza.
		  sql = "CREATE TABLE IF NOT EXISTS CONNECTIONS(" \
				"ID INTEGER PRIMARY KEY AUTOINCREMENT," \
				"NAME TEXT," \
				"HTTP_PORT INT,"\
				"REGISTER_PORT INT,"\
				"CONTROL_PORT INT,"\
				"STREAMING_PORT INT,"\
				"MULTICAST_PORT INT,"\
				"RTSP_PORT INT,"\
				"RTP_MC_VPORT INT,"\
				"RTP_MC_APORT INT,"\
				"USERNAME TEXT,"\
				"PASSWORD TEXT,"\
				"UNICAST_IP TEXT,"\
				"MULTICAST_IP TEXT,"\
				"CHANNEL INT,"\
				"CONTACT_TYPE INT,"\
				"PTZFILE TEXT,"\
				"CONNECTION_TYPE INT,"\
				"RTSP_ADDRESS TEXT,"\
				"USBCAM_INDEX INT,"\
				"SNAP_FOLDER TEXT,"\
				"REC_FOLDER TEXT"\
				");";
	if(SQLITE_OK != sqlite3_exec(database, sql, NULL, NULL, &errmsg)){
		BugSurv::CriticalErrorMessage(errmsg);
	}
	// >
}

// Visszaadja struktúrákat tartalmazó vectorban a feldolgozott lekérdezés eredményét.
const vector<Configurations::ConnectionConf>& Configurations::getConnectionConfs(bool refresh){
	// $ Ha az eddig tárolt rekordok száma 0 vagy kényszerített frissítés van, akkor újratölti a tábla adatait
	if(connections.size() == 0 || refresh){
		if(connections.size())connections.clear();
		sql = "SELECT ID, NAME, HTTP_PORT, REGISTER_PORT, CONTROL_PORT, STREAMING_PORT, MULTICAST_PORT, RTSP_PORT, RTP_MC_VPORT, RTP_MC_APORT, USERNAME, PASSWORD, UNICAST_IP, MULTICAST_IP, CHANNEL, CONTACT_TYPE, PTZFILE, CONNECTION_TYPE, RTSP_ADDRESS, USBCAM_INDEX, SNAP_FOLDER, REC_FOLDER FROM CONNECTIONS";
		if(SQLITE_OK != sqlite3_exec(database, sql, Configurations::sqlcb_connections, &connections, &errmsg)){
			BugSurv::CriticalErrorMessage(errmsg);
		}
	}
	return connections;
}

//Adatbázisban tárolt ID alapján kiválasztás
Configurations::ConnectionConf* Configurations::getConnectionConfById(int id){
	for(std::vector<ConnectionConf>::iterator it = connections.begin(); it != connections.end(); it++){
		if(it->id == id) return &(*it);
	}
	return 0;
}

//Adatbázisban tárolt ID alapján törlés
bool Configurations::delConnectionConfById(int id){
	// < Törlés az adatbázisból
	char sql[200];
	sprintf(sql, "DELETE FROM CONNECTIONS WHERE ID = %d", id);
	std::vector<ConnectionConf>::iterator it;
	// >
	
	// < Keresés a rekordok között helyileg
	for(it = connections.begin(); it != connections.end() && it->id != id; it++){
		if(it->id == id) return &(*it);
	}
	// >

	// < Törlés helyileg is 
	if(it != connections.end()){
		if(SQLITE_OK != sqlite3_exec(database, sql, 0, 0, &errmsg)){
			BugSurv::CriticalErrorMessage(errmsg);
			return false;
		} else {
			connections.erase(it);
		}
	} else {
		BugSurv::CriticalErrorMessage("Inkonzisztens kapcsolattábla!");
		return false;
	}
	// >
	return true;
}

//Kapcsolat konfiguráció hozzáadása vagy frissítése
bool Configurations::addConnectionConf(ConnectionConf tmp){
	std::vector<ConnectionConf> tempvec;
	char sql[3000];
	sprintf(sql, "SELECT ID, NAME, HTTP_PORT, REGISTER_PORT, CONTROL_PORT, STREAMING_PORT, MULTICAST_PORT, RTSP_PORT, RTP_MC_VPORT, RTP_MC_APORT, USERNAME, PASSWORD, UNICAST_IP, MULTICAST_IP, CHANNEL, CONTACT_TYPE, PTZFILE, CONNECTION_TYPE, RTSP_ADDRESS, USBCAM_INDEX, SNAP_FOLDER, REC_FOLDER FROM CONNECTIONS WHERE NAME = \"%s\"", tmp.name.c_str());
	if(SQLITE_OK != sqlite3_exec(database, sql, sqlcb_connections, &tempvec, &errmsg)){
		BugSurv::CriticalErrorMessage(errmsg);
		return false;
	}
	if(tempvec.empty()){
		// < Ha még nem létezik, akkor hozzáadja
		sprintf(sql, "INSERT INTO CONNECTIONS (ID, NAME, HTTP_PORT, REGISTER_PORT, CONTROL_PORT, STREAMING_PORT, MULTICAST_PORT, RTSP_PORT, RTP_MC_VPORT, RTP_MC_APORT, USERNAME, PASSWORD, UNICAST_IP, MULTICAST_IP, CHANNEL, CONTACT_TYPE, PTZFILE, CONNECTION_TYPE, RTSP_ADDRESS, USBCAM_INDEX, SNAP_FOLDER, REC_FOLDER) VALUES(NULL, \"%s\", %d, %d, %d, %d, %d, %d, %d, %d, \"%s\", \"%s\", \"%s\", \"%s\", %d, %d, \"%s\", %d, \"%s\", %d, \"%s\", \"%s\");", tmp.name.c_str(), tmp.httpPost, tmp.registerPort, tmp.controlPort, tmp.streamingPort, tmp.multicastPort, tmp.rtspPort, tmp.rtpMulticastVideoPort, tmp.rtpMulticastAudioPort, tmp.user.c_str(), tmp.password.c_str(), tmp.unicastIP.c_str(), tmp.multicastIP.c_str(), tmp.channelNumber, tmp.contactType, tmp.ptzfile.c_str(), tmp.connectionType, tmp.rtspAddress.c_str(), tmp.usbcamIndex, tmp.snapFolder.c_str(), tmp.recFolder.c_str());
		if(SQLITE_OK != sqlite3_exec(database, sql, 0, 0, &errmsg)){
			BugSurv::CriticalErrorMessage(errmsg);
			return false;
		} else {
			getConnectionConfs(true);
		}
		// >
	} else {
		// < Ha már létezik, akkor frissíti
		sprintf(sql, "UPDATE CONNECTIONS SET HTTP_PORT = %d, REGISTER_PORT = %d, CONTROL_PORT = %d, STREAMING_PORT = %d, MULTICAST_PORT = %d, RTSP_PORT = %d, RTP_MC_VPORT = %d, RTP_MC_APORT = %d, USERNAME = \"%s\", PASSWORD = \"%s\", UNICAST_IP = \"%s\", MULTICAST_IP = \"%s\", CHANNEL = %d, CONTACT_TYPE = %d, PTZFILE = \"%s\", CONNECTION_TYPE = \"%d\", RTSP_ADDRESS = \"%s\", USBCAM_INDEX = %d, SNAP_FOLDER = \"%s\", REC_FOLDER = \"%s\" WHERE ID = %d", tmp.httpPost, tmp.registerPort, tmp.controlPort, tmp.streamingPort, tmp.multicastPort, tmp.rtspPort, tmp.rtpMulticastVideoPort, tmp.rtpMulticastAudioPort, tmp.user.c_str(), tmp.password.c_str(), tmp.unicastIP.c_str(), tmp.multicastIP.c_str(), tmp.channelNumber, tmp.contactType, tmp.ptzfile.c_str(), tmp.connectionType, tmp.rtspAddress.c_str(), tmp.usbcamIndex, tmp.snapFolder.c_str(), tmp.recFolder.c_str(), tempvec.begin()->id);
		if(SQLITE_OK != sqlite3_exec(database, sql, 0, 0, &errmsg)){
			BugSurv::CriticalErrorMessage(errmsg);
			return false;
		} else {
			getConnectionConfs(true);
		}
		// >
	}
	return false;
}

Configurations::~Configurations(void)
{
	// $ Adatbázis lezárása
	sqlite3_close(database);
}
