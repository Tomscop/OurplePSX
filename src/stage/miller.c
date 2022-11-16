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
	IO_Data arc_fback, arc_fback_ptr[12];
	
	Gfx_Tex tex_back; //millerbg
	Gfx_Tex tex_spite; //spite
	
	//Doggo state
	Gfx_Tex tex_doggo;
	u8 doggo_frame, doggo_tex_id;

	Animatable doggo_animatable;
	
	//Fback state
	Gfx_Tex tex_fback;
	u8 fback_frame, fback_tex_id;

	Animatable fback_animatable;
	
	//fade stuff
	fixed_t fade, fadespd;
	fixed_t fade2, fadespd2;
	
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

//Fback animation and rects
static const CharFrame fback_frame[] = {
	{0, {  0,  0,125, 94}, {  0,  0}}, //0 vagas
	{0, {126,  0,125, 94}, {  0,  0}}, //1 news
	{0, {  0, 95,125, 94}, {  0,  0}}, //2 partners
	{0, {126, 95,125, 94}, {  0,  0}}, //3 door
	{1, {  0,  0,125, 94}, {  0,  0}}, //4 fire
	{1, {126,  0,125, 94}, {  0,  0}}, //5 sad
	{1, {  0, 95,125, 94}, {  0,  0}}, //6 GRAND CANYON
	{1, {126, 95,125, 94}, {  0,  0}}, //7 bird
	{2, {  0,  0,125, 94}, {  0,  0}}, //8 doom
	{2, {126,  0,125, 94}, {  0,  0}}, //9 connect four
	{2, {  0, 95,125, 94}, {  0,  0}}, //10 old
	{2, {126, 95,125, 94}, {  0,  0}}, //11 yippee
	{3, {  0,  0,192, 83}, {  0,  0}}, //12 hint
	{4, {  0,  0, 81,121}, {  0,  0}}, //13 open 1
	{4, { 82,  0, 81,121}, {  0,  0}}, //14 open 2
	{4, {164,  0, 81,121}, {  0,  0}}, //15 open 3
	{4, {  0,122, 81,121}, {  0,  0}}, //16 open 4
	{4, { 82,122, 81,121}, {  0,  0}}, //17 open 5
	{4, {164,122, 81,121}, {  0,  0}}, //18 open 6
	{5, {  0,  0, 81,121}, {  0,  0}}, //19 open 7
	{5, { 82,  0, 81,121}, {  0,  0}}, //20 open 8
	{5, {164,  0, 81,121}, {  0,  0}}, //21 open 9
	{5, {  0,122, 81,121}, {  0,  0}}, //22 open 10
	{5, { 82,122, 81,121}, {  0,  0}}, //23 open 11
	{5, {164,122, 81,121}, {  0,  0}}, //24 open 12
	{6, {  0,  0, 81,121}, {  0,  0}}, //25 open 13
	{6, { 82,  0, 81,121}, {  0,  0}}, //26 open 14
	{6, {164,  0, 81,121}, {  0,  0}}, //27 open 15
	{6, {  0,122, 81,121}, {  0,  0}}, //28 open 16
	{6, { 82,122, 81,121}, {  0,  0}}, //29 open 17
	{6, {164,122, 81,121}, {  0,  0}}, //30 open 18
	{7, {  0,  0, 81,121}, {  0,  0}}, //31 open 19
	{7, { 82,  0, 81,121}, {  0,  0}}, //32 open 20
	{7, {164,  0, 81,121}, {  0,  0}}, //33 open 21
	{7, {  0,122, 81,121}, {  0,  0}}, //34 open 22
	{7, { 82,122, 81,121}, {  0,  0}}, //35 open 23
	{7, {164,122, 81,121}, {  0,  0}}, //36 open 24
	{8, {  0,  0, 81,121}, {  0,  0}}, //37 open 25
	{8, { 82,  0, 81,121}, {  0,  0}}, //38 open 26
	{8, {164,  0, 81,121}, {  0,  0}}, //39 open 27
	{8, {  0,122, 81,121}, {  0,  0}}, //40 open 28
	{8, { 82,122, 81,121}, {  0,  0}}, //41 open 29
	{8, {164,122, 81,121}, {  0,  0}}, //42 open 30
	{9, {  0,  0, 81,121}, {  0,  0}}, //43 open 31
	{9, { 82,  0, 81,121}, {  0,  0}}, //44 open 32
	{9, {164,  0, 81,121}, {  0,  0}}, //45 open 33
	{9, {  0,122, 81,121}, {  0,  0}}, //46 open 34
	{9, { 82,122, 81,121}, {  0,  0}}, //47 open 35
	{9, {164,122, 81,121}, {  0,  0}}, //48 open 36
	{10, {  0,  0, 81,121}, {  0,  0}}, //49 open 37
	{10, { 82,  0, 81,121}, {  0,  0}}, //50 open 38
	{10, {164,  0, 81,121}, {  0,  0}}, //51 open 39
	{10, {  0,122, 81,121}, {  0,  0}}, //52 open 40
	{10, { 82,122, 81,121}, {  0,  0}}, //53 open 41
	{11, {  0,  0,160,133}, {  0,  0}}, //54 ty0
};

