/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "flipside.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Flipside background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //lake

} Back_Flipside;


void Back_Flipside_DrawBG(StageBack *back)
{
	Back_Flipside *this = (Back_Flipside*)back;
	
	fixed_t fx, fy;

	//Draw lake
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT lake_src = {0, 0, 255, 255};
	RECT_FIXED lake_dst = {
		FIXED_DEC(-235 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-205,1) - fy,
		FIXED_DEC(642 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(451,1)
	};

	Debug_StageMoveDebug(&lake_dst, 6, fx, fy);
	Stage_DrawTex(&this->tex_back0, &lake_src, &lake_dst, stage.camera.bzoom);
}

void Back_Flipside_Free(StageBack *back)
{
	Back_Flipside *this = (Back_Flipside*)back;
	

	//Free structure
	Mem_Free(this);
}

StageBack *Back_Flipside_New(void)
{
	//Allocate background structure
	Back_Flipside *this = (Back_Flipside*)Mem_Alloc(sizeof(Back_Flipside));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Flipside_DrawBG;
	this->back.free = Back_Flipside_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\FLIPSIDE\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
