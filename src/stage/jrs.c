/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "jrs.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Jrs background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	IO_Data arc_green, arc_green_ptr[1];
	IO_Data arc_rain, arc_rain_ptr[3];
	
	Gfx_Tex tex_back0; //jrsbg1
	Gfx_Tex tex_back1; //jrsbg2

	//Green state
	Gfx_Tex tex_green;
	u8 green_frame, green_tex_id;

	Animatable green_animatable;
	
	//Rain state
	Gfx_Tex tex_rain;
	u8 rain_frame, rain_tex_id;

	Animatable rain_animatable;
} Back_Jrs;

//Green animation and rects
static const CharFrame green_frame[] = {
	{0, {  0,  0, 51, 90}, { 71, 98}}, //0 idle 1
	{0, { 52,  0, 51, 90}, { 71, 98}}, //1 idle 2
	{0, {  0, 91, 51, 90}, { 71, 98}}, //2 idle 3
	{0, { 52, 91, 51, 90}, { 71, 98}}, //3 idle 4
};

static const Animation green_anim[] = {
	{2, (const u8[]){0, 1, 2, 3, ASCR_BACK, 1}}, //Idle
};

//Rain animation and rects
static const CharFrame rain_frame[] = {
	{0, {  0,  0,255,255}, { 71, 98}}, //0 rain 1
	{1, {  0,  0,255,255}, { 71, 98}}, //1 rain 2
	{2, {  0,  0,255,255}, { 71, 98}}, //2 rain 3
};

static const Animation rain_anim[] = {
	{2, (const u8[]){0, 1, 2, ASCR_CHGANI, 0}}, //Rain
};

//Green functions
void Jrs_Green_SetFrame(void *user, u8 frame)
{
	Back_Jrs *this = (Back_Jrs*)user;
	
	//Check if this is a new frame
	if (frame != this->green_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &green_frame[this->green_frame = frame];
		if (cframe->tex != this->green_tex_id)
			Gfx_LoadTex(&this->tex_green, this->arc_green_ptr[this->green_tex_id = cframe->tex], 0);
	}
}

void Jrs_Green_Draw(Back_Jrs *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &green_frame[this->green_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy, src.w << FIXED_SHIFT ,src.h<< FIXED_SHIFT };
	Debug_StageMoveDebug(&dst, 7, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_green, &src, &dst, stage.camera.bzoom);
}

//Rain functions
void Jrs_Rain_SetFrame(void *user, u8 frame)
{
	Back_Jrs *this = (Back_Jrs*)user;
	
	//Check if this is a new frame
	if (frame != this->rain_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &rain_frame[this->rain_frame = frame];
		if (cframe->tex != this->rain_tex_id)
			Gfx_LoadTex(&this->tex_rain, this->arc_rain_ptr[this->rain_tex_id = cframe->tex], 0);
	}
}

void Jrs_Rain_Draw(Back_Jrs *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &rain_frame[this->rain_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = {ox, oy,576 << FIXED_SHIFT,454 << FIXED_SHIFT};
	Debug_StageMoveDebug(&dst, 6, stage.camera.x, stage.camera.y);
	Stage_BlendTex(&this->tex_rain, &src, &dst, stage.camera.bzoom, 1);
}

void Back_Jrs_DrawFG(StageBack *back)
{
	Back_Jrs *this = (Back_Jrs*)back;
	
	fixed_t fx, fy;
	
	//Animate and draw rain
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step & 0x3 )== 0)
		Animatable_SetAnim(&this->rain_animatable, 0);
		
	Animatable_Animate(&this->rain_animatable, (void*)this, Jrs_Rain_SetFrame);
	
	Jrs_Rain_Draw(this, FIXED_DEC(-152 + 71,1) - fx, FIXED_DEC(-71 + 98,1) - fy);
}

void Back_Jrs_DrawBG(StageBack *back)
{
	Back_Jrs *this = (Back_Jrs*)back;
	
	fixed_t fx, fy;
	
	//Animate and draw green
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step & 0x7 )== 0)
		Animatable_SetAnim(&this->green_animatable, 0);
		
	Animatable_Animate(&this->green_animatable, (void*)this, Jrs_Green_SetFrame);
	
	Jrs_Green_Draw(this, FIXED_DEC(192 + 71,1) - fx, FIXED_DEC(7 + 98,1) - fy);
	
	//Draw jrsbg
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT jrsbg1_src = {0, 0,213,178};
	RECT_FIXED jrsbg1_dst = {
		FIXED_DEC(-151 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-31,1) - fy,
		FIXED_DEC(323 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(270,1)
	};
	
	RECT jrsbg2_src = {0, 0,212,178};
	RECT_FIXED jrsbg2_dst = {
		FIXED_DEC(171 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-31,1) - fy,
		FIXED_DEC(322 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(270,1)
	};
	
	Debug_StageMoveDebug(&jrsbg1_dst, 8, fx, fy);
	Debug_StageMoveDebug(&jrsbg2_dst, 9, fx, fy);
	Stage_DrawTex(&this->tex_back0, &jrsbg1_src, &jrsbg1_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &jrsbg2_src, &jrsbg2_dst, stage.camera.bzoom);
}

void Back_Jrs_Free(StageBack *back)
{
	Back_Jrs *this = (Back_Jrs*)back;
	
	//Free green archive
	Mem_Free(this->arc_green);
	
	//Free rain archive
	Mem_Free(this->arc_rain);

	//Free structure
	Mem_Free(this);
}

StageBack *Back_Jrs_New(void)
{
	//Allocate background structure
	Back_Jrs *this = (Back_Jrs*)Mem_Alloc(sizeof(Back_Jrs));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_Jrs_DrawFG;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Jrs_DrawBG;
	this->back.free = Back_Jrs_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\JRS\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Mem_Free(arc_back);
	
	//Load green textures
	this->arc_green = IO_Read("\\JRS\\GREEN.ARC;1");
	this->arc_green_ptr[0] = Archive_Find(this->arc_green, "green.tim");
	
	//Load rain textures
	this->arc_rain = IO_Read("\\JRS\\RAIN.ARC;1");
	this->arc_rain_ptr[0] = Archive_Find(this->arc_rain, "rain0.tim");
	this->arc_rain_ptr[1] = Archive_Find(this->arc_rain, "rain1.tim");
	this->arc_rain_ptr[2] = Archive_Find(this->arc_rain, "rain2.tim");
	
	//Initialize green state
	Animatable_Init(&this->green_animatable, green_anim);
	Animatable_SetAnim(&this->green_animatable, 0);
	this->green_frame = this->green_tex_id = 0xFF; //Force art load
	
	//Initialize rain state
	Animatable_Init(&this->rain_animatable, rain_anim);
	Animatable_SetAnim(&this->rain_animatable, 0);
	this->rain_frame = this->rain_tex_id = 0xFF; //Force art load
	
	return (StageBack*)this;
}
