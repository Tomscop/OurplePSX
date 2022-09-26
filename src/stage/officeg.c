/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "officeg.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//OfficeG background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //officeg1
	Gfx_Tex tex_back1; //officeg2
	Gfx_Tex tex_back2; //black

} Back_OfficeG;


void Back_OfficeG_DrawFG(StageBack *back)
{
	if ((stage.song_step >= 888 && stage.song_step <= 895) || (stage.song_step >= 1336 && stage.song_step <= 1344) || (stage.song_step >= 1400 && stage.song_step <= 1408) || (stage.song_step >= 2168 && stage.song_step <= 2176) || (stage.song_step >= 2432))
	{
		Back_OfficeG *this = (Back_OfficeG*)back;
		
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

void Back_OfficeG_DrawBG(StageBack *back)
{
	Back_OfficeG *this = (Back_OfficeG*)back;

	fixed_t fx, fy;

	//Draw officeg
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT officeg1_src = {0, 0, 226, 255};
	RECT_FIXED officeg1_dst = {
		FIXED_DEC(-165 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-140,1) - fy,
		FIXED_DEC(233 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(263,1)
	};
	
	RECT officeg2_src = {0, 0, 225, 255};
	RECT_FIXED officeg2_dst = {
		FIXED_DEC(68 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-140,1) - fy,
		FIXED_DEC(232 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(263,1)
	};
	
	Debug_StageMoveDebug(&officeg1_dst, 8, fx, fy);
	Debug_StageMoveDebug(&officeg2_dst, 9, fx, fy);
	Stage_DrawTex(&this->tex_back0, &officeg1_src, &officeg1_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &officeg2_src, &officeg2_dst, stage.camera.bzoom);
}

void Back_OfficeG_Free(StageBack *back)
{
	Back_OfficeG *this = (Back_OfficeG*)back;
	

	//Free structure
	Mem_Free(this);
}

StageBack *Back_OfficeG_New(void)
{
	//Allocate background structure
	Back_OfficeG *this = (Back_OfficeG*)Mem_Alloc(sizeof(Back_OfficeG));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_OfficeG_DrawFG;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_OfficeG_DrawBG;
	this->back.free = Back_OfficeG_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\OFFICEG\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
