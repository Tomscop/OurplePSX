/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "miller.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Miller background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back; //millerbg

} Back_Miller;


void Back_Miller_DrawBG(StageBack *back)
{
	Back_Miller *this = (Back_Miller*)back;
	
	fixed_t fx, fy;

	//Draw millerbg
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT millerbg_src = {0, 0, 255, 255};
	RECT_FIXED millerbg_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(392 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(305,1)
	};

	Debug_StageMoveDebug(&millerbg_dst, 6, fx, fy);
	Stage_DrawTex(&this->tex_back, &millerbg_src, &millerbg_dst, stage.camera.bzoom);
	
	//Draw blackf
	RECT screen_src = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
	Gfx_DrawRect(&screen_src, 0, 0, 0);
}

void Back_Miller_Free(StageBack *back)
{
	Back_Miller *this = (Back_Miller*)back;
	

	//Free structure
	Mem_Free(this);
}

StageBack *Back_Miller_New(void)
{
	//Allocate background structure
	Back_Miller *this = (Back_Miller*)Mem_Alloc(sizeof(Back_Miller));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Miller_DrawBG;
	this->back.free = Back_Miller_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\MILLER\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back, Archive_Find(arc_back, "back.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
