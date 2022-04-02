/*
 * OpenBOR - http://www.chronocrash.com
 * -----------------------------------------------------------------------
 * All rights reserved, see LICENSE in OpenBOR root for details.
 *
 * Copyright (c) 2004 - 2014 OpenBOR Team
 */

#include <3ds.h>

#include "ctrport.h"
#include "openbor.h"
#include "packfile.h"
#include "ram.h"
#include "video.h"
#include "menu.h"

aptHookCookie cookie;

char packfile[MAX_FILENAME_LEN]       = {"bor.pak"};
#ifdef CTR_ROMFS
char paksDir[MAX_FILENAME_LEN]        = {"romfs:/"};
#else
char paksDir[MAX_FILENAME_LEN]        = {"Paks"};
#endif
char savesDir[MAX_FILENAME_LEN]       = {"Saves"};
char logsDir[MAX_FILENAME_LEN]        = {"Logs"};
char screenShotsDir[MAX_FILENAME_LEN] = {"ScreenShots"};


void borExit(int reset)
{
	gspLcdInit();
	GSPLCD_PowerOnBacklight(GSPLCD_SCREEN_BOTH);
	gspLcdExit();
#ifdef CTR_ROMFS
	romfsExit();
#endif
	exit(0);
}

void aptHookFunc(APT_HookType hookType, void *param)
{
	switch (hookType) {
		case APTHOOK_ONSUSPEND:
			gspLcdInit();
			GSPLCD_PowerOnBacklight(GSPLCD_SCREEN_BOTH);
			gspLcdExit();
		case APTHOOK_ONSLEEP:
			break;
		case APTHOOK_ONRESTORE:
		case APTHOOK_ONWAKEUP:
			gspLcdInit();
			GSPLCD_PowerOffBacklight(GSPLCD_SCREEN_BOTTOM);
			gspLcdExit();
			break;
		default:
			break;
	}
}

int main(int argc, char *argv[])
{
	aptHook(&cookie, aptHookFunc, NULL);

	_Bool isN3DS;
	APT_CheckNew3DS(&isN3DS);
	if(isN3DS)
		osSetSpeedupEnable(true);

	gspLcdInit();
	GSPLCD_PowerOffBacklight(GSPLCD_SCREEN_BOTTOM);
	gspLcdExit();

	setSystemRam();
	initSDL();
#ifdef CTR_ROMFS
	romfsInit();
#endif
	char resourcePath[128];
    strcpy(resourcePath,"sdmc:/3ds/OpenBOR");
	dirExists(resourcePath, 1);
    chdir(resourcePath);

	packfile_mode(0);
	dirExists(paksDir, 1);
	dirExists(savesDir, 1);
	dirExists(logsDir, 1);
	dirExists(screenShotsDir, 1);

	Menu();
	openborMain(argc, argv);

	return 0;
}

