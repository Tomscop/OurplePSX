/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "stage01.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Stage01 background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_0; //stage0
	Gfx_Tex tex_1; //stage1
	Gfx_Tex tex_2; //stage2
	Gfx_Tex tex_3; //black

} Back_Stage01;


void Back_Stage01_DrawBG(StageBack *back)
{
	Back_Stage01 *this = (Back_Stage01*)back;
	
	fixed_t fx, fy;
	
	//Draw stage
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT stage0_src = {0, 0,248,128};
	RECT_FIXED stage0_dst = {
		FIXED_DEC(-151 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-31,1) - fy,
		FIXED_DEC(432 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(233,1)
	};
	
	RECT stage1_src = {0, 0,248,128};
	RECT_FIXED stage1_dst = {
		FIXED_DEC(-151 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-31,1) - fy,
		FIXED_DEC(432 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(233,1)
	};
	
	RECT stage2_src = {0, 0,248,128};
	RECT_FIXED stage2_dst = {
		FIXED_DEC(-151 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-31,1) - fy,
		FIXED_DEC(432 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(233,1)
	};
	
	RECT black_src = {0, 0,64,64};
	RECT_FIXED black_dst = {
		FIXED_DEC(-310 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-62,1) - fy,
		FIXED_DEC(550 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(300,1)
	};
	
	Debug_StageMoveDebug(&stage0_dst, 7, fx, fy);
	Debug_StageMoveDebug(&stage1_dst, 8, fx, fy);
	Debug_StageMoveDebug(&stage2_dst, 9, fx, fy);
	Debug_StageMoveDebug(&black_dst, 6, fx, fy);
	if (stage.song_step >= 943)
		Stage_DrawTex(&this->tex_2, &stage2_src, &stage2_dst, stage.camera.bzoom);
	if (stage.song_step >= 656)
		Stage_DrawTex(&this->tex_1, &stage1_src, &stage1_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_0, &stage0_src, &stage0_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_3, &black_src, &black_dst, stage.camera.bzoom);
}

void Back_Stage01_Free(StageBack *back)
{
	Back_Stage01 *this = (Back_Stage01*)back;
	

	//Free structure
	Mem_Free(this);
}

StageBack *Back_Stage01_New(void)
{
	//Allocate background structure
	Back_Stage01 *this = (Back_Stage01*)Mem_Alloc(sizeof(Back_Stage01));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Stage01_DrawBG;
	this->back.free = Back_Stage01_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\STAGE01\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_0, Archive_Find(arc_back, "0.tim"), 0);
	Gfx_LoadTex(&this->tex_1, Archive_Find(arc_back, "1.tim"), 0);
	Gfx_LoadTex(&this->tex_2, Archive_Find(arc_back, "2.tim"), 0);
	Gfx_LoadTex(&this->tex_3, Archive_Find(arc_back, "black.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
