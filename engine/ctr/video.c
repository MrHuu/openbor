/*
 * OpenBOR - http://www.chronocrash.com
 * -----------------------------------------------------------------------
 * All rights reserved, see LICENSE in OpenBOR root for details.
 *
 * Copyright (c) 2004 - 2014 OpenBOR Team
 */

#include <3ds.h>
#include <SDL_framerate.h>
#include <math.h>
#include "types.h"
#include "video.h"
#include "screen.h"
#include "ctrport.h"
#include "openbor.h"
#include "gfxtypes.h"
#include "gfx.h"

extern int videoMode;

#define SKIP_CODE

FPSmanager framerate_manager;
s_videomodes stored_videomodes;
static SDL_Surface *screen = NULL;
static SDL_Surface *bscreen = NULL;
static SDL_Surface *bscreen2 = NULL;
static SDL_Color colors[256];
static int bytes_per_pixel = 1;
int stretch = 0;
u8 pDeltaBuffer[480 * 2592];

void initSDL()
{
	const SDL_VideoInfo* video_info;
	int init_flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER;

	if(SDL_Init(init_flags) < 0)
	{
		printf("SDL Failed to Init!!!! (%s)\n", SDL_GetError());
		borExit(0);
	}
	SDL_ShowCursor(SDL_DISABLE);

	// Store the monitor's current resolution before setting the video mode for the first time
	video_info = SDL_GetVideoInfo();

	SDL_initFramerate(&framerate_manager);
	SDL_setFramerate(&framerate_manager, 200);
}

static unsigned masks[4][4] = {{0,0,0,0},{0x1F,0x07E0,0xF800,0},{0xFF,0xFF00,0xFF0000,0},{0xFF,0xFF00,0xFF0000,0}};

int video_set_mode(s_videomodes videomodes)
{
	stored_videomodes = videomodes;

	if(screen) SDL_FreeAndNullVideoSurface(screen);
	if(bscreen) { SDL_FreeSurface(bscreen); bscreen=NULL; }
	if(bscreen2) { SDL_FreeSurface(bscreen2); bscreen2=NULL; }

	bytes_per_pixel = videomodes.pixel;
	if(videomodes.hRes==0 && videomodes.vRes==0)
	{
		Term_Gfx();
		return 0;
	}

	if(savedata.screen[videoMode][0])
	{
		screen = SDL_SetVideoMode(videomodes.hRes*savedata.screen[videoMode][0],videomodes.vRes*savedata.screen[videoMode][0],16,SDL_SWSURFACE|SDL_DOUBLEBUF|SDL_FULLSCREEN);

		SDL_ShowCursor(SDL_DISABLE);
		bscreen = SDL_AllocSurface(SDL_SWSURFACE, videomodes.hRes, videomodes.vRes, 8*bytes_per_pixel, masks[bytes_per_pixel-1][0], masks[bytes_per_pixel-1][1], masks[bytes_per_pixel-1][2], masks[bytes_per_pixel-1][3]); // 24bit mask
		bscreen2 = SDL_AllocSurface(SDL_SWSURFACE, videomodes.hRes+4, videomodes.vRes+8, 16, masks[1][2], masks[1][1], masks[1][0], masks[1][3]);
		Init_Gfx(565, 16);
		memset(pDeltaBuffer, 0x00, 1244160);
		if(bscreen==NULL || bscreen2==NULL) return 0;
	}
	else
	{
		if(bytes_per_pixel>1)
		{
			bscreen = SDL_AllocSurface(SDL_SWSURFACE, videomodes.hRes, videomodes.vRes, 8*bytes_per_pixel, masks[bytes_per_pixel-1][0], masks[bytes_per_pixel-1][1], masks[bytes_per_pixel-1][2], masks[bytes_per_pixel-1][3]); // 24bit mask
			if(!bscreen) return 0;
		}

		 if(((videomodes.hRes>400) && (videomodes.vRes>240)) || !(savedata.stretch==0))
		{
			screen = SDL_SetVideoMode(videomodes.hRes,videomodes.vRes,8*bytes_per_pixel,SDL_SWSURFACE|SDL_DOUBLEBUF|SDL_FITWIDTH|SDL_FITHEIGHT);
		} else {
//			screen = SDL_SetVideoMode(videomodes.hRes,videomodes.vRes,8*bytes_per_pixel,SDL_SWSURFACE|SDL_DOUBLEBUF|SDL_TOPSCR|SDL_CONSOLEBOTTOM);
			screen = SDL_SetVideoMode(videomodes.hRes,videomodes.vRes,8*bytes_per_pixel,SDL_SWSURFACE|SDL_DOUBLEBUF|SDL_TOPSCR);
		}

		SDL_ShowCursor(SDL_DISABLE);
	}

	if(bytes_per_pixel==1)
	{
		SDL_SetColors(screen,colors,0,256);
		if(bscreen) SDL_SetColors(bscreen,colors,0,256);
	}

	if(screen==NULL) return 0;

	video_clearscreen();
	return 1;
}

