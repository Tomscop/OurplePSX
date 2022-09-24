/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "storage.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Storage background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //storagebg1
	Gfx_Tex tex_back1; //storagebg2

} Back_Storage;


void Back_Storage_DrawBG(StageBack *back)
{
	Back_Storage *this = (Back_Storage*)back;
	
	
	
	fixed_t fx, fy;
	
	
	
	//Draw storagebg
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT storagebg1_src = {0, 0, 234,182};
	RECT_FIXED storagebg1_dst = {
		FIXED_DEC(-151 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-31,1) - fy,
		FIXED_DEC(404 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(314,1)
	};
	
	RECT storagebg2_src = {0, 0, 111, 182};
	RECT_FIXED storagebg2_dst = {
		FIXED_DEC(253 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-31,1) - fy,
		FIXED_DEC(192 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(314,1)
	};
	
	Debug_StageMoveDebug(&storagebg1_dst, 8, fx, fy);
	Debug_StageMoveDebug(&storagebg2_dst, 9, fx, fy);
	Stage_DrawTex(&this->tex_back0, &storagebg1_src, &storagebg1_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &storagebg2_src, &storagebg2_dst, stage.camera.bzoom);
}

void Back_Storage_Free(StageBack *back)
{
	Back_Storage *this = (Back_Storage*)back;
	

	//Free structure
	Mem_Free(this);
}

StageBack *Back_Storage_New(void)
{
	//Allocate background structure
	Back_Storage *this = (Back_Storage*)Mem_Alloc(sizeof(Back_Storage));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Storage_DrawBG;
	this->back.free = Back_Storage_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\STORAGE\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