static const Animation fback_anim[] = {
	{1, (const u8[]){0, ASCR_BACK, 1}}, //Vagas
	{1, (const u8[]){1, ASCR_BACK, 1}}, //News
	{1, (const u8[]){2, ASCR_BACK, 1}}, //Partners
	{1, (const u8[]){3, ASCR_BACK, 1}}, //Door
	{1, (const u8[]){4, ASCR_BACK, 1}}, //Fire
	{1, (const u8[]){5, ASCR_BACK, 1}}, //Sad
	{1, (const u8[]){6, ASCR_BACK, 1}}, //GRAND CANYON
	{1, (const u8[]){7, ASCR_BACK, 1}}, //Bird
	{1, (const u8[]){8, ASCR_BACK, 1}}, //Doom
	{1, (const u8[]){9, ASCR_BACK, 1}}, //Connect Four
	{1, (const u8[]){10, ASCR_BACK, 1}}, //Old
	{1, (const u8[]){11, ASCR_BACK, 1}}, //Yippee
	{1, (const u8[]){12, ASCR_BACK, 1}}, //Hint
	{1, (const u8[]){19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, ASCR_BACK, 1}}, //Open
	{1, (const u8[]){54, ASCR_BACK, 1}}, //Ty0
};

//Fback functions
void Miller_Fback_SetFrame(void *user, u8 frame)
{
	Back_Miller *this = (Back_Miller*)user;
	
	//Check if this is a new frame
	if (frame != this->fback_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &fback_frame[this->fback_frame = frame];
		if (cframe->tex != this->fback_tex_id)
			Gfx_LoadTex(&this->tex_fback, this->arc_fback_ptr[this->fback_tex_id = cframe->tex], 0);
	}
}

