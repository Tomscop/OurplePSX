/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "takecake.h"

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
	IO_Data arc_kid, arc_kid_ptr[1];
	
	Gfx_Tex tex_back0; //takecakebg
	
	//Kid state
	Gfx_Tex tex_kid;
	u8 kid_frame, kid_tex_id;

	Animatable kid_animatable;

} Back_TakeCake;

//Kid animation and rects
static const CharFrame kid_frame[] = {
	{0, {  0,  0, 86, 79}, { 71, 98}}, //0 idle 1
	{0, { 87,  0, 86, 79}, { 71, 98}}, //1 idle 2
	{0, {  0, 80, 86, 79}, { 71, 98}}, //2 idle 3
	{0, { 87, 80, 86, 79}, { 71, 98}}, //3 idle 4
	{0, {  0,160, 86, 79}, { 71, 98}}, //4 idle 5
	{0, { 87,160, 86, 79}, { 71, 98}}, //5 idle 6
};

static const Animation kid_anim[] = {
	{2, (const u8[]){0, 1, 2, ASCR_BACK, 1}}, //Idle 1
	{2, (const u8[]){3, 4, 5, ASCR_BACK, 1}}, //Idle 2
};

//Kid functions
void TakeCake_Kid_SetFrame(void *user, u8 frame)
{
	Back_TakeCake *this = (Back_TakeCake*)user;
	
	//Check if this is a new frame
	if (frame != this->kid_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &kid_frame[this->kid_frame = frame];
		if (cframe->tex != this->kid_tex_id)
			Gfx_LoadTex(&this->tex_kid, this->arc_kid_ptr[this->kid_tex_id = cframe->tex], 0);
	}
}

void TakeCake_Kid_Draw(Back_TakeCake *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &kid_frame[this->kid_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy,63 << FIXED_SHIFT ,58 << FIXED_SHIFT };
	Debug_StageMoveDebug(&dst, 7, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_kid, &src, &dst, stage.camera.bzoom);
}

void Back_TakeCake_DrawBG(StageBack *back)
{
	Back_TakeCake *this = (Back_TakeCake*)back;
	
	fixed_t fx, fy;

	//Animate and draw kid
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	if (stage.flag & STAGE_FLAG_JUST_STEP && ((stage.song_beat % 2) == 0))
		Animatable_SetAnim(&this->kid_animatable, 0);
	
	if (stage.flag & STAGE_FLAG_JUST_STEP && ((stage.song_beat % 2) == 1))
		Animatable_SetAnim(&this->kid_animatable, 1);
	
	Animatable_Animate(&this->kid_animatable, (void*)this, TakeCake_Kid_SetFrame);
	
	TakeCake_Kid_Draw(this, FIXED_DEC(163 + 71,1) - fx, FIXED_DEC(32 + 98,1) - fy);
	TakeCake_Kid_Draw(this, FIXED_DEC(0 + 71,1) - fx, FIXED_DEC(0 + 98,1) - fy);
	TakeCake_Kid_Draw(this, FIXED_DEC(0 + 71,1) - fx, FIXED_DEC(0 + 98,1) - fy);
	TakeCake_Kid_Draw(this, FIXED_DEC(0 + 71,1) - fx, FIXED_DEC(0 + 98,1) - fy);
	
	//Draw takecakebg
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT takecakebg_src = {0, 0, 255, 255};
	RECT_FIXED takecakebg_dst = {
		FIXED_DEC(-212 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-164,1) - fy,
		FIXED_DEC(668 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(387,1)
	};

	Debug_StageMoveDebug(&takecakebg_dst, 6, fx, fy);
	Stage_DrawTex(&this->tex_back0, &takecakebg_src, &takecakebg_dst, stage.camera.bzoom);
}

void Back_TakeCake_Free(StageBack *back)
{
	Back_TakeCake *this = (Back_TakeCake*)back;
	
	//Free kid archive
	Mem_Free(this->arc_kid);
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_TakeCake_New(void)
{
	//Allocate background structure
	Back_TakeCake *this = (Back_TakeCake*)Mem_Alloc(sizeof(Back_TakeCake));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_TakeCake_DrawBG;
	this->back.free = Back_TakeCake_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\TAKECAKE\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Mem_Free(arc_back);
	
	//Load kid textures
	this->arc_kid = IO_Read("\\TAKECAKE\\KID.ARC;1");
	this->arc_kid_ptr[0] = Archive_Find(this->arc_kid, "kid.tim");
	
	//Initialize kid state
	Animatable_Init(&this->kid_animatable, kid_anim);
	Animatable_SetAnim(&this->kid_animatable, 0);
	this->kid_frame = this->kid_tex_id = 0xFF; //Force art load
	
	return (StageBack*)this;
}
