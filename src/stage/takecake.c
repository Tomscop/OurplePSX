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
	IO_Data arc_charlie, arc_charlie_ptr[16];
	
	Gfx_Tex tex_back0; //takecakebg
	
	//Kid state
	Gfx_Tex tex_kid;
	u8 kid_frame, kid_tex_id;

	Animatable kid_animatable;
	
	//Charlie state
	Gfx_Tex tex_charlie;
	u8 charlie_frame, charlie_tex_id;

	Animatable charlie_animatable;

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

//Charlie animation and rects
static const CharFrame charlie_frame[] = {
	{0, {  0,  0,255,119}, { 71, 98}}, //0 idle 1
	{0, {  0,120,255,119}, { 71, 98}}, //1 idle 2
	
	{1, {  0,  0,255,119}, { 71, 98}}, //2 death 1
	{1, {  0,120,255,119}, { 71, 98}}, //3 death 2
	{2, {  0,  0,255,119}, { 71, 98}}, //4 death 3
	{2, {  0,120,255,119}, { 71, 98}}, //5 death 4
	{3, {  0,  0,255,119}, { 71, 98}}, //6 death 5
	{3, {  0,120,255,119}, { 71, 98}}, //7 death 6
	{4, {  0,  0,255,119}, { 71, 98}}, //8 death 7
	{4, {  0,120,255,119}, { 71, 98}}, //9 death 8
	{5, {  0,  0,255,119}, { 71, 98}}, //10 death 9
	{5, {  0,120,255,119}, { 71, 98}}, //11 death 10
	{6, {  0,  0,255,119}, { 71, 98}}, //12 death 11
	{6, {  0,120,255,119}, { 71, 98}}, //13 death 12
	{7, {  0,  0,255,119}, { 71, 98}}, //14 death 13
	{7, {  0,120,255,119}, { 71, 98}}, //15 death 14
	{8, {  0,  0,255,119}, { 71, 98}}, //16 death 15
	{8, {  0,120,255,119}, { 71, 98}}, //17 death 16
	{9, {  0,  0,255,119}, { 71, 98}}, //18 death 17
	{9, {  0,120,255,119}, { 71, 98}}, //19 death 18
	{10, {  0,  0,255,119}, { 71, 98}}, //20 death 19
	{10, {  0,120,255,119}, { 71, 98}}, //21 death 20
	{11, {  0,  0,255,119}, { 71, 98}}, //22 death 21
	{11, {  0,120,255,119}, { 71, 98}}, //23 death 22
	{12, {  0,  0,255,119}, { 71, 98}}, //24 death 23
	{12, {  0,120,255,119}, { 71, 98}}, //25 death 24
	{13, {  0,  0,255,119}, { 71, 98}}, //26 death 25
	{13, {  0,120,255,119}, { 71, 98}}, //27 death 26
	{14, {  0,  0,255,119}, { 71, 98}}, //28 death 27
	{14, {  0,120,255,119}, { 71, 98}}, //29 death 28
	{15, {  0,  0,255,119}, { 71, 98}}, //30 death 29
	{15, {  0,120,255,119}, { 71, 98}}, //31 death 30
};

static const Animation charlie_anim[] = {
	{4, (const u8[]){0, 1, ASCR_CHGANI, 0}}, //Idle
	{1, (const u8[]){0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 
	10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14,
	15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
	17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 27, 28,
	29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 30, 30, 30, 31, 31, 31, ASCR_CHGANI, 2}}, //Death
	{2, (const u8[]){31, ASCR_BACK, 1}}, //Dead
};

//Charlie functions
void TakeCake_Charlie_SetFrame(void *user, u8 frame)
{
	Back_TakeCake *this = (Back_TakeCake*)user;
	
	//Check if this is a new frame
	if (frame != this->charlie_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &charlie_frame[this->charlie_frame = frame];
		if (cframe->tex != this->charlie_tex_id)
			Gfx_LoadTex(&this->tex_charlie, this->arc_charlie_ptr[this->charlie_tex_id = cframe->tex], 0);
	}
}

