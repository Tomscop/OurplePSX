/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "deskroom.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Deskroom background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //deskroombg
	Gfx_Tex tex_back1; //black

} Back_Deskroom;


void Back_Deskroom_DrawBG(StageBack *back)
{
	if (stage.song_step >= 270 && stage.song_step <= 911 || stage.song_step >= 1167 && stage.song_step <= 1423 || stage.song_step >= 2193)
	{
		Back_Deskroom *this = (Back_Deskroom*)back;
	
	
	
		fixed_t fx, fy;
	
	
	
		//Draw deskroombg
		fx = stage.camera.x;
		fy = stage.camera.y;
	
		RECT deskroombg_src = {0, 0, 255, 255};
		RECT_FIXED deskroombg_dst = {
			FIXED_DEC(-235 - screen.SCREEN_WIDEOADD2,1) - fx,
			FIXED_DEC(-205,1) - fy,
			FIXED_DEC(547 + screen.SCREEN_WIDEOADD,1),
			FIXED_DEC(326,1)
		};
	
		RECT black_src = {0, 0, 255, 255};
		RECT_FIXED black_dst = {
			FIXED_DEC(-195 - screen.SCREEN_WIDEOADD2,1) - fx,
			FIXED_DEC(-146,1) - fy,
			FIXED_DEC(341 + screen.SCREEN_WIDEOADD,1),
			FIXED_DEC(258,1)
		};
	
		Debug_StageMoveDebug(&deskroombg_dst, 8, fx, fy);
		Debug_StageMoveDebug(&black_dst, 9, fx, fy);
		Stage_DrawTex(&this->tex_back1, &black_src, &black_dst, stage.camera.bzoom);
		Stage_DrawTex(&this->tex_back0, &deskroombg_src, &deskroombg_dst, stage.camera.bzoom);
	}
	else
	{
		Back_Deskroom *this = (Back_Deskroom*)back;
	
	
	
		fixed_t fx, fy;
	
	
	
		//Draw deskroombg
		fx = stage.camera.x;
		fy = stage.camera.y;
	
		RECT deskroombg_src = {0, 0, 255, 255};
		RECT_FIXED deskroombg_dst = {
			FIXED_DEC(-235 - screen.SCREEN_WIDEOADD2,1) - fx,
			FIXED_DEC(-205,1) - fy,
			FIXED_DEC(547 + screen.SCREEN_WIDEOADD,1),
			FIXED_DEC(326,1)
		};

		Debug_StageMoveDebug(&deskroombg_dst, 8, fx, fy);
		Stage_DrawTex(&this->tex_back0, &deskroombg_src, &deskroombg_dst, stage.camera.bzoom);
	}
}

void Back_Deskroom_Free(StageBack *back)
{
	Back_Deskroom *this = (Back_Deskroom*)back;
	

	//Free structure
	Mem_Free(this);
}

StageBack *Back_Deskroom_New(void)
{
	//Allocate background structure
	Back_Deskroom *this = (Back_Deskroom*)Mem_Alloc(sizeof(Back_Deskroom));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Deskroom_DrawBG;
	this->back.free = Back_Deskroom_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\DESKROOM\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