void video_fullscreen_flip()
{
	size_t w, h;

	// switch between SDL fullscreen and SDL windowed
	if(screen) {
		w = screen->w;
		h = screen->h;
		SDL_FreeVideoSurface(screen);
	} else {
		w = 320;
		h = 240;
	}

	if(savedata.stretch==0) {
		screen = SDL_SetVideoMode(w,h,8*bytes_per_pixel,SDL_SWSURFACE|SDL_DOUBLEBUF|SDL_TOPSCR);
	} else {
		screen = SDL_SetVideoMode(w,h,8*bytes_per_pixel,SDL_SWSURFACE|SDL_DOUBLEBUF|SDL_FITWIDTH|SDL_FITHEIGHT);
	}

	SDL_ShowCursor(SDL_DISABLE);
	SDL_SetColors(screen,colors,0,256);
	if(bscreen) SDL_SetColors(bscreen,colors,0,256);
}

int video_copy_screen(s_screen* src)
{
	unsigned char *sp;
	char *dp;
	int width, height, linew, slinew;
	int h;
	SDL_Surface* ds = NULL;
	SDL_Rect rectdes, rectsrc;

	width = screen->w;
	if(width > src->width) width = src->width;
	height = screen->h;
	if(height > src->height) height = src->height;
	if(!width || !height) return 0;
	h = height;

	if(bscreen)
	{
		rectdes.x = rectdes.y = 0;
		rectdes.w = width*savedata.screen[videoMode][0]; rectdes.h = height*savedata.screen[videoMode][0];
		if(bscreen2) {rectsrc.x = 2; rectsrc.y = 4;}
		else         {rectsrc.x = 0; rectsrc.y = 0;}
		rectsrc.w = width; rectsrc.h = height;
		if(SDL_MUSTLOCK(bscreen)) SDL_LockSurface(bscreen);
	}

	// Copy to linear video ram
	if(SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);

	sp = (unsigned char*)src->data;
	ds = (bscreen?bscreen:screen);
	dp = ds->pixels;

	linew = width*bytes_per_pixel;
	slinew = src->width*bytes_per_pixel;

	do{
		memcpy(dp, sp, linew);
		sp += slinew;
		dp += ds->pitch;
	}while(--h);

	if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);

	if(bscreen)
	{
		if(SDL_MUSTLOCK(bscreen)) SDL_UnlockSurface(bscreen);
		if(bscreen2) SDL_BlitSurface(bscreen, NULL, bscreen2, &rectsrc);
		else         SDL_BlitSurface(bscreen, NULL, screen, &rectsrc);
		if(bscreen2)
		{
			if(bscreen2 && SDL_MUSTLOCK(bscreen2)) SDL_LockSurface(bscreen2);
			if(SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);

			if(savedata.screen[videoMode][0]==2) (*GfxBlitters[(int)savedata.screen[videoMode][1]])((u8*)bscreen2->pixels+bscreen2->pitch*4+4, bscreen2->pitch, pDeltaBuffer+bscreen2->pitch, (u8*)screen->pixels, screen->pitch, screen->w>>1, screen->h>>1);

			if(SDL_MUSTLOCK(bscreen2)) SDL_UnlockSurface(bscreen2);
			if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
		}
	}
	SDL_Flip(screen);

	return 1;
}

void video_clearscreen()
{
	if(SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);
	memset(screen->pixels, 0, screen->pitch*screen->h);
	if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
	if(bscreen)
	{
		if(SDL_MUSTLOCK(bscreen)) SDL_LockSurface(bscreen);
		memset(bscreen->pixels, 0, bscreen->pitch*bscreen->h);
		if(SDL_MUSTLOCK(bscreen)) SDL_UnlockSurface(bscreen);
	}
}

void video_stretch(int enable)
{
	stretch = enable;
}

void vga_vwait(void)
{
	static int prevtick = 0;
	int now = SDL_GetTicks();
	int wait = 1000/60 - (now - prevtick);
	if (wait>0)
	{
		SDL_Delay(wait);
	}
	else SDL_Delay(1);
	prevtick = now;
}

void vga_setpalette(unsigned char* palette)
{
	
	int i;
	for(i=0;i<256;i++){
		colors[i].r=palette[0];
		colors[i].g=palette[1];
		colors[i].b=palette[2];
		palette+=3;
	}

	SDL_SetColors(screen,colors,0,256);
	if(bscreen) SDL_SetColors(bscreen,colors,0,256);

}

void vga_set_color_correction(int gm, int br)
{
}