void TakeCake_Charlie_Draw(Back_TakeCake *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &charlie_frame[this->charlie_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = { ox, oy,138 << FIXED_SHIFT ,64 << FIXED_SHIFT };
	Debug_StageMoveDebug(&dst, 8, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_charlie, &src, &dst, stage.camera.bzoom);
}

void Back_TakeCake_DrawBG(StageBack *back)
{
	Back_TakeCake *this = (Back_TakeCake*)back;
	
	fixed_t fx, fy;

	//Animate and draw kid
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step & 0x7)== 0)
		Animatable_SetAnim(&this->kid_animatable, 0);
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step & 0x7)== 4)
		Animatable_SetAnim(&this->kid_animatable, 1);
	
	Animatable_Animate(&this->kid_animatable, (void*)this, TakeCake_Kid_SetFrame);
	
	TakeCake_Kid_Draw(this, FIXED_DEC(161 + 71,1) - fx, FIXED_DEC(31 + 98,1) - fy);
	TakeCake_Kid_Draw(this, FIXED_DEC(207 + 71,1) - fx, FIXED_DEC(71 + 98,1) - fy);
	TakeCake_Kid_Draw(this, FIXED_DEC(-126 + 71,1) - fx, FIXED_DEC(31 + 98,1) - fy);
	TakeCake_Kid_Draw(this, FIXED_DEC(-168 + 71,1) - fx, FIXED_DEC(71 + 98,1) - fy);
	
	//Draw takecakebg
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT takecakebg_src = {0, 0, 255, 255};
	RECT_FIXED takecakebg_dst = {
		FIXED_DEC(-212 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-164,1) - fy,
		FIXED_DEC(559 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(324,1)
	};

	Debug_StageMoveDebug(&takecakebg_dst, 6, fx, fy);
	Stage_DrawTex(&this->tex_back0, &takecakebg_src, &takecakebg_dst, stage.camera.bzoom);
	
	//Animate and draw charlie
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	if ((stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step & 0x3)== 0) && (stage.song_step <= 656))
		Animatable_SetAnim(&this->charlie_animatable, 0);
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 656))
		Animatable_SetAnim(&this->charlie_animatable, 1);
	
	Animatable_Animate(&this->charlie_animatable, (void*)this, TakeCake_Charlie_SetFrame);
	
	TakeCake_Charlie_Draw(this, FIXED_DEC(-127 + 71,1) - fx, FIXED_DEC(-30 + 98,1) - fy);
	
	//Draw blackf
	RECT screen_src = {0, 0, screen.SCREEN_WIDTH, screen.SCREEN_HEIGHT};
	Gfx_DrawRect(&screen_src, 0, 0, 0);
}

void Back_TakeCake_Free(StageBack *back)
{
	Back_TakeCake *this = (Back_TakeCake*)back;
	
	//Free kid archive
	Mem_Free(this->arc_kid);
	
	//Free charlie archive
	Mem_Free(this->arc_charlie);
	
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
	
	//Load charlie textures
	this->arc_charlie = IO_Read("\\TAKECAKE\\CHARLIE.ARC;1");
	this->arc_charlie_ptr[0] = Archive_Find(this->arc_charlie, "char0.tim");
	this->arc_charlie_ptr[1] = Archive_Find(this->arc_charlie, "char3.tim");
	this->arc_charlie_ptr[2] = Archive_Find(this->arc_charlie, "char4.tim");
	this->arc_charlie_ptr[3] = Archive_Find(this->arc_charlie, "char5.tim");
	this->arc_charlie_ptr[4] = Archive_Find(this->arc_charlie, "char6.tim");
	this->arc_charlie_ptr[5] = Archive_Find(this->arc_charlie, "char7.tim");
	this->arc_charlie_ptr[6] = Archive_Find(this->arc_charlie, "char8.tim");
	this->arc_charlie_ptr[7] = Archive_Find(this->arc_charlie, "char9.tim");
	this->arc_charlie_ptr[8] = Archive_Find(this->arc_charlie, "char10.tim");
	this->arc_charlie_ptr[9] = Archive_Find(this->arc_charlie, "char11.tim");
	this->arc_charlie_ptr[10] = Archive_Find(this->arc_charlie, "char12.tim");
	this->arc_charlie_ptr[11] = Archive_Find(this->arc_charlie, "char13.tim");
	this->arc_charlie_ptr[12] = Archive_Find(this->arc_charlie, "char14.tim");
	this->arc_charlie_ptr[13] = Archive_Find(this->arc_charlie, "char15.tim");
	this->arc_charlie_ptr[14] = Archive_Find(this->arc_charlie, "char16.tim");
	this->arc_charlie_ptr[15] = Archive_Find(this->arc_charlie, "char17.tim");
	
	//Initialize kid state
	Animatable_Init(&this->kid_animatable, kid_anim);
	Animatable_SetAnim(&this->kid_animatable, 0);
	this->kid_frame = this->kid_tex_id = 0xFF; //Force art load
	
	//Initialize charlie state
	Animatable_Init(&this->charlie_animatable, charlie_anim);
	Animatable_SetAnim(&this->charlie_animatable, 0);
	this->charlie_frame = this->charlie_tex_id = 0xFF; //Force art load
	
	return (StageBack*)this;
}
