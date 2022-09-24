/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "officeb.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//OfficeB background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //officeb1
	Gfx_Tex tex_back1; //officeb2
	Gfx_Tex tex_back2; //black

} Back_OfficeB;


void Back_OfficeB_DrawFG(StageBack *back)
{
	if (stage.song_step >= 888 && stage.song_step <= 895 || stage.song_step >= 1336 && stage.song_step <= 1344 || stage.song_step >= 1400 && stage.song_step <= 1408 || stage.song_step >= 2168 && stage.song_step <= 2176 || stage.song_step >= 2432)
	{
		Back_OfficeB *this = (Back_OfficeB*)back;
	
	
	
		fixed_t fx, fy;
	
	
	
		//Draw black
		fx = stage.camera.x;
		fy = stage.camera.y;
	
		RECT black_src = {0, 0, 255, 255};
		RECT_FIXED black_dst = {
			FIXED_DEC(-165 - screen.SCREEN_WIDEOADD2,1) - fx,
			FIXED_DEC(-140,1) - fy,
			FIXED_DEC(500 + screen.SCREEN_WIDEOADD,1),
			FIXED_DEC(400,1)
		};
	
		Debug_StageMoveDebug(&black_dst, 10, fx, fy);
		Stage_DrawTex(&this->tex_back2, &black_src, &black_dst, stage.camera.bzoom);
	}
}

void Back_OfficeB_DrawBG(StageBack *back)
{
	Back_OfficeB *this = (Back_OfficeB*)back;
	
	
	
	fixed_t fx, fy;
	
	
	
	//Draw officeb
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT officeb1_src = {0, 0, 249, 255};
	RECT_FIXED officeb1_dst = {
		FIXED_DEC(-165 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-33,1) - fy,
		FIXED_DEC(151 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(155,1)
	};
	
	RECT officeb2_src = {0, 0, 249, 255};
	RECT_FIXED officeb2_dst = {
		FIXED_DEC(-14 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-33,1) - fy,
		FIXED_DEC(151 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(155,1)
	};
	
	Debug_StageMoveDebug(&officeb1_dst, 8, fx, fy);
	Debug_StageMoveDebug(&officeb2_dst, 9, fx, fy);
	Stage_DrawTex(&this->tex_back0, &officeb1_src, &officeb1_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &officeb2_src, &officeb2_dst, stage.camera.bzoom);
}

void Back_OfficeB_Free(StageBack *back)
{
	Back_OfficeB *this = (Back_OfficeB*)back;
	

	//Free structure
	Mem_Free(this);
}

StageBack *Back_OfficeB_New(void)
{
	//Allocate background structure
	Back_OfficeB *this = (Back_OfficeB*)Mem_Alloc(sizeof(Back_OfficeB));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_OfficeB_DrawFG;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_OfficeB_DrawBG;
	this->back.free = Back_OfficeB_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\OFFICEB\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
