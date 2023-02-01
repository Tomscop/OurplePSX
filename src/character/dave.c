/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "dave.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Dave player types
enum
{
	Dave_ArcMain_Dave0,
	Dave_ArcMain_Dave1,
	Dave_ArcMain_Dave2,
	Dave_ArcMain_Dave3,
	
	Dave_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Dave_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
} Char_Dave;

//Dave player definitions
static const CharFrame char_dave_frame[] = 
{
	{Dave_ArcMain_Dave0, {  0,  0, 88,106}, {  0,  0}}, //0 idle 1
	{Dave_ArcMain_Dave0, { 89,  0, 89,104}, {  0, -2}}, //1 idle 2
	{Dave_ArcMain_Dave0, {  0,107, 85,111}, { -2,  5}}, //2 idle 3
	{Dave_ArcMain_Dave0, { 86,107, 85,111}, { -2,  5}}, //3 idle 4
	
	{Dave_ArcMain_Dave0, {178,  0, 77,110}, { 14,  2}}, //4 left 1
	{Dave_ArcMain_Dave0, {172,111, 76,110}, { 14,  2}}, //5 left 2
	
	{Dave_ArcMain_Dave1, {  0,  0, 78, 78}, {-10,-30}}, //6 down 1
	{Dave_ArcMain_Dave1, { 79,  0, 78, 79}, {-10,-29}}, //7 down 2
	
	{Dave_ArcMain_Dave1, {158,  0, 51,125}, {-21, 17}}, //8 up 1
	{Dave_ArcMain_Dave1, {  0, 80, 53,122}, {-20, 14}}, //9 up 2
	
	{Dave_ArcMain_Dave1, { 54, 80, 81,100}, { -1,-11}}, //10 right 1
	{Dave_ArcMain_Dave1, {136,126, 82,100}, {  0,-11}}, //11 right 2
	
	{Dave_ArcMain_Dave2, {  0,  0, 76,111}, { 14,  3}}, //12 left miss 1
	{Dave_ArcMain_Dave2, { 77,  0, 76,111}, { 14,  3}}, //13 left miss 2
	
	{Dave_ArcMain_Dave2, { 54,112, 78, 81}, {-10,-27}}, //14 down miss 1
	{Dave_ArcMain_Dave2, {133,123, 77, 81}, {-11,-27}}, //15 down miss 2

	{Dave_ArcMain_Dave2, {154,  0, 53,122}, {-20, 14}}, //16 up miss 1
	{Dave_ArcMain_Dave2, {  0,112, 53,122}, {-20, 14}}, //17 up miss 2
	
	{Dave_ArcMain_Dave3, {  0,  0, 82,100}, {  0,-11}}, //18 right miss 1
	{Dave_ArcMain_Dave3, { 83,  0, 81,100}, {  0,-11}}, //19 right miss 2
	
	{Dave_ArcMain_Dave1, {  0,203,114, 48}, {  8,-71}}, //20 dead
};
static const Animation char_dave_anim[PlayerAnim_Max] = {
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
static const Animation char_dave_anim2[PlayerAnim_Max] = {
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

//Dave player functions
void Char_Dave_SetFrame(void *user, u8 frame)
{
	Char_Dave *this = (Char_Dave*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		CharFrame *cframe;

 		cframe = &char_dave_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Dave_Tick(Character *character)
{
	Char_Dave *this = (Char_Dave*)character;

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
	Animatable_Animate(&character->animatable, (void*)this, Char_Dave_SetFrame);
	
	if (stage.song_step == 3774)
		Animatable_Init(&this->character.animatable, char_dave_anim2);
	if (stage.song_step == 4032)
	{
		Animatable_Init(&this->character.animatable, char_dave_anim);
		character->set_anim(character, CharAnim_Idle);
	}
	
	Character_Draw(character, &this->tex, &char_dave_frame[this->frame]);
}

void Char_Dave_SetAnim(Character *character, u8 anim)
{
	Char_Dave *this = (Char_Dave*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Dave_Free(Character *character)
{
	Char_Dave *this = (Char_Dave*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Dave_New(fixed_t x, fixed_t y)
{
	//Allocate dave object
	Char_Dave *this = Mem_Alloc(sizeof(Char_Dave));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Dave_New] Failed to allocate dave object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Dave_Tick;
	this->character.set_anim = Char_Dave_SetAnim;
	this->character.free = Char_Dave_Free;
	
	Animatable_Init(&this->character.animatable, char_dave_anim);
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
	this->arc_main = IO_Read("\\CHAR2\\DAVE.ARC;1");
	
	const char **pathp = (const char *[]){
		"dave0.tim",   //Dave_ArcMain_Dave0
		"dave1.tim",   //Dave_ArcMain_Dave1
		"dave2.tim",   //Dave_ArcMain_Dave2
		"dave3.tim",   //Dave_ArcMain_Dave3
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);

	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
