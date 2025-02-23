/*
	This file is part of FreeIntv.

	FreeIntv is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	FreeIntv is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along
	with FreeIntv; if not, write to the Free Software Foundation, Inc.,
	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#include <math.h>
#include "controller.h"
#include "memory.h"

const double PI = 3.14159265358979323846;

#define K_1 0x81
#define K_2 0x41
#define K_3 0x21
#define K_4 0x22
#define K_5 0x84
#define K_6 0x82
#define K_7 0x42
#define K_8 0x44
#define K_9 0x48
#define K_0 0x24
#define K_C 0x28
#define K_E 0x88

#define B_TOP 0xA0
#define B_LEFT 0x60
#define B_RIGHT 0xC0

#define D_N 0x04
#define D_NNE 0x14
#define D_NE 0x16
#define D_ENE 0x06
#define D_E 0x02
#define D_ESE 0x12
#define D_SE 0x13
#define D_SSE 0x03
#define D_S 0x01
#define D_SSW 0x11
#define D_SW 0x19
#define D_WSW 0x09
#define D_W 0x08
#define D_WNW 0x18
#define D_NW 0x1C
#define D_NNW 0x0C

int controllerSwap;

// 16-way DISC directions, clockwise from North
int discDirections[16] = {D_N, D_NNE, D_NE, D_ENE, D_E, D_ESE, D_SE, D_SSE, D_S, D_SSW, D_SW, D_WSW, D_W, D_WNW, D_NW, D_NNW};
int keypadDirections[8] = {K_2, K_3, K_6, K_9, K_8, K_7, K_4, K_1};

// keypad states: 1,2,3, 4,5,6, 7,8,9, C,0,E
int keypadStates[12] = {K_1, K_2, K_3, K_4, K_5, K_6, K_7, K_8, K_9, K_C, K_0, K_E};

int cursor[4] = {0, 0, 0, 0}; // mini keypad cursor (button row/column p0x,p0y p1x,p1y)

//int getQuickKeypadState(int player);

void controllerInit()
{
	controllerSwap = 0;
	// by default input 0 maps to Right Controller (0x1FE)
	// and input 1 maps to Left Controller (0x1FF)
	// pressing select (freeintv_libretro.c) will
	// swap the left and right controllers
}

void setControllerInput(int player, int state)
{
	Memory[(player ^ controllerSwap) + 0x1FE] = (state ^ 0xFF) & 0xFF;
}

int getControllerState(int joypad[], int player)
{
	// converts joypad input for use by system
	// int Lx = 0;	  // left analog X
	// int Ly = 0;	  // left analog Y
	// int Rx = 0;	  // right analog X
	// int Ry = 0;	  // right analog Y
	// double theta; // analog joy angle
	// int norm;	  // theta, normalized

	int state = 0; // 0xFF;

	if (joypad[0] != 0)
	{
		state |= D_N;
	} // 0xFB - Up
	if (joypad[1] != 0)
	{
		state |= D_S;
	} // 0xFE - Down
	if (joypad[2] != 0)
	{
		state |= D_W;
	} // 0xF7 - Left
	if (joypad[3] != 0)
	{
		state |= D_E;
	} // 0xFD - Right

	if (joypad[0] != 0 && joypad[2] != 0)
	{
		state |= D_NW;
	} // 0xE3 - Up+Left
	if (joypad[0] != 0 && joypad[3] != 0)
	{
		state |= D_NE;
	} // 0xE9 - Up+Right
	if (joypad[1] != 0 && joypad[2] != 0)
	{
		state |= D_SW;
	} // 0x36 - Down+Left
	if (joypad[1] != 0 && joypad[3] != 0)
	{
		state |= D_SE;
	} // 0x3C - Down+Right

	if (joypad[4] != 0)
	{
		state |= B_LEFT;
	} // 0x9F - Button Left
	if (joypad[5] != 0)
	{
		state |= B_RIGHT;
	} // 0x3F - Button Right

	if (joypad[7] != 0)
	{
		state |= B_TOP;
	} 


	/* Analog Controls for 16-way disc control */
	// no longer using analog

	// Lx = joypad[14] / 8192;
	// Ly = joypad[15] / 8192;
	// if(Lx != 0 || Ly != 0)
	// {
	// 	// find angle
	// 	theta = atan2((double)Ly, (double)Lx) + PI;
	// 	// normalize
	// 	if(theta<0.0) { theta = 0.0; }
	// 	norm = floor((theta/(2*PI))*15.0);
	// 	norm -= 3;
	// 	if(norm<0) { norm += 16; }
	// 	state |= discDirections[norm & 0x0F];
	// }

	// // Right-analog to keypad mapping (for Tron Deadly Discs)
	// Rx = joypad[16] / 8192;
	// Ry = joypad[17] / 8192;
	// if(Rx != 0 || Ry != 0)
	// {
	// 	// find angle
	// 	theta = atan2((double)Ry, (double)Rx) + PI;
	// 	// normalize
	// 	if(theta<0.0) { theta = 0.0; }
	// 	norm = floor((theta/(2*PI))*7.0);
	// 	norm -= 1;
	// 	if(norm<0) { norm += 8; }
	// 	state |= keypadDirections[norm & 0x07];
	// }

	// Thumbsticks for Keypad 0/5
	// if(joypad[18]!=0) { state |= K_0; } // 0x48 - Keypad 0
	// if(joypad[19]!=0) { state |= K_5; } // 0x42 - Keypad 5

	// // L/R triggers for Keypad Enter/Clear
	// if(joypad[12]!=0) { state |= K_C; } // 0x88 - Keypad Clear
	// if(joypad[13]!=0) { state |= K_E; } // 0x28 - Keypad Enter

	if (joypad[10] != 0)
	{
		state |= K_1;
	}

	if (joypad[11] != 0)
	{
		state |= K_2;
	}

	if (joypad[6] != 0)
	{
		state |= K_3;
	}

	if (joypad[12] != 0)
	{
		state |= K_4;
	}

	if (joypad[13] != 0)
	{
		state |= K_5;
	}

	if (joypad[14] != 0)
	{
		state |= K_6;
	}

	if (joypad[15] != 0)
	{
		state |= K_7;
	}

	if (joypad[16] != 0)
	{
		state |= K_8;
	}

	if (joypad[17] != 0)
	{
		state |= K_9;
	}

	if (joypad[18] != 0)
	{
		state |= K_C;
	}

	if (joypad[19] != 0)
	{
		state |= K_0;
	}

	if (joypad[20] != 0)
	{
		state |= K_E;
	}

	return state;
}
