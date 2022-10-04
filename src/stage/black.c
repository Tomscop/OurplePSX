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
	Gfx_Tex tex_back1; //Oval
	Gfx_Tex tex_back0; //Blackbg

} Back_Black;

void Back_Black_DrawFG(StageBack *back)
{
	if (stage.stage_id == StageId_1_4)
	{
		if ((stage.song_step >= 124 && stage.song_step <= 128) || (stage.song_step >= 380 && stage.song_step <= 384) || (stage.song_step >= 828 && stage.song_step <= 832) || (stage.song_step >= 1020))
		{
			Back_Black *this = (Back_Black*)back;
		
			fixed_t fx, fy;
		
			//Draw blackf
			fx = stage.camera.x;
			fy = stage.camera.y;
	
			RECT blackf_src = {0, 0, 255, 255};
			RECT_FIXED blackf_dst = {
				FIXED_DEC(-265 - screen.SCREEN_WIDEOADD2,1) - fx,
				FIXED_DEC(-145,1) - fy,
				FIXED_DEC(600 + screen.SCREEN_WIDEOADD,1),
				FIXED_DEC(450,1)
			};
		
			Debug_StageMoveDebug(&blackf_dst, 9, fx, fy);
			Stage_DrawTex(&this->tex_back0, &blackf_src, &blackf_dst, stage.camera.bzoom);
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
	
	RECT oval_src = {0, 0,116, 28};
	RECT_FIXED oval_dst = {
		FIXED_DEC(-196 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-38,1) - fy,
		FIXED_DEC(160 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(37,1)
	};
	
	RECT blackbg_src = {0, 0, 256, 256};
	RECT_FIXED blackbg_dst = {
		FIXED_DEC(-265 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-145,1) - fy,
		FIXED_DEC(400 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(400,1)
	};
	if ((stage.stage_id == StageId_6_2) || (stage.stage_id == StageId_1_4))
	{
		blackbg_dst.w = FIXED_DEC(600 + screen.SCREEN_WIDEOADD,1);
		blackbg_dst.h = FIXED_DEC(450,1);
	}
	if (stage.stage_id == StageId_4_2)
	{
		blackbg_dst.w = FIXED_DEC(300,1);
		blackbg_dst.h = FIXED_DEC(270,1);
	}
	
	Debug_StageMoveDebug(&blackbg_dst, 8, fx, fy);
	Debug_StageMoveDebug(&oval_dst, 9, fx, fy);
	Stage_DrawTex(&this->tex_back1, &oval_src, &oval_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back0, &blackbg_src, &blackbg_dst, stage.camera.bzoom);
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
	
	if (stage.stage_id == StageId_4_2)
	{
		//Set background functions
		this->back.draw_fg = NULL;
		this->back.draw_md = NULL;
		this->back.draw_bg = Back_Black_DrawBG;
		this->back.free = Back_Black_Free;
		
		//Load background textures
		IO_Data arc_back = IO_Read("\\BLACK\\BACK.ARC;1");
		Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
		Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
		Mem_Free(arc_back);
	}
	else
	{
		//Set background functions
		this->back.draw_fg = Back_Black_DrawFG;
		this->back.draw_md = NULL;
		this->back.draw_bg = Back_Black_DrawBG;
		this->back.free = Back_Black_Free;
		
		//Load background textures
		IO_Data arc_back = IO_Read("\\BLACK\\BACK.ARC;1");
		Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
		Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back0.tim"), 0);
		Mem_Free(arc_back);
	}
	
	return (StageBack*)this;
}
