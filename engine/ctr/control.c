/*
 * OpenBOR - http://www.chronocrash.com
 * -----------------------------------------------------------------------
 * Licensed under a BSD-style license, see LICENSE in OpenBOR root for details.
 *
 * Copyright (c) 2004 - 2017 OpenBOR Team
 */

#include <string.h>
#include <3ds.h>
#include "ctrport.h"
#include "control.h"
#include "openbor.h"

#define	PAD_START			1
#define	MAX_PADS			1
#define	PAD_END				(18*MAX_PADS)

static int usejoy;
static int lastkey[MAX_PADS];

static const char *padnames[PAD_END+1+1] = {
	"...",
#define CONTROLNAMES(x) \
	x" Up",             \
	x" Right",          \
	x" Down",           \
	x" Left",           \
	x" A",              \
	x" B",              \
	x" X",              \
	x" Y",              \
	x" LT",             \
	x" RT",             \
	x" Start",          \
	x" Select",         \
	x" ZL",             \
	x" ZR",
	CONTROLNAMES("3DS")
	"undefined"
};

static unsigned int getPad(int port);

static int flag_to_index(unsigned int flag)
{
	int index = 0;
	unsigned int bit = 1;
	while (!((bit<<index)&flag) && index<31) ++index;
	return index;
}

void control_exit()
{
	usejoy = 0;
}

void control_init(int joy_enable)
{
	usejoy = joy_enable;
}

int control_usejoy(int enable)
{
	usejoy = enable;
	return 0;
}

int control_getjoyenabled()
{
	return usejoy;
}

int keyboard_getlastkey(void)
{
	int i, ret=0;
	for (i=0; i<MAX_PADS; i++)
	{
		ret |= lastkey[i];
		lastkey[i] = 0;
	}
	return ret;
}

void control_setkey(s_playercontrols * pcontrols, unsigned int flag, int key)
{
	if (!pcontrols) return;
	pcontrols->settings[flag_to_index(flag)] = key;
	pcontrols->keyflags = pcontrols->newkeyflags = 0;
}

// Scan input for newly-pressed keys.
// Return value:
// 0  = no key was pressed
// >0 = key code for pressed key
// <0 = error
int control_scankey()
{
	static unsigned ready = 0;
	unsigned i, k=0;

	for (i=0; i<MAX_PADS; i++)
	{
		if (lastkey[i])
		{
			k = 1 + i*18 + flag_to_index(lastkey[i]);
			break;
		}
	}

	if (ready && k)
	{
		ready = 0;
		return k;
	}
	ready = (!k);
	return 0;
}

char * control_getkeyname(unsigned keycode)
{
	if (keycode >= PAD_START && keycode <= PAD_END) return (char*)padnames[keycode];
	return "...";
}

void control_update(s_playercontrols ** playercontrols, int numplayers)
{
	unsigned int k;
	unsigned int i;
	int player;
	int t;
	s_playercontrols * pcontrols;
	unsigned port[MAX_PADS];
	for (i=0; i<MAX_PADS; i++) port[i] = getPad(i);
	for (player=0; player<numplayers; player++)
	{
		pcontrols = playercontrols[player];
		k = 0;
		for (i=0; i<32; i++)
		{
			t = pcontrols->settings[i];
			if (t >= PAD_START && t <= PAD_END)
			{
				int portnum = (t-1) / 18;
				int shiftby = (t-1) % 18;
				if (portnum >= 0 && portnum <= 3)
				{
					if ((port[portnum] >> shiftby) & 1) k |= (1<<i);
				}
			}
		}
		pcontrols->kb_break = 0;
		pcontrols->newkeyflags = k & (~pcontrols->keyflags);
		pcontrols->keyflags = k;
	}
}

void control_rumble(int port, int ratio, int msec)
{
}

static unsigned int getPad(int port)
{
	unsigned int btns = 0;

	SDL_Event ev;
	while(SDL_PollEvent(&ev))
	{
		switch(ev.type)
		{
			case SDL_QUIT:
				borShutdown(2, "HUU");
				borExit(0);
				break;
		}
	}

	aptMainLoop();
	hidScanInput();
	u32 kHeld = hidKeysHeld();

	if (port != 0) return lastkey[port] = 0;

	if (kHeld & KEY_UP)	      btns |= CTR_DPAD_UP;
	if (kHeld & KEY_RIGHT)    btns |= CTR_DPAD_RIGHT;
	if (kHeld & KEY_DOWN)     btns |= CTR_DPAD_DOWN;
	if (kHeld & KEY_LEFT)     btns |= CTR_DPAD_LEFT;
	if (kHeld & KEY_A)        btns |= CTR_A;
	if (kHeld & KEY_B)        btns |= CTR_B;
	if (kHeld & KEY_X)        btns |= CTR_X;
	if (kHeld & KEY_Y)        btns |= CTR_Y;
	if (kHeld & KEY_L)        btns |= CTR_LEFT_TRIGGER;
	if (kHeld & KEY_R)        btns |= CTR_RIGHT_TRIGGER;
	if (kHeld & KEY_SELECT)   btns |= CTR_SELECT;
	if (kHeld & KEY_START)    btns |= CTR_START;
	if (kHeld & KEY_ZL)       btns |= CTR_ZL;
	if (kHeld & KEY_ZR)       btns |= CTR_ZR;

	return lastkey[port] = btns;
}
