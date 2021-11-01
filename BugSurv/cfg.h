/*
** @project			BugSurv Framework
** @file			cfg.h
** @author			Giczi D�niel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		K�l�n f�jlba gy�jtve az el�rhet� konfigur�ci�s define direkt�v�k.
*/

#ifndef CFG_H
#define CFG_H

// < Konfigur�ci�s define direkt�v�k

// $ Akkor haszn�ld, ha kev�s a CPU (villog�st okozhat renderel�skor)
//#define LOWRESOURCES

// $ ACTi SDK �ltal visszaadott FPS �rt�k ef�l�tt �rv�nytelen
#define ACTI_MAXFPS 60

// $ RTSP csatlakoz�s sor�n ennyi id� eltelt�vel n�zi meg, hogy fut-e m�g a sz�l
#define CVRTSP_TIMEOUT 3000

// $ UI alap�rtelmezett kapcsolat�nak neve
#define DEFAULT_CONNECTION_NAME "(ismeretlen kapcsolat)"

// >

#endif // CFG_H