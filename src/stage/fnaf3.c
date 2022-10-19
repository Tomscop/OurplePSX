/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "fnaf3.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//FNAF 3 background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //fnaf3bg

} Back_FNAF3;


void Back_FNAF3_DrawBG(StageBack *back)
{
	Back_FNAF3 *this = (Back_FNAF3*)back;
	
	fixed_t fx, fy;

	//Draw fnaf3bg
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT fnaf3bg_src = {0, 0, 255, 255};
	RECT_FIXED fnaf3bg_dst = {
		FIXED_DEC(-212 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-164,1) - fy,
		FIXED_DEC(525 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(246,1)
	};

	Debug_StageMoveDebug(&fnaf3bg_dst, 8, fx, fy);
	Stage_DrawTex(&this->tex_back0, &fnaf3bg_src, &fnaf3bg_dst, stage.camera.bzoom);
}

void Back_FNAF3_Free(StageBack *back)
{
	Back_FNAF3 *this = (Back_FNAF3*)back;
	

	//Free structure
	Mem_Free(this);
}

StageBack *Back_FNAF3_New(void)
{
	//Allocate background structure
	Back_FNAF3 *this = (Back_FNAF3*)Mem_Alloc(sizeof(Back_FNAF3));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_FNAF3_DrawBG;
	this->back.free = Back_FNAF3_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\FNAF3\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
