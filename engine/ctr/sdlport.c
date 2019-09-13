/*
 * OpenBOR - http://www.chronocrash.com
 * -----------------------------------------------------------------------
 * All rights reserved, see LICENSE in OpenBOR root for details.
 *
 * Copyright (c) 2004 - 2014 OpenBOR Team
 */

#include "sdlport.h"
#include "packfile.h"
#include "ram.h"
#include "video.h"
#include "menu.h"

#ifdef CTR
#define appExit exit
#undef exit
#endif

char packfile[MAX_FILENAME_LEN] = {"bor.pak"};
char paksDir[MAX_FILENAME_LEN] = {"Paks"};
char savesDir[MAX_FILENAME_LEN] = {"Saves"};
char logsDir[MAX_FILENAME_LEN] = {"Logs"};
char screenShotsDir[MAX_FILENAME_LEN] = {"ScreenShots"};

void borExit(int reset)
{

	SDL_Delay(1000);
	SDL_Quit();

}

aptHookCookie cookie;

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
			gspLcdInit();
			GSPLCD_PowerOffBacklight(GSPLCD_SCREEN_BOTTOM);
			gspLcdExit();
			break;
		case APTHOOK_ONWAKEUP:
			gspLcdInit();
			GSPLCD_PowerOffBacklight(GSPLCD_SCREEN_BOTTOM);
			gspLcdExit();
			break;
		case APTHOOK_ONEXIT:
			SDL_Quit();
			break;
		default:
			break;
	}
}

int main(int argc, char *argv[])
{
	
	aptHook(&cookie, aptHookFunc, NULL);

	char resourcePath[128];
    strcpy(resourcePath,"sdmc:/3ds/OpenBOR");
	dirExists(resourcePath, 1);
    chdir(resourcePath);

	APT_CheckNew3DS(&isN3DS);
	if(isN3DS)
		osSetSpeedupEnable(true);

	gspLcdInit();
	GSPLCD_PowerOffBacklight(GSPLCD_SCREEN_BOTTOM);
	gspLcdExit();

	setSystemRam();
	initSDL();

	gfxFlushBuffers();

	packfile_mode(0);

	dirExists(paksDir, 1);
	dirExists(savesDir, 1);
	dirExists(logsDir, 1);
	dirExists(screenShotsDir, 1);
	Menu();
	openborMain(argc, argv);
	borExit(0);

	gspLcdInit();
	GSPLCD_PowerOnBacklight(GSPLCD_SCREEN_BOTTOM);
	gspLcdExit();

	return 0;
}

