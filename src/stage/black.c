/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "black.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Black background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //Oval

} Back_Black;

void Back_Black_DrawFG(StageBack *back)
{
	Back_Black *this = (Back_Black*)back;
		
	if (stage.stage_id == StageId_1_4)
	{
		if ((stage.song_step >= 124 && stage.song_step <= 128) || (stage.song_step >= 380 && stage.song_step <= 384) || (stage.song_step >= 828 && stage.song_step <= 832) || (stage.song_step >= 1020))
		{
			//Draw blackf
			RECT screen_src = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
			Gfx_DrawRect(&screen_src, 0, 0, 0);
		}
	}
}

void Back_Black_DrawBG(StageBack *back)
{
	Back_Black *this = (Back_Black*)back;

	fixed_t fx, fy;

	//Draw blackbg
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	if (stage.stage_id == StageId_4_2)
	{
		RECT oval_src = {0, 0,116, 28};
		RECT_FIXED oval_dst = {
			FIXED_DEC(-196 - screen.SCREEN_WIDEOADD2,1) - fx,
			FIXED_DEC(-38,1) - fy,
			FIXED_DEC(160 + screen.SCREEN_WIDEOADD,1),
			FIXED_DEC(37,1)
		};

		Debug_StageMoveDebug(&oval_dst, 9, fx, fy);
		Stage_DrawTex(&this->tex_back0, &oval_src, &oval_dst, stage.camera.bzoom);
  }
	
	//Draw blackf
	RECT screen_src = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
	Gfx_DrawRect(&screen_src, 0, 0, 0);
}

void Back_Black_Free(StageBack *back)
{
	Back_Black *this = (Back_Black*)back;
	

	//Free structure
	Mem_Free(this);
}

StageBack *Back_Black_New(void)
{
	//Allocate background structure
	Back_Black *this = (Back_Black*)Mem_Alloc(sizeof(Back_Black));
	if (this == NULL)
		return NULL;
	
		//Set background functions
		this->back.draw_fg = Back_Black_DrawFG;
		this->back.draw_md = NULL;
		this->back.draw_bg = Back_Black_DrawBG;
		this->back.free = Back_Black_Free;
	
	if (stage.stage_id == StageId_4_2)
	{
		//Load background textures
		IO_Data arc_back = IO_Read("\\BLACK\\BACK.ARC;1");
		Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0); //oval
		Mem_Free(arc_back);
	}

	Gfx_SetClear(0, 0, 0);
	
	return (StageBack*)this;
}
