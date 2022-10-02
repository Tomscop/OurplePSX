/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "black2.h"

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
	IO_Data arc_eye, arc_eye_ptr[23];
	
	Gfx_Tex tex_back0; //Blackbg
	
	//Eye state
	Gfx_Tex tex_eye;
	u8 eye_frame, eye_tex_id;

	Animatable eye_animatable;

} Back_Black2;

//Eye animation and rects
static const CharFrame eye_frame[] = {
	{0, {  0,  0,173,102}, {  0,  0}}, //0 eye 1
	{0, {  0,103,173,102}, {  0,  0}}, //1 eye 2
	{1, {  0,  0,173,102}, {  0,  0}}, //2 eye 3
	{1, {  0,103,173,102}, {  0,  0}}, //3 eye 4
	{2, {  0,  0,173,102}, {  0,  0}}, //4 eye 5
	{2, {  0,103,173,102}, {  0,  0}}, //5 eye 6
	{3, {  0,  0,173,102}, {  0,  0}}, //6 eye 7
	{3, {  0,103,173,102}, {  0,  0}}, //7 eye 8
	{4, {  0,  0,173,102}, {  0,  0}}, //8 eye 9
	{4, {  0,103,173,102}, {  0,  0}}, //9 eye 10
	{5, {  0,  0,173,102}, {  0,  0}}, //10 eye 11
	{5, {  0,103,173,102}, {  0,  0}}, //11 eye 12
	{6, {  0,  0,173,102}, {  0,  0}}, //12 eye 13
	{6, {  0,103,173,102}, {  0,  0}}, //13 eye 14
	{7, {  0,  0,173,102}, {  0,  0}}, //14 eye 15
	{7, {  0,103,173,102}, {  0,  0}}, //15 eye 16
	{8, {  0,  0,173,102}, {  0,  0}}, //16 eye 17
	{8, {  0,103,173,102}, {  0,  0}}, //17 eye 18
	{9, {  0,  0,173,102}, {  0,  0}}, //18 eye 19
	{9, {  0,103,173,102}, {  0,  0}}, //19 eye 20
	{10, {  0,  0,173,102}, {  0,  0}}, //20 eye 21
	{10, {  0,103,173,102}, {  0,  0}}, //21 eye 22
	{11, {  0,  0,173,102}, {  0,  0}}, //22 eye 23
	{11, {  0,103,173,102}, {  0,  0}}, //23 eye 24
	{12, {  0,  0,173,102}, {  0,  0}}, //24 eye 25
	{12, {  0,103,173,102}, {  0,  0}}, //25 eye 26
	{13, {  0,  0,173,102}, {  0,  0}}, //26 eye 27
	{13, {  0,103,173,102}, {  0,  0}}, //27 eye 28
	{14, {  0,  0,173,102}, {  0,  0}}, //28 eye 29
	{14, {  0,103,173,102}, {  0,  0}}, //29 eye 30
	{15, {  0,  0,173,102}, {  0,  0}}, //30 eye 31
	{15, {  0,103,173,102}, {  0,  0}}, //31 eye 32
	{16, {  0,  0,173,102}, {  0,  0}}, //32 eye 33
	{16, {  0,103,173,102}, {  0,  0}}, //33 eye 34
	{17, {  0,  0,173,102}, {  0,  0}}, //34 eye 35
	{17, {  0,103,173,102}, {  0,  0}}, //35 eye 36
	{18, {  0,  0,173,102}, {  0,  0}}, //36 eye 37
	{18, {  0,103,173,102}, {  0,  0}}, //37 eye 38
	{19, {  0,  0,173,102}, {  0,  0}}, //38 eye 39
	{19, {  0,103,173,102}, {  0,  0}}, //39 eye 40
	{20, {  0,  0,173,102}, {  0,  0}}, //40 eye 41
	{20, {  0,103,173,102}, {  0,  0}}, //41 eye 42
	{21, {  0,  0,173,102}, {  0,  0}}, //42 eye 43
	{21, {  0,103,173,102}, {  0,  0}}, //43 eye 44
	{22, {  0,  0,173,102}, {  0,  0}}, //44 eye 45
	{22, {  0,103,173,102}, {  0,  0}}, //45 eye 46
	{23, {  0,  0,173,102}, {  0,  0}}, //46 eye 47
};

static const Animation eye_anim[] = {
	{2, (const u8[]){0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, ASCR_CHGANI, 0}}, //0 Eye
};

//Eye functions
void Black2_Eye_SetFrame(void *user, u8 frame)
{
	Back_Black2 *this = (Back_Black2*)user;
	
	//Check if this is a new frame
	if (frame != this->eye_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &eye_frame[this->eye_frame = frame];
		if (cframe->tex != this->eye_tex_id)
			Gfx_LoadTex(&this->tex_eye, this->arc_eye_ptr[this->eye_tex_id = cframe->tex], 0);
	}
}

