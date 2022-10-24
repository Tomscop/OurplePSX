/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "ngroom.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//NGRoom background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //ngroombg1
	Gfx_Tex tex_back1; //ngroombg2
	Gfx_Tex tex_nfg; //nfg

} Back_NGRoom;

void Back_NGRoom_DrawFG(StageBack *back)
{
	Back_NGRoom *this = (Back_NGRoom*)back;
	
	fixed_t fx, fy;
	
	//Draw nfg
	fx = stage.camera.x * 2;
	fy = stage.camera.y * 2;
	
	RECT nfg_src = {0, 0,255,109};
	RECT_FIXED nfg_dst = {
		FIXED_DEC(-125 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(182,1) - fy,
		FIXED_DEC(512 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(109,1)
	};
	
	Debug_StageMoveDebug(&nfg_dst, 7, fx, fy);
	Stage_DrawTex(&this->tex_nfg, &nfg_src, &nfg_dst, stage.camera.bzoom);
}

void Back_NGRoom_DrawBG(StageBack *back)
{
	Back_NGRoom *this = (Back_NGRoom*)back;
	
	fixed_t fx, fy;
	
	//Draw ngroombg
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT ngroombg1_src = {0, 0,255,255};
	RECT_FIXED ngroombg1_dst = {
		FIXED_DEC(-151 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-20,1) - fy,
		FIXED_DEC(242 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(198,1)
	};
	
	RECT ngroombg2_src = {0, 0,255,255};
	RECT_FIXED ngroombg2_dst = {
		FIXED_DEC(90 + screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-20,1) - fy,
		FIXED_DEC(241 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(198,1)
	};
	
	Debug_StageMoveDebug(&ngroombg1_dst, 8, fx, fy);
	Debug_StageMoveDebug(&ngroombg2_dst, 9, fx, fy);
	Stage_DrawTex(&this->tex_back0, &ngroombg1_src, &ngroombg1_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &ngroombg2_src, &ngroombg2_dst, stage.camera.bzoom);
}

void Back_NGRoom_Free(StageBack *back)
{
	Back_NGRoom *this = (Back_NGRoom*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_NGRoom_New(void)
{
	//Allocate background structure
	Back_NGRoom *this = (Back_NGRoom*)Mem_Alloc(sizeof(Back_NGRoom));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_NGRoom_DrawFG;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_NGRoom_DrawBG;
	this->back.free = Back_NGRoom_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\NGROOM\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_nfg, Archive_Find(arc_back, "nfg.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
