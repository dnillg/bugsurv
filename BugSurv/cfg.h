/*
** @project			BugSurv Framework
** @file			cfg.h
** @author			Giczi Dániel
** @contact			reactorx2@gmail.com
** @version			1.1 (2014. 12. 03.)
** @description	
		Külön fájlba gyûjtve az elérhetõ konfigurációs define direktívák.
*/

#ifndef CFG_H
#define CFG_H

// < Konfigurációs define direktívák

// $ Akkor használd, ha kevés a CPU (villogást okozhat rendereléskor)
//#define LOWRESOURCES

// $ ACTi SDK által visszaadott FPS érték efölött érvénytelen
#define ACTI_MAXFPS 60

// $ RTSP csatlakozás során ennyi idõ elteltével nézi meg, hogy fut-e még a szál
#define CVRTSP_TIMEOUT 3000

// $ UI alapértelmezett kapcsolatának neve
#define DEFAULT_CONNECTION_NAME "(ismeretlen kapcsolat)"

// >

#endif // CFG_H