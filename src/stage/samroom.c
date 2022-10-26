/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "samroom.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//SAM Room background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //samroombg1
	Gfx_Tex tex_back1; //samroombg2

} Back_SAMRoom;

void Back_SAMRoom_DrawBG(StageBack *back)
{
	Back_SAMRoom *this = (Back_SAMRoom*)back;
	
	fixed_t fx, fy;
	
	//Draw samroombg
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT samroombg1_src = {0, 0,244,255};
	RECT_FIXED samroombg1_dst = {
		FIXED_DEC(-151 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-20,1) - fy,
		FIXED_DEC(231 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(241,1)
	};
	
	RECT samroombg2_src = {0, 0,166,255};
	RECT_FIXED samroombg2_dst = {
		FIXED_DEC(79 + screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-20,1) - fy,
		FIXED_DEC(157 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(241,1)
	};
	
	Debug_StageMoveDebug(&samroombg1_dst, 8, fx, fy);
	Debug_StageMoveDebug(&samroombg2_dst, 9, fx, fy);
	Stage_DrawTex(&this->tex_back0, &samroombg1_src, &samroombg1_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &samroombg2_src, &samroombg2_dst, stage.camera.bzoom);
}

void Back_SAMRoom_Free(StageBack *back)
{
	Back_SAMRoom *this = (Back_SAMRoom*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_SAMRoom_New(void)
{
	//Allocate background structure
	Back_SAMRoom *this = (Back_SAMRoom*)Mem_Alloc(sizeof(Back_SAMRoom));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_SAMRoom_DrawBG;
	this->back.free = Back_SAMRoom_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\SAMROOM\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