void Miller_Fback_Draw(Back_Miller *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &fback_frame[this->fback_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	if ((stage.song_step >= 112) && (stage.song_step <= 460))
	{
		RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
		RECT_FIXED dst = { ox, oy, 320 << FIXED_SHIFT , 240 << FIXED_SHIFT };
		Debug_StageMoveDebug(&dst, 8, stage.camera.x, stage.camera.y);
		Stage_DrawTex(&this->tex_fback, &src, &dst, FIXED_DEC (1,1));
	}
	else if ((stage.song_step >= 461) && (stage.song_step <= 4367))
	{
		RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
		RECT_FIXED dst = { ox, oy, 122 << FIXED_SHIFT , 182 << FIXED_SHIFT };
		Debug_StageMoveDebug(&dst, 8, stage.camera.x, stage.camera.y);
		Stage_DrawTex(&this->tex_fback, &src, &dst, FIXED_DEC (1,1));
	}
	else
	{
		RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
		RECT_FIXED dst = { ox, oy, src.w << FIXED_SHIFT , src.h << FIXED_SHIFT };
		Debug_StageMoveDebug(&dst, 8, stage.camera.x, stage.camera.y);
		Stage_DrawTex(&this->tex_fback, &src, &dst, FIXED_DEC (1,1));
	}
}

void Back_Miller_DrawFG(StageBack *back)
{
	Back_Miller *this = (Back_Miller*)back;
	
	fixed_t fx, fy;
	
	//start fade2
	if (stage.song_step == 76)
	{
		this->fade2 = FIXED_DEC(1,1);
		this->fadespd2 = FIXED_DEC(440,1);
	}
	if (stage.song_step == 116)
	{
		this->fade2 = FIXED_DEC(1,1);
		this->fadespd2 = FIXED_DEC(175,1);
	}
	if (stage.song_step == 140)
	{
		this->fade2 = FIXED_DEC(1,1);
		this->fadespd2 = FIXED_DEC(175,1);
	}
	if (stage.song_step == 164)
	{
		this->fade2 = FIXED_DEC(1,1);
		this->fadespd2 = FIXED_DEC(175,1);
	}
	if (stage.song_step == 187)
	{
		this->fade2 = FIXED_DEC(1,1);
		this->fadespd2 = FIXED_DEC(175,1);
	}
	if (stage.song_step == 212)
	{
		this->fade2 = FIXED_DEC(1,1);
		this->fadespd2 = FIXED_DEC(175,1);
	}
	if (stage.song_step == 235)
	{
		this->fade2 = FIXED_DEC(1,1);
		this->fadespd2 = FIXED_DEC(175,1);
	}
	if (stage.song_step == 260)
	{
		this->fade2 = FIXED_DEC(1,1);
		this->fadespd2 = FIXED_DEC(175,1);
	}
	if (stage.song_step == 284)
	{
		this->fade2 = FIXED_DEC(1,1);
		this->fadespd2 = FIXED_DEC(175,1);
	}
	if (stage.song_step == 308)
	{
		this->fade2 = FIXED_DEC(1,1);
		this->fadespd2 = FIXED_DEC(175,1);
	}
	if (stage.song_step == 332)
	{
		this->fade2 = FIXED_DEC(1,1);
		this->fadespd2 = FIXED_DEC(175,1);
	}
	if (stage.song_step == 356)
	{
		this->fade2 = FIXED_DEC(1,1);
		this->fadespd2 = FIXED_DEC(175,1);
	}
	if (stage.song_step == 380)
	{
		this->fade2 = FIXED_DEC(1,1);
		this->fadespd2 = FIXED_DEC(175,1);
	}

	//end fade2
	if (stage.song_step == 80)
		this->fade2 = 0;
	if (stage.song_step == 134)
		this->fade2 = 0;
	if (stage.song_step == 158)
		this->fade2 = 0;
	if (stage.song_step == 182)
		this->fade2 = 0;
	if (stage.song_step == 205)
		this->fade2 = 0;
	if (stage.song_step == 230)
		this->fade2 = 0;
	if (stage.song_step == 253)
		this->fade2 = 0;
	if (stage.song_step == 278)
		this->fade2 = 0;
	if (stage.song_step == 302)
		this->fade2 = 0;
	if (stage.song_step == 326)
		this->fade2 = 0;
	if (stage.song_step == 350)
		this->fade2 = 0;
	if (stage.song_step == 374)
		this->fade2 = 0;
	if (stage.song_step == 398)
		this->fade2 = 0;

	if (this->fade2 > 0)
	{
		RECT flash = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
		u8 flash_col = this->fade2 >> FIXED_SHIFT;
		Gfx_BlendRect(&flash, flash_col, flash_col, flash_col, 2);
		this->fade2 += FIXED_MUL(this->fadespd2, timer_dt);
	}
	
	//Animate and draw fback
	if ((stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 16)))
		Animatable_SetAnim(&this->fback_animatable, 12);
	if ((stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 112)))
		Animatable_SetAnim(&this->fback_animatable, 0);
	if ((stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 136)))
		Animatable_SetAnim(&this->fback_animatable, 4);
	if ((stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 160)))
		Animatable_SetAnim(&this->fback_animatable, 5);
	if ((stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 183)))
		Animatable_SetAnim(&this->fback_animatable, 6);
	if ((stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 208)))
		Animatable_SetAnim(&this->fback_animatable, 7);
	if ((stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 231)))
		Animatable_SetAnim(&this->fback_animatable, 8);
	if ((stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 256)))
		Animatable_SetAnim(&this->fback_animatable, 9);
	if ((stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 280)))
		Animatable_SetAnim(&this->fback_animatable, 10);
	if ((stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 304)))
		Animatable_SetAnim(&this->fback_animatable, 11);
	if ((stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 328)))
		Animatable_SetAnim(&this->fback_animatable, 1);
	if ((stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 352)))
		Animatable_SetAnim(&this->fback_animatable, 2);
	if ((stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 376)))
		Animatable_SetAnim(&this->fback_animatable, 3);
	if ((stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 461)))
		Animatable_SetAnim(&this->fback_animatable, 13);
	if ((stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 4368)))
		Animatable_SetAnim(&this->fback_animatable, 14);
	
	Animatable_Animate(&this->fback_animatable, (void*)this, Miller_Fback_SetFrame);
	
	if ((stage.song_step >= 18) && (stage.song_step <= 79))
		Miller_Fback_Draw(this, FIXED_DEC(-137 + 41,1), FIXED_DEC(114 - 156,1));
	
	if ((stage.song_step >= 112) && (stage.song_step <= 133) || (stage.song_step >= 136) && (stage.song_step <= 157) || (stage.song_step >= 160) && (stage.song_step <= 181) || (stage.song_step >= 183) && (stage.song_step <= 204))
		Miller_Fback_Draw(this, FIXED_DEC(-201 + 41,1), FIXED_DEC(36 - 156,1));
	if ((stage.song_step >= 208) && (stage.song_step <= 229) || (stage.song_step >= 231) && (stage.song_step <= 252) || (stage.song_step >= 256) && (stage.song_step <= 277) || (stage.song_step >= 280) && (stage.song_step <= 301))
		Miller_Fback_Draw(this, FIXED_DEC(-201 + 41,1), FIXED_DEC(36 - 156,1));
	if ((stage.song_step >= 304) && (stage.song_step <= 325) || (stage.song_step >= 328) && (stage.song_step <= 349) || (stage.song_step >= 352) && (stage.song_step <= 373) || (stage.song_step >= 376) && (stage.song_step <= 397))
		Miller_Fback_Draw(this, FIXED_DEC(-201 + 41,1), FIXED_DEC(36 - 156,1));
	
	if ((stage.song_step >= 461) && (stage.song_step <= 480))
		Miller_Fback_Draw(this, FIXED_DEC(-102 + 41,1), FIXED_DEC(65 - 156,1));
	if (stage.song_step >= 4368)
		Miller_Fback_Draw(this, FIXED_DEC(-201 + 41,1), FIXED_DEC(89 - 156,1));
	
	//start fade
	if (stage.song_step == 1824)
	{
		this->fade = FIXED_DEC(255,1);
		this->fadespd = FIXED_DEC(110,1);
	}
	else if (stage.song_step == 2531)
	{
		this->fade = FIXED_DEC(255,1);
		this->fadespd = FIXED_DEC(120,1);
	}

	//end fade
	if ((stage.song_step == 1919) || (stage.song_step == 2559))
		this->fade = 0;

	if (this->fade > 0)
	{
		RECT flash = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
		u8 flash_col = this->fade >> FIXED_SHIFT;
		Gfx_BlendRect(&flash, flash_col, flash_col, flash_col, 2);
		this->fade -= FIXED_MUL(this->fadespd, timer_dt);
	}
	
	//Draw blackf
	RECT screen_src = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
	if ((stage.song_step <= 480) || (stage.song_step >= 4312) || (stage.song_step >= 3771) && (stage.song_step <= 3776) || (stage.song_step >= 4029) && (stage.song_step <= 4032))
		Gfx_DrawRect(&screen_src, 0, 0, 0);
	if ((stage.song_step >= 1776) && (stage.song_step <= 1824) || (stage.song_step >= 2464) && (stage.song_step <= 2531))
		Gfx_DrawRect(&screen_src, 0, 0, 0);
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
	
	//Draw spite1
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT spite1_src = {65, 0, 87, 34};
	RECT_FIXED spite1_dst = {
		FIXED_DEC(-53 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(207,1) - fy,
		FIXED_DEC(58 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(23,1)
	};
	
	//Draw spite2
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT spite2_src = {153, 0, 47, 35};
	RECT_FIXED spite2_dst = {
		FIXED_DEC(-175 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(194,1) - fy,
		FIXED_DEC(24 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(18,1)
	};
	
	//Draw spite3
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT spite3_src = {65, 36, 83, 80};
	RECT_FIXED spite3_dst = {
		FIXED_DEC(-167 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(106,1) - fy,
		FIXED_DEC(52 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(50,1)
	};
	
	//Draw spite4
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT spite4_src = {149, 36, 104, 79};
	RECT_FIXED spite4_dst = {
		FIXED_DEC(-81 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(68,1) - fy,
		FIXED_DEC(87 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(66,1)
	};
	
	Debug_StageMoveDebug(&spite1_dst, 9, fx, fy);
	if (stage.spite <= 19)
		Stage_DrawTex(&this->tex_spite, &spite1_src, &spite1_dst, stage.camera.bzoom);
	Debug_StageMoveDebug(&spite2_dst, 10, fx, fy);
	if (stage.spite <= 30)
		Stage_DrawTex(&this->tex_spite, &spite2_src, &spite2_dst, stage.camera.bzoom);
	Debug_StageMoveDebug(&spite3_dst, 11, fx, fy);
	if (stage.spite <= 41)
		Stage_DrawTex(&this->tex_spite, &spite3_src, &spite3_dst, stage.camera.bzoom);
	Debug_StageMoveDebug(&spite4_dst, 12, fx, fy);
	if (stage.spite <= 51)
		Stage_DrawTex(&this->tex_spite, &spite4_src, &spite4_dst, stage.camera.bzoom);
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
	
	//Free fback archive
	Mem_Free(this->arc_fback);

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
	this->back.draw_fg = Back_Miller_DrawFG;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Miller_DrawBG;
	this->back.free = Back_Miller_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\MILLER\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back, Archive_Find(arc_back, "back.tim"), 0);
	Gfx_LoadTex(&this->tex_spite, Archive_Find(arc_back, "spite.tim"), 0);
	Mem_Free(arc_back);
	
	//Load doggo textures
	this->arc_doggo = IO_Read("\\MILLER\\DOGGO.ARC;1");
	this->arc_doggo_ptr[0] = Archive_Find(this->arc_doggo, "doggo.tim");
	
	//Load fback textures
	this->arc_fback = IO_Read("\\MILLER\\FBACK.ARC;1");
	this->arc_fback_ptr[0] = Archive_Find(this->arc_fback, "fback0.tim");
	this->arc_fback_ptr[1] = Archive_Find(this->arc_fback, "fback1.tim");
	this->arc_fback_ptr[2] = Archive_Find(this->arc_fback, "fback2.tim");
	this->arc_fback_ptr[3] = Archive_Find(this->arc_fback, "hint.tim");
	this->arc_fback_ptr[4] = Archive_Find(this->arc_fback, "open0.tim");
	this->arc_fback_ptr[5] = Archive_Find(this->arc_fback, "open1.tim");
	this->arc_fback_ptr[6] = Archive_Find(this->arc_fback, "open2.tim");
	this->arc_fback_ptr[7] = Archive_Find(this->arc_fback, "open3.tim");
	this->arc_fback_ptr[8] = Archive_Find(this->arc_fback, "open4.tim");
	this->arc_fback_ptr[9] = Archive_Find(this->arc_fback, "open5.tim");
	this->arc_fback_ptr[10] = Archive_Find(this->arc_fback, "open6.tim");
	this->arc_fback_ptr[11] = Archive_Find(this->arc_fback, "ty0.tim");
	
	//Initialize doggo state
	Animatable_Init(&this->doggo_animatable, doggo_anim);
	Animatable_SetAnim(&this->doggo_animatable, 0);
	this->doggo_frame = this->doggo_tex_id = 0xFF; //Force art load
	
	//Initialize fback state
	Animatable_Init(&this->fback_animatable, fback_anim);
	Animatable_SetAnim(&this->fback_animatable, 0);
	this->fback_frame = this->fback_tex_id = 0xFF; //Force art load
	
	//Initialize Fade
	this->fade = this->fadespd = 0;
	this->fade2 = this->fadespd2 = 0;
	
	return (StageBack*)this;
}
