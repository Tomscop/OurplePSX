/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "peter.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Peter player types
enum
{
	Peter_ArcMain_Peter0,
	Peter_ArcMain_Peter1,
	Peter_ArcMain_Peter2,
	
	Peter_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Peter_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
} Char_Peter;

//Peter player definitions
static const CharFrame char_peter_frame[] = 
{
	{Peter_ArcMain_Peter0, {  0,  0, 53,111}, {  0,  0}}, //0 idle 1
	{Peter_ArcMain_Peter0, { 54,  0, 55,110}, {  1, -1}}, //1 idle 2
	{Peter_ArcMain_Peter0, {110,  0, 52,113}, {  0,  2}}, //2 idle 3
	{Peter_ArcMain_Peter0, {163,  0, 52,113}, {  0,  2}}, //3 idle 4
	
	{Peter_ArcMain_Peter0, {  0,114, 73,109}, { 27, -4}}, //4 left 1
	{Peter_ArcMain_Peter0, { 74,114, 72,109}, { 26, -4}}, //5 left 2
	
	{Peter_ArcMain_Peter0, {147,114, 56, 90}, {  1,-22}}, //6 down 1
	{Peter_ArcMain_Peter1, {  0,  0, 55, 90}, {  0,-22}}, //7 down 2
	
	{Peter_ArcMain_Peter1, { 56,  0, 56,116}, { -2,  3}}, //8 up 1
	{Peter_ArcMain_Peter1, {113,  0, 56,115}, { -2,  2}}, //9 up 2
	
	{Peter_ArcMain_Peter1, {170,  0, 59,104}, { -4, -7}}, //10 right 1
	{Peter_ArcMain_Peter1, {  0,117, 60,104}, { -3, -7}}, //11 right 2
	
	{Peter_ArcMain_Peter1, {133,117, 72,108}, { 26, -4}}, //12 left miss 1
	{Peter_ArcMain_Peter2, {  0,  0, 72,108}, { 26, -4}}, //13 left miss 2
	
	{Peter_ArcMain_Peter2, { 73,  0, 55, 92}, {  0,-21}}, //14 down miss 1
	{Peter_ArcMain_Peter2, {129,  0, 55, 91}, { -1,-21}}, //15 down miss 2

	{Peter_ArcMain_Peter2, {185,  0, 56,114}, { -2,  2}}, //16 up miss 1
	{Peter_ArcMain_Peter2, {  0,115, 56,114}, { -2,  2}}, //17 up miss 2
	
	{Peter_ArcMain_Peter2, { 57,115, 61,103}, { -2, -7}}, //18 right miss 1
	{Peter_ArcMain_Peter2, {119,115, 59,103}, { -3, -8}}, //19 right miss 2
	
	{Peter_ArcMain_Peter1, { 61,117, 71, 70}, { 19,-40}}, //20 dead
};
static const Animation char_peter_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2, 3, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 4, 5, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 6, 7, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 8, 9, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){ 10, 11, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
	
	{1, (const u8[]){ 12, 13, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{1, (const u8[]){ 14, 15, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{1, (const u8[]){ 16, 17, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{1, (const u8[]){ 18, 19, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};
static const Animation char_peter_anim2[PlayerAnim_Max] = {
	{2, (const u8[]){ 20, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 20, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 20, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 20, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){ 20, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
	
	{2, (const u8[]){ 20, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{2, (const u8[]){ 20, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{2, (const u8[]){ 20, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{2, (const u8[]){ 20, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};

//Peter player functions
void Char_Peter_SetFrame(void *user, u8 frame)
{
	Char_Peter *this = (Char_Peter*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		CharFrame *cframe;

 		cframe = &char_peter_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Peter_Tick(Character *character)
{
	Char_Peter *this = (Char_Peter*)character;
	
	//Camera stuff
	if (stage.stage_id == StageId_6_3)
	{
		if (stage.song_step == 2160)
		{
			this->character.focus_zoom = FIXED_DEC(206,128);
		}
		if (stage.song_step == 2162)
		{
			this->character.focus_zoom = FIXED_DEC(181,256);
		}
		if (stage.song_step == 2164)
		{
			this->character.focus_zoom = FIXED_DEC(226,128);
		}
		if (stage.song_step == 2166)
		{
			this->character.focus_zoom = FIXED_DEC(181,256);
		}
		if (stage.song_step == 2168)
		{
			this->character.focus_zoom = FIXED_DEC(226,128);
		}
		if (stage.song_step == 2169)
		{
			this->character.focus_zoom = FIXED_DEC(181,256);
		}
		if (stage.song_step == 2171)
		{
			this->character.focus_zoom = FIXED_DEC(138,64);
		}
		if (stage.song_step == 2172)
		{
			this->character.focus_zoom = FIXED_DEC(181,256);
		}
		if (stage.song_step == 2173)
		{
			this->character.focus_zoom = FIXED_DEC(1234,512);
		}
		if (stage.song_step == 2175)
		{
			this->character.focus_zoom = FIXED_DEC(181,256);
		}
		if (stage.song_step == 2432)
		{
			this->character.focus_zoom = FIXED_DEC(1851,512);
		}
		if (stage.song_step == 2433)
		{
			this->character.focus_zoom = FIXED_DEC(181,256);
		}
	}
	
	//Handle animation updates
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0 ||
	    (character->animatable.anim != CharAnim_Left &&
	     character->animatable.anim != CharAnim_LeftAlt &&
	     character->animatable.anim != CharAnim_Down &&
	     character->animatable.anim != CharAnim_DownAlt &&
	     character->animatable.anim != CharAnim_Up &&
	     character->animatable.anim != CharAnim_UpAlt &&
	     character->animatable.anim != CharAnim_Right &&
	     character->animatable.anim != CharAnim_RightAlt))
		Character_CheckEndSing(character);
	
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		//Perform idle dance
		if (Animatable_Ended(&character->animatable) &&
			(character->animatable.anim != CharAnim_Left &&
		     character->animatable.anim != CharAnim_LeftAlt &&
		     character->animatable.anim != PlayerAnim_LeftMiss &&
		     character->animatable.anim != CharAnim_Down &&
		     character->animatable.anim != CharAnim_DownAlt &&
		     character->animatable.anim != PlayerAnim_DownMiss &&
		     character->animatable.anim != CharAnim_Up &&
		     character->animatable.anim != CharAnim_UpAlt &&
		     character->animatable.anim != PlayerAnim_UpMiss &&
		     character->animatable.anim != CharAnim_Right &&
		     character->animatable.anim != CharAnim_RightAlt &&
		     character->animatable.anim != PlayerAnim_RightMiss) &&
			(stage.song_step & 0x7) == 0)
			character->set_anim(character, CharAnim_Idle);
	}
	
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_Peter_SetFrame);
	
	if (stage.song_step == 1782)
		Animatable_Init(&this->character.animatable, char_peter_anim2);
	if (stage.song_step == 4032)
	{
		Animatable_Init(&this->character.animatable, char_peter_anim);
		character->set_anim(character, CharAnim_Idle);
	}
	
	Character_Draw(character, &this->tex, &char_peter_frame[this->frame]);
}

void Char_Peter_SetAnim(Character *character, u8 anim)
{
	Char_Peter *this = (Char_Peter*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Peter_Free(Character *character)
{
	Char_Peter *this = (Char_Peter*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Peter_New(fixed_t x, fixed_t y)
{
	//Allocate peter object
	Char_Peter *this = Mem_Alloc(sizeof(Char_Peter));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Peter_New] Failed to allocate peter object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Peter_Tick;
	this->character.set_anim = Char_Peter_SetAnim;
	this->character.free = Char_Peter_Free;
	
	Animatable_Init(&this->character.animatable, char_peter_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	this->character.health_i = 4;

	//health bar color
	this->character.health_bar = 0xFFEA7B2F;

	this->character.focus_x = FIXED_DEC(-179,1);
	this->character.focus_y = FIXED_DEC(4,1);
	this->character.focus_zoom = FIXED_DEC(181,256);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR2\\PETER.ARC;1");
	
	const char **pathp = (const char *[]){
		"peter0.tim",   //Peter_ArcMain_Peter0
		"peter1.tim",   //Peter_ArcMain_Peter1
		"peter2.tim",   //Peter_ArcMain_Peter2
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);

	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