void Black2_Eye_Draw(Back_Black2 *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &eye_frame[this->eye_frame];
    
    fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = {ox, oy,410 << FIXED_SHIFT,242 << FIXED_SHIFT};
	Debug_StageMoveDebug(&dst, 6, stage.camera.x, stage.camera.y);
	Stage_DrawTex(&this->tex_eye, &src, &dst, FIXED_DEC (1,1));
}

void Back_Black2_DrawBG(StageBack *back)
{
	Back_Black2 *this = (Back_Black2*)back;

	fixed_t fx, fy;
	
	//Animate and draw eye
	if (stage.flag & STAGE_FLAG_JUST_STEP && (stage.song_step == 0))
	{
		Animatable_SetAnim(&this->eye_animatable, 0);
	}
	Animatable_Animate(&this->eye_animatable, (void*)this, Black2_Eye_SetFrame);
	
	if (stage.prefs.eyes == true)	
		Black2_Eye_Draw(this, FIXED_DEC(-254 - -55,1), FIXED_DEC(-138 - -15,1));

	//Draw blackbg
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT blackbg_src = {0, 0, 256, 256};
	RECT_FIXED blackbg_dst = {
		FIXED_DEC(-265 - screen.SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-145,1) - fy,
		FIXED_DEC(400 + screen.SCREEN_WIDEOADD,1),
		FIXED_DEC(400,1)
	};
	
	Debug_StageMoveDebug(&blackbg_dst, 8, fx, fy);
	Stage_DrawTex(&this->tex_back0, &blackbg_src, &blackbg_dst, stage.camera.bzoom);
}

void Back_Black2_Free(StageBack *back)
{
	Back_Black2 *this = (Back_Black2*)back;
	
	//Free eye archive
	Mem_Free(this->arc_eye);

	//Free structure
	Mem_Free(this);
}

StageBack *Back_Black2_New(void)
{
	//Allocate background structure
	Back_Black2 *this = (Back_Black2*)Mem_Alloc(sizeof(Back_Black2));
	if (this == NULL)
		return NULL;
	
		//Set background functions
		this->back.draw_fg = NULL;
		this->back.draw_md = NULL;
		this->back.draw_bg = Back_Black2_DrawBG;
		this->back.free = Back_Black2_Free;
		
		//Load background textures
		IO_Data arc_back = IO_Read("\\BLACK\\BACK.ARC;1");
		Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
		
		//Load eye textures
		this->arc_eye = IO_Read("\\BLACK2\\EYE.ARC;1");
		this->arc_eye_ptr[0] = Archive_Find(this->arc_eye, "eye0.tim");
		this->arc_eye_ptr[1] = Archive_Find(this->arc_eye, "eye1.tim");
		this->arc_eye_ptr[2] = Archive_Find(this->arc_eye, "eye2.tim");
		this->arc_eye_ptr[3] = Archive_Find(this->arc_eye, "eye3.tim");
		this->arc_eye_ptr[4] = Archive_Find(this->arc_eye, "eye4.tim");
		this->arc_eye_ptr[5] = Archive_Find(this->arc_eye, "eye5.tim");
		this->arc_eye_ptr[6] = Archive_Find(this->arc_eye, "eye6.tim");
		this->arc_eye_ptr[7] = Archive_Find(this->arc_eye, "eye7.tim");
		this->arc_eye_ptr[8] = Archive_Find(this->arc_eye, "eye8.tim");
		this->arc_eye_ptr[9] = Archive_Find(this->arc_eye, "eye9.tim");
		this->arc_eye_ptr[10] = Archive_Find(this->arc_eye, "eye10.tim");
		this->arc_eye_ptr[11] = Archive_Find(this->arc_eye, "eye11.tim");
		this->arc_eye_ptr[12] = Archive_Find(this->arc_eye, "eye12.tim");
		this->arc_eye_ptr[13] = Archive_Find(this->arc_eye, "eye13.tim");
		this->arc_eye_ptr[14] = Archive_Find(this->arc_eye, "eye14.tim");
		this->arc_eye_ptr[15] = Archive_Find(this->arc_eye, "eye15.tim");
		this->arc_eye_ptr[16] = Archive_Find(this->arc_eye, "eye16.tim");
		this->arc_eye_ptr[17] = Archive_Find(this->arc_eye, "eye17.tim");
		this->arc_eye_ptr[18] = Archive_Find(this->arc_eye, "eye18.tim");
		this->arc_eye_ptr[19] = Archive_Find(this->arc_eye, "eye19.tim");
		this->arc_eye_ptr[20] = Archive_Find(this->arc_eye, "eye20.tim");
		this->arc_eye_ptr[21] = Archive_Find(this->arc_eye, "eye21.tim");
		this->arc_eye_ptr[22] = Archive_Find(this->arc_eye, "eye22.tim");
		this->arc_eye_ptr[23] = Archive_Find(this->arc_eye, "eye23.tim");
	
	//Initialize eye state
	Animatable_Init(&this->eye_animatable, eye_anim);
	Animatable_SetAnim(&this->eye_animatable, 0);
	this->eye_frame = this->eye_tex_id = 0xFF; //Force art load
	
	return (StageBack*)this;
}
