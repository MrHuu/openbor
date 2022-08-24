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

void backlightEnable(bool enable, u32 screen)
{
	u8 device_model = 0xFF;
	CFGU_GetSystemModel(&device_model);
	if (device_model != CFG_MODEL_2DS)
	{
		gspLcdInit();
		enable ? GSPLCD_PowerOnBacklight(screen):GSPLCD_PowerOffBacklight(screen);
		gspLcdExit();
	}
}

void borExit(int reset)
{
	backlightEnable(true,GSPLCD_SCREEN_BOTTOM);
#ifdef CTR_ROMFS
	romfsExit();
#endif
	cfguExit();
	exit(0);
}

void aptHookFunc(APT_HookType hookType, void *param)
{
	switch (hookType) {
		case APTHOOK_ONSUSPEND:
			backlightEnable(true,GSPLCD_SCREEN_BOTTOM);
			break;
		case APTHOOK_ONRESTORE:
		case APTHOOK_ONWAKEUP:
			backlightEnable(false,GSPLCD_SCREEN_BOTTOM);
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

	cfguInit();
	backlightEnable(false,GSPLCD_SCREEN_BOTTOM);

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

