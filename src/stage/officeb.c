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
	IO_Data arc_js, arc_js_ptr[35];
	
	Gfx_Tex tex_back0; //officeb1
	Gfx_Tex tex_back1; //officeb2
	
	//Js state
	Gfx_Tex tex_js;
	u8 js_frame, js_tex_id;

	Animatable js_animatable;
} Back_OfficeB;

//Js animation and rects
static const CharFrame js_frame[] = {
	{0, {  0,  0,255,255}, {  0,  0}}, //0 black
	
	{1, {  0,  0,255,254}, {  0,  0}}, //1 freddy 1
	{2, {  0,  0,255,254}, {  0,  0}}, //2 freddy 2
	{3, {  0,  0,255,254}, {  0,  0}}, //3 freddy 3
	{4, {  0,  0,255,254}, {  0,  0}}, //4 freddy 4
	{5, {  0,  0,255,254}, {  0,  0}}, //5 freddy 5
	{6, {  0,  0,255,254}, {  0,  0}}, //6 freddy 6
	{7, {  0,  0,255,254}, {  0,  0}}, //7 freddy 7
	{8, {  0,  0,255,254}, {  0,  0}}, //8 freddy 8
	{9, {  0,  0,255,254}, {  0,  0}}, //9 freddy 9
	{10, {  0,  0,255,254}, {  0,  0}}, //10 freddy 10
	{11, {  0,  0,255,254}, {  0,  0}}, //11 freddy 11
	{12, {  0,  0,255,254}, {  0,  0}}, //12 freddy 12
	{13, {  0,  0,255,254}, {  0,  0}}, //13 freddy 13
	{14, {  0,  0,255,254}, {  0,  0}}, //14 freddy 14
	{15, {  0,  0,255,254}, {  0,  0}}, //15 freddy 15
	
	{16, {  0,  0,255,254}, {  0,  0}}, //16 golden freddy
	
	{17, {  0,  0,255,220}, {  0,  0}}, //17 foxy 1
	{18, {  0,  0,255,220}, {  0,  0}}, //18 foxy 2
	{19, {  0,  0,255,220}, {  0,  0}}, //19 foxy 3
	{20, {  0,  0,255,220}, {  0,  0}}, //20 foxy 4
	{21, {  0,  0,255,220}, {  0,  0}}, //21 foxy 5
	{22, {  0,  0,255,220}, {  0,  0}}, //22 foxy 6
	{23, {  0,  0,255,220}, {  0,  0}}, //23 foxy 7
	{24, {  0,  0,255,220}, {  0,  0}}, //24 foxy 8
	{25, {  0,  0,255,220}, {  0,  0}}, //25 foxy 9
	{26, {  0,  0,255,220}, {  0,  0}}, //26 foxy 10
	{27, {  0,  0,255,220}, {  0,  0}}, //27 foxy 11
	{28, {  0,  0,255,220}, {  0,  0}}, //28 foxy 12
	{29, {  0,  0,255,220}, {  0,  0}}, //29 foxy 13
	{30, {  0,  0,255,220}, {  0,  0}}, //30 foxy 14
	{31, {  0,  0,255,220}, {  0,  0}}, //31 foxy 15
	{32, {  0,  0,255,220}, {  0,  0}}, //32 foxy 16
	{33, {  0,  0,255,220}, {  0,  0}}, //33 foxy 17
	{34, {  0,  0,255,220}, {  0,  0}}, //34 foxy 18
};

static const Animation js_anim[] = {
	{1, (const u8[]){0, ASCR_BACK, 1}}, //0 Black
	{1, (const u8[]){1, 2, 3, 1, 2, 3, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, ASCR_BACK, 1}}, //1 Freddy Jumpscare 1
	{1, (const u8[]){1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, ASCR_BACK, 1}}, //2 Freddy Jumpscare 2
	{1, (const u8[]){1, 2, 1, 2, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, ASCR_BACK, 1}}, //3 Freddy Jumpscare 3
	{1, (const u8[]){16, ASCR_BACK, 1}}, //4 Golden Freddy
	{1, (const u8[]){17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, ASCR_BACK, 1}}, //5 Foxy Run
};

