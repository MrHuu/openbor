/*
 * OpenBOR - http://www.chronocrash.com
 * -----------------------------------------------------------------------
 * All rights reserved, see LICENSE in OpenBOR root for details.
 *
 * Copyright (c) 2004 - 2014 OpenBOR Team
 */

#ifndef SDLPORT_H
#define SDLPORT_H

#include <SDL.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include "globals.h"

#if CTR
#include "3ds.h"
_Bool isN3DS;
#endif

#define SDL_FreeVideoSurface(X) SDL_FreeSurface(X)
#define SDL_FreeAndNullVideoSurface(X) { SDL_FreeSurface(X); X=NULL; }

//#define MEMTEST 1

void initSDL();
void borExit(int reset);
void openborMain(int argc, char** argv);

extern char packfile[MAX_FILENAME_LEN];
extern char paksDir[MAX_FILENAME_LEN];
extern char savesDir[MAX_FILENAME_LEN];
extern char logsDir[MAX_FILENAME_LEN];
extern char screenShotsDir[MAX_FILENAME_LEN];

#endif
