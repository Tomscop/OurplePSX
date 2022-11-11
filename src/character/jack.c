/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "jack.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Jack player types
enum
{
	Jack_ArcMain_Jack0,
	Jack_ArcMain_Jack1,
	Jack_ArcMain_Jack2,
	
	Jack_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Jack_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
} Char_Jack;

//Jack player definitions
static const CharFrame char_jack_frame[] = 
{
	{Jack_ArcMain_Jack0, {  0,  0, 74,104}, {  0,  0}}, //0 idle 1
	{Jack_ArcMain_Jack0, { 75,  0, 75,103}, {  1, -1}}, //1 idle 2
	{Jack_ArcMain_Jack0, {151,  0, 73,105}, { -1,  1}}, //2 idle 3
	{Jack_ArcMain_Jack0, {  0,106, 73,106}, {  0,  2}}, //3 idle 4
	
	{Jack_ArcMain_Jack0, { 74,106, 90,102}, { 30, -4}}, //4 left 1
	{Jack_ArcMain_Jack0, {165,106, 88,102}, { 28, -4}}, //5 left 2
	
	{Jack_ArcMain_Jack1, {  0,  0, 65, 87}, { -5,-15}}, //6 down 1
	{Jack_ArcMain_Jack1, { 66,  0, 64, 88}, { -6,-14}}, //7 down 2
	
	{Jack_ArcMain_Jack1, {131,  0, 35,123}, {-24, 18}}, //8 up 1
	{Jack_ArcMain_Jack1, {167,  0, 36,119}, {-24, 14}}, //9 up 2
	
	{Jack_ArcMain_Jack1, {  0, 89, 64, 97}, {-13, -7}}, //10 right 1
	{Jack_ArcMain_Jack1, { 65, 89, 63, 98}, {-12, -6}}, //11 right 2
	
	{Jack_ArcMain_Jack1, {129,124, 91,103}, { 31, -3}}, //12 left miss 1
	{Jack_ArcMain_Jack2, {  0,  0, 88,103}, { 28, -3}}, //13 left miss 2
	
	{Jack_ArcMain_Jack2, { 89,  0, 65, 87}, { -5,-15}}, //14 down miss 1
	{Jack_ArcMain_Jack2, {155,  0, 64, 88}, { -6,-14}}, //15 down miss 2
	
	{Jack_ArcMain_Jack2, {128, 89, 35,123}, {-24, 18}}, //16 up miss 1
	{Jack_ArcMain_Jack2, {164, 89, 35,120}, {-24, 15}}, //17 up miss 2
	
	{Jack_ArcMain_Jack2, {  0,104, 63, 98}, {-13, -6}}, //18 right miss 1
	{Jack_ArcMain_Jack2, { 64,104, 63, 98}, {-12, -6}}, //19 right miss 2
};
static const Animation char_jack_anim[PlayerAnim_Max] = {
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

//Jack player functions
void Char_Jack_SetFrame(void *user, u8 frame)
{
	Char_Jack *this = (Char_Jack*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		CharFrame *cframe;

 		cframe = &char_jack_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Jack_Tick(Character *character)
{
	Char_Jack *this = (Char_Jack*)character;

	if (stage.stage_id == StageId_6_3)
	{
		if (stage.song_step == 3775)
		{
			this->character.health_i = 6;
		}
		if (stage.song_step == 4032)
		{
			this->character.health_i = 4;
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
	Animatable_Animate(&character->animatable, (void*)this, Char_Jack_SetFrame);

	Character_Draw(character, &this->tex, &char_jack_frame[this->frame]);
}

void Char_Jack_SetAnim(Character *character, u8 anim)
{
	Char_Jack *this = (Char_Jack*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Jack_Free(Character *character)
{
	Char_Jack *this = (Char_Jack*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Jack_New(fixed_t x, fixed_t y)
{
	//Allocate jack object
	Char_Jack *this = Mem_Alloc(sizeof(Char_Jack));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Jack_New] Failed to allocate jack object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Jack_Tick;
	this->character.set_anim = Char_Jack_SetAnim;
	this->character.free = Char_Jack_Free;
	
	Animatable_Init(&this->character.animatable, char_jack_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	this->character.health_i = 4;

	//health bar color
	this->character.health_bar = 0xFFEA7B2F;

	this->character.focus_x = FIXED_DEC(-51,1);
	this->character.focus_y = FIXED_DEC(49,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR2\\JACK.ARC;1");
	
	const char **pathp = (const char *[]){
		"jack0.tim",   //Jack_ArcMain_Jack0
		"jack1.tim",   //Jack_ArcMain_Jack1
		"jack2.tim",   //Jack_ArcMain_Jack2
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);

	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
