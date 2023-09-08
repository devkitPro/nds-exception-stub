/*---------------------------------------------------------------------------------

  Copyright (C) 2005 - 2017
  	Dave Murphy (WinterMute)

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any
  damages arising from the use of this software.

  Permission is granted to anyone to use this software for any
  purpose, including commercial applications, and to alter it and
  redistribute it freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you
     must not claim that you wrote the original software. If you use
     this software in a product, an acknowledgment in the product
     documentation would be appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and
     must not be misrepresented as being the original software.
  3. This notice may not be removed or altered from any source
     distribution.

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <calico/system/dietprint.h>

#include "font_bin.h"

typedef struct con {
	int x,y;
} consoleVars;


static consoleVars topCON = {0,0} , btmCON = {0,0};

static consoleVars *currentCON = &topCON;


//---------------------------------------------------------------------------------
void setScreen(int screen) {
//---------------------------------------------------------------------------------
	currentCON = (screen == 0 ) ? &btmCON : &topCON;
}

//---------------------------------------------------------------------------------
// upcvt_4bit()
// Convert a 1-bit font to GBA 4-bit format.
//---------------------------------------------------------------------------------
void upcvt_4bit(void *dst, const u8 *src, u32 len) {
//---------------------------------------------------------------------------------
	u32 *out = dst;

	for(; len > 0; len--) {
		u32 dst_bits = 0;
		u32 src_bits = *src++;
		u32 x;

		for(x = 0; x < 8; x++) {
			dst_bits <<= 4;
			dst_bits |= src_bits & 1;
			src_bits >>= 1;
		}
	*out++ = dst_bits;
	}
}

//---------------------------------------------------------------------------------
static void newRow() {
//---------------------------------------------------------------------------------
	currentCON->y++;
	currentCON->x = 0;
	if (currentCON->y > 23) {
		u32 *src = (u32*)(BG_MAP_RAM(4) + 32);
		u32 *dst = (u32*)BG_MAP_RAM(4);
		memcpy(dst,src,64*24);
		currentCON->y = 23;
	}
}

//---------------------------------------------------------------------------------
static void writeChar(char c) {
//---------------------------------------------------------------------------------
	u16 *mapcell = BG_MAP_RAM(4) + currentCON->x + (currentCON->y * 32);
	switch(c) {

	case 10:
		newRow();
	case 13:
		currentCON->x = 0;
		break;
	default:
		*mapcell = c;
		currentCON->x++;
		break;
	}
	if (currentCON->x > 31) newRow();
}

//---------------------------------------------------------------------------------
static void writeString(const char *str, size_t len) {
//---------------------------------------------------------------------------------
	while(len--) {
		writeChar(str ? (*str++) : ' ');
	}
}

//---------------------------------------------------------------------------------
void setCursor(int row, int column) {
//---------------------------------------------------------------------------------
	currentCON->x = column;
	currentCON->y = row;
}

void getCursor(int *row, int*column) {
	*column = currentCON->x;
	*row = currentCON->y;
}

//---------------------------------------------------------------------------------
void initDisplay() {
//---------------------------------------------------------------------------------
	videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE);
	videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE);

	upcvt_4bit(BG_TILE_RAM(0),font_bin,font_bin_size);
	upcvt_4bit(BG_TILE_RAM_SUB(0),font_bin,font_bin_size);
	BGCTRL[0] = BG_MAP_BASE(4) | BG_TILE_BASE(0) | BG_COLOR_16 | BG_32x32;
	BGCTRL_SUB[0] = BG_MAP_BASE(4) | BG_TILE_BASE(0) | BG_COLOR_16 | BG_32x32;

	u16 *top = BG_MAP_RAM(4);
	u16 *btm = BG_MAP_RAM_SUB(4);
	int i;
	for ( i = 0; i < 32*32; i++ ) {
		*top++ = 0x20;
		*btm++ = 0x20;
	}
	BG_PALETTE[0]=RGB5(31,0,0);
	BG_PALETTE[1]=RGB5(31,31,31);
	BG_PALETTE_SUB[0]=RGB5(31,0,0);
	BG_PALETTE_SUB[1]=RGB5(31,31,31);

	dietPrintSetFunc(writeString);
}

//---------------------------------------------------------------------------------
u32 vramDefault() {
//---------------------------------------------------------------------------------
	u32 tmp = VRAM_CR;
	vramSetBankA(VRAM_A_MAIN_BG);
	vramSetBankC(VRAM_C_SUB_BG);
	return tmp;
}
