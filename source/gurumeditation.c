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

#include <nds/ndstypes.h>
#include <nds/memory.h>

#include <nds/timers.h>
#include <nds/arm9/video.h>
#include <nds/arm9/console.h>
#include <nds/arm9/exceptions.h>
#include <nds/arm9/background.h>

#include <calico/system/dietprint.h>

#include "display.h"

//---------------------------------------------------------------------------------
static void initSystem(void) {
//---------------------------------------------------------------------------------
	register int i;
	// stop timers and dma
	for (i=0; i<4; i++)
	{
		DMA_CR(i) = 0;
		DMA_SRC(i) = 0;
		DMA_DEST(i) = 0;
		TIMER_CR(i) = 0;
		TIMER_DATA(i) = 0;
	}

	// clear video display registers
	dmaFillWords(0, (void*)0x04000000, 0x56);
	dmaFillWords(0, (void*)0x04001008, 0x56);

	videoSetModeSub(0);

	vramDefault();

	VRAM_E_CR = 0;
	VRAM_F_CR = 0;
	VRAM_G_CR = 0;
	VRAM_H_CR = 0;
	VRAM_I_CR = 0;
}

//---------------------------------------------------------------------------------
static void excepthandler(ExcptContext* ctx, unsigned flags) {
//---------------------------------------------------------------------------------
	REG_MASTER_BRIGHT = 0;
	REG_MASTER_BRIGHT_SUB = 0;

	initSystem();
	initDisplay();

	guruMeditationDump(ctx, flags);

	u32* stack = (u32*)ctx->r[13];
	for (unsigned i = 0; i < 8; i ++) {
		dietPrint("\n  %08lX:  %08lX %08lX", (u32)&stack[i*2], stack[i*2], stack[(i*2)+1]);
	}
}

ExcptHandler g_excptHandler = excepthandler;