//Js functions
void OfficeB_Js_SetFrame(void *user, u8 frame)
{
	Back_OfficeB *this = (Back_OfficeB*)user;
	
	//Check if this is a new frame
	if (frame != this->js_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &js_frame[this->js_frame = frame];
		if (cframe->tex != this->js_tex_id)
			Gfx_LoadTex(&this->tex_js, this->arc_js_ptr[this->js_tex_id = cframe->tex], 0);
	}
}

void OfficeB_Js_Draw(Back_OfficeB *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &js_frame[this->js_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	if (stage.song_step >= 1035 && stage.song_step <= 1050)
	{
		RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
		RECT_FIXED dst = {ox, oy,449 << FIXED_SHIFT,356 << FIXED_SHIFT};
		Debug_StageMoveDebug(&dst, 6, stage.camera.x, stage.camera.y);
		Stage_DrawTex(&this->tex_js, &src, &dst, FIXED_DEC(1,1));
	}
	else
	{
		RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
		RECT_FIXED dst = {ox, oy,323 << FIXED_SHIFT,256 << FIXED_SHIFT};
		Debug_StageMoveDebug(&dst, 6, stage.camera.x, stage.camera.y);
		Stage_DrawTex(&this->tex_js, &src, &dst, FIXED_DEC(1,1));
	}
}

void Back_OfficeB_DrawFG(StageBack *back)
{
	Back_OfficeB *this = (Back_OfficeB*)back;
	
	//Animate and draw js
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 312))
	{
				Animatable_SetAnim(&this->js_animatable, 1);
	}
	else if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 448))
	{
				Animatable_SetAnim(&this->js_animatable, 2);
	}
	else if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 1051))
	{
				Animatable_SetAnim(&this->js_animatable, 2);
	}
	else if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 1532))
	{
				Animatable_SetAnim(&this->js_animatable, 3);
	}
	else if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 2040))
	{
				Animatable_SetAnim(&this->js_animatable, 1);
	}
	else if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 496))
	{
				Animatable_SetAnim(&this->js_animatable, 4);
	}
	else if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 760))
	{
				Animatable_SetAnim(&this->js_animatable, 4);
	}
	else if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 1035))
	{
				Animatable_SetAnim(&this->js_animatable, 5);
	}
	else if (stage.flag & STAGE_FLAG_JUST_STEP && ((stage.song_step >= 488 && stage.song_step <= 495) || (stage.song_step >= 1024 && stage.song_step <= 1032) || (stage.song_step >= 1440 && stage.song_step <= 1471) || (stage.song_step >= 2240)))
	{	
		Animatable_SetAnim(&this->js_animatable, 0);
	}
	Animatable_Animate(&this->js_animatable, (void*)this, OfficeB_Js_SetFrame);
	
	if ((stage.song_step >= 312 && stage.song_step <= 319) || (stage.song_step >= 448 && stage.song_step <= 456) || (stage.song_step >= 488 && stage.song_step <= 512) || (stage.song_step >= 760 && stage.song_step <= 767) || (stage.song_step >= 1024 && stage.song_step <= 1055) || (stage.song_step >= 1440 && stage.song_step <= 1471) || (stage.song_step >= 1532 && stage.song_step <= 1554) || (stage.song_step >= 2040 && stage.song_step <= 2047) || (stage.song_step >= 2240))
		if (stage.song_step >= 1035 && stage.song_step <= 1050)
			OfficeB_Js_Draw(this, FIXED_DEC(-225 - -30,1), FIXED_DEC(-198 + -37,1));
		else
			OfficeB_Js_Draw(this, FIXED_DEC(-190 - -30,1), FIXED_DEC(-84 + -37,1));
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
	
	//Free js archive
	Mem_Free(this->arc_js);

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
	Mem_Free(arc_back);
	
	//Load js textures
	this->arc_js = IO_Read("\\OFFICEB\\JS.ARC;1");
	this->arc_js_ptr[0] = Archive_Find(this->arc_js, "jsb.tim");
	this->arc_js_ptr[1] = Archive_Find(this->arc_js, "jsf0.tim");
	this->arc_js_ptr[2] = Archive_Find(this->arc_js, "jsf1.tim");
	this->arc_js_ptr[3] = Archive_Find(this->arc_js, "jsf2.tim");
	this->arc_js_ptr[4] = Archive_Find(this->arc_js, "jsf3.tim");
	this->arc_js_ptr[5] = Archive_Find(this->arc_js, "jsf4.tim");
	this->arc_js_ptr[6] = Archive_Find(this->arc_js, "jsf5.tim");
	this->arc_js_ptr[7] = Archive_Find(this->arc_js, "jsf6.tim");
	this->arc_js_ptr[8] = Archive_Find(this->arc_js, "jsf7.tim");
	this->arc_js_ptr[9] = Archive_Find(this->arc_js, "jsf8.tim");
	this->arc_js_ptr[10] = Archive_Find(this->arc_js, "jsf9.tim");
	this->arc_js_ptr[11] = Archive_Find(this->arc_js, "jsf10.tim");
	this->arc_js_ptr[12] = Archive_Find(this->arc_js, "jsf11.tim");
	this->arc_js_ptr[13] = Archive_Find(this->arc_js, "jsf12.tim");
	this->arc_js_ptr[14] = Archive_Find(this->arc_js, "jsf13.tim");
	this->arc_js_ptr[15] = Archive_Find(this->arc_js, "jsf14.tim");
	this->arc_js_ptr[16] = Archive_Find(this->arc_js, "jsg.tim");
	this->arc_js_ptr[17] = Archive_Find(this->arc_js, "jsfx0.tim");
	this->arc_js_ptr[18] = Archive_Find(this->arc_js, "jsfx1.tim");
	this->arc_js_ptr[19] = Archive_Find(this->arc_js, "jsfx2.tim");
	this->arc_js_ptr[20] = Archive_Find(this->arc_js, "jsfx3.tim");
	this->arc_js_ptr[21] = Archive_Find(this->arc_js, "jsfx4.tim");
	this->arc_js_ptr[22] = Archive_Find(this->arc_js, "jsfx5.tim");
	this->arc_js_ptr[23] = Archive_Find(this->arc_js, "jsfx6.tim");
	this->arc_js_ptr[24] = Archive_Find(this->arc_js, "jsfx7.tim");
	this->arc_js_ptr[25] = Archive_Find(this->arc_js, "jsfx8.tim");
	this->arc_js_ptr[26] = Archive_Find(this->arc_js, "jsfx9.tim");
	this->arc_js_ptr[27] = Archive_Find(this->arc_js, "jsfx10.tim");
	this->arc_js_ptr[28] = Archive_Find(this->arc_js, "jsfx11.tim");
	this->arc_js_ptr[29] = Archive_Find(this->arc_js, "jsfx12.tim");
	this->arc_js_ptr[30] = Archive_Find(this->arc_js, "jsfx13.tim");
	this->arc_js_ptr[31] = Archive_Find(this->arc_js, "jsfx14.tim");
	this->arc_js_ptr[32] = Archive_Find(this->arc_js, "jsfx15.tim");
	this->arc_js_ptr[33] = Archive_Find(this->arc_js, "jsfx16.tim");
	this->arc_js_ptr[34] = Archive_Find(this->arc_js, "jsfx17.tim");
	
	//Initialize js state
	Animatable_Init(&this->js_animatable, js_anim);
	Animatable_SetAnim(&this->js_animatable, 0);
	this->js_frame = this->js_tex_id = 0xFF; //Force art load
	
	return (StageBack*)this;
}
