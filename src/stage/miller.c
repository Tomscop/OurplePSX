/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "miller.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Miller background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	IO_Data arc_doggo, arc_doggo_ptr[1];
	
	Gfx_Tex tex_back; //millerbg
	
	//Doggo state
	Gfx_Tex tex_doggo;
	u8 doggo_frame, doggo_tex_id;

	Animatable doggo_animatable;
	
} Back_Miller;

//Doggo animation and rects
static const CharFrame doggo_frame[] = {
	{0, {  0,  0,102, 73}, {  0,  0}}, //0 idle 1
	{0, {  0,  0,102, 73}, {  0, -1}}, //1 idle 2
	{0, {  0,  0,102, 73}, {  0, -2}}, //2 idle 3
	{0, {  0,  0,102, 73}, {  0, -3}}, //3 idle 4
	{0, {  0,  0,102, 73}, {  0, -4}}, //4 idle 5
	{0, {  0,  0,102, 73}, {  0, -5}}, //5 idle 6
	{0, {  0,  0,102, 73}, {  0, -6}}, //6 idle 7
	{0, {  0,  0,102, 73}, {  0, -7}}, //7 idle 8
	{0, {  0,  0,102, 73}, {  0, -8}}, //8 idle 9
	{0, {  0,  0,102, 73}, {  0, -9}}, //9 idle 10
	{0, {  0,  0,102, 73}, {  0,-10}}, //10 idle 11
	{0, {  0,  0,102, 73}, {  0,-11}}, //11 idle 12
	{0, {  0,  0,102, 73}, {  0,-12}}, //12 idle 13
	{0, {  0,  0,102, 73}, {  0,-13}}, //13 idle 14
	{0, {  0,  0,102, 73}, {  0,-14}}, //14 idle 15
	{0, {  0,  0,102, 73}, {  0,-15}}, //15 idle 16
	{0, {103,  0,105, 59}, { -3,-27}}, //16 dead
};

static const Animation doggo_anim[] = {
	{1, (const u8[]){1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, ASCR_BACK, 1}}, //Idle 1
	{1, (const u8[]){14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, ASCR_BACK, 1}}, //Idle 2
	{2, (const u8[]){16, ASCR_BACK, 1}}, //Dead
};

//Doggo functions
void Miller_Doggo_SetFrame(void *user, u8 frame)
{
	Back_Miller *this = (Back_Miller*)user;
	
	//Check if this is a new frame
	if (frame != this->doggo_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &doggo_frame[this->doggo_frame = frame];
		if (cframe->tex != this->doggo_tex_id)
			Gfx_LoadTex(&this->tex_doggo, this->arc_doggo_ptr[this->doggo_tex_id = cframe->tex], 0);
	}
}

void Miller_Doggo_Draw(Back_Miller *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &doggo_frame[this->doggo_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy, (src.w / 2) << FIXED_SHIFT , (src.h / 2) << FIXED_SHIFT };
	Debug_StageMoveDebug(&dst, 7, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_doggo, &src, &dst, stage.camera.bzoom);
}

void Back_Miller_DrawBG(StageBack *back)
{
	Back_Miller *this = (Back_Miller*)back;
	
	fixed_t fx, fy;

	//Animate and draw doggo
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	if ((stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step & 0xF)== 0) && (stage.song_step <= 2975))
		Animatable_SetAnim(&this->doggo_animatable, 0);
	if ((stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step & 0xF)== 7) && (stage.song_step <= 2975))
		Animatable_SetAnim(&this->doggo_animatable, 1);
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 2976))
		Animatable_SetAnim(&this->doggo_animatable, 2);
	if ((stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step & 0xF)== 0) && (stage.song_step >= 4032))
		Animatable_SetAnim(&this->doggo_animatable, 0);
	if ((stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step & 0xF)== 7) && (stage.song_step >= 4032))
		Animatable_SetAnim(&this->doggo_animatable, 1);
	
	Animatable_Animate(&this->doggo_animatable, (void*)this, Miller_Doggo_SetFrame);
	
	Miller_Doggo_Draw(this, FIXED_DEC(268,1) - fx, FIXED_DEC(111,1) - fy);
	
	//Draw millerbg
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT millerbg_src = {0, 0, 255, 255};
	RECT_FIXED millerbg_dst = {
		FIXED_DEC(0 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(0,1) - fy,
		FIXED_DEC(392 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(305,1)
	};

	Debug_StageMoveDebug(&millerbg_dst, 6, fx, fy);
	Stage_DrawTex(&this->tex_back, &millerbg_src, &millerbg_dst, stage.camera.bzoom);
	
	//Draw blackf
	RECT screen_src = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
	Gfx_DrawRect(&screen_src, 0, 0, 0);
}

void Back_Miller_Free(StageBack *back)
{
	Back_Miller *this = (Back_Miller*)back;
	
	//Free doggo archive
	Mem_Free(this->arc_doggo);

	//Free structure
	Mem_Free(this);
}

StageBack *Back_Miller_New(void)
{
	//Allocate background structure
	Back_Miller *this = (Back_Miller*)Mem_Alloc(sizeof(Back_Miller));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Miller_DrawBG;
	this->back.free = Back_Miller_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\MILLER\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back, Archive_Find(arc_back, "back.tim"), 0);
	Mem_Free(arc_back);
	
	//Load doggo textures
	this->arc_doggo = IO_Read("\\MILLER\\DOGGO.ARC;1");
	this->arc_doggo_ptr[0] = Archive_Find(this->arc_doggo, "doggo.tim");
	
	//Initialize doggo state
	Animatable_Init(&this->doggo_animatable, doggo_anim);
	Animatable_SetAnim(&this->doggo_animatable, 0);
	this->doggo_frame = this->doggo_tex_id = 0xFF; //Force art load
	
	return (StageBack*)this;
}
