/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "steven.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Steven player types
enum
{
	Steven_ArcMain_Steven0,
	Steven_ArcMain_Steven1,
	Steven_ArcMain_Steven2,
	Steven_ArcMain_Steven3,
	
	Steven_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Steven_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
} Char_Steven;

//Steven player definitions
static const CharFrame char_steven_frame[] = 
{
	{Steven_ArcMain_Steven0, {  0,  0, 72,100}, {  0,  0}}, //0 idle 1
	{Steven_ArcMain_Steven0, { 73,  0, 73,100}, {  0, -1}}, //1 idle 2
	{Steven_ArcMain_Steven0, {146,  0, 73,102}, {  0,  1}}, //2 idle 3
	{Steven_ArcMain_Steven0, {  0,103, 72,102}, {  0,  2}}, //3 idle 4
	
	{Steven_ArcMain_Steven0, { 73,103, 81,101}, {  9,  0}}, //4 left 1
	{Steven_ArcMain_Steven0, {155,103, 80,101}, {  8,  0}}, //5 left 2
	
	{Steven_ArcMain_Steven1, {  0,  0, 63, 83}, {-11,-18}}, //6 down 1
	{Steven_ArcMain_Steven1, { 64,  0, 62, 83}, {-11,-18}}, //7 down 2
	
	{Steven_ArcMain_Steven1, {127,  0, 59,105}, {-17,  4}}, //8 up 1
	{Steven_ArcMain_Steven1, {187,  0, 60,105}, {-16,  4}}, //9 up 2
	
	{Steven_ArcMain_Steven1, {  0, 84, 63,101}, {-17,  1}}, //10 right 1
	{Steven_ArcMain_Steven1, { 64, 84, 63,101}, {-16,  0}}, //11 right 2
	
	{Steven_ArcMain_Steven2, {  0,  0, 80,101}, {  8,  0}}, //12 left miss 1
	{Steven_ArcMain_Steven2, { 81,  0, 79,101}, {  7,  0}}, //13 left miss 2
	
	{Steven_ArcMain_Steven2, {161,  0, 62, 82}, {-12,-19}}, //14 down miss 1
	{Steven_ArcMain_Steven2, {  0,102, 62, 84}, {-12,-17}}, //15 down miss 2

	{Steven_ArcMain_Steven2, { 63,102, 59,104}, {-17,  3}}, //16 up miss 1
	{Steven_ArcMain_Steven2, {161,112, 59,105}, {-17,  4}}, //17 up miss 2
	
	{Steven_ArcMain_Steven3, {  0,  0, 63,101}, {-16,  1}}, //18 right miss 1
	{Steven_ArcMain_Steven3, { 64,  0, 63,101}, {-16,  1}}, //19 right miss 2
	
	{Steven_ArcMain_Steven1, {128,106,120, 63}, { 14,-54}}, //20 dead
};
static const Animation char_steven_anim[PlayerAnim_Max] = {
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
static const Animation char_steven_anim2[PlayerAnim_Max] = {
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

//Steven player functions
void Char_Steven_SetFrame(void *user, u8 frame)
{
	Char_Steven *this = (Char_Steven*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		CharFrame *cframe;

 		cframe = &char_steven_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Steven_Tick(Character *character)
{
	Char_Steven *this = (Char_Steven*)character;

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
	Animatable_Animate(&character->animatable, (void*)this, Char_Steven_SetFrame);
	
	if (stage.song_step == 1534)
		Animatable_Init(&this->character.animatable, char_steven_anim2);
	if (stage.song_step == 4032)
	{
		Animatable_Init(&this->character.animatable, char_steven_anim);
		character->set_anim(character, CharAnim_Idle);
	}
	
	Character_Draw(character, &this->tex, &char_steven_frame[this->frame]);
}

void Char_Steven_SetAnim(Character *character, u8 anim)
{
	Char_Steven *this = (Char_Steven*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Steven_Free(Character *character)
{
	Char_Steven *this = (Char_Steven*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Steven_New(fixed_t x, fixed_t y)
{
	//Allocate steven object
	Char_Steven *this = Mem_Alloc(sizeof(Char_Steven));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Steven_New] Failed to allocate steven object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Steven_Tick;
	this->character.set_anim = Char_Steven_SetAnim;
	this->character.free = Char_Steven_Free;
	
	Animatable_Init(&this->character.animatable, char_steven_anim);
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
	this->arc_main = IO_Read("\\CHAR2\\STEVEN.ARC;1");
	
	const char **pathp = (const char *[]){
		"steven0.tim",   //Steven_ArcMain_Steven0
		"steven1.tim",   //Steven_ArcMain_Steven1
		"steven2.tim",   //Steven_ArcMain_Steven2
		"steven3.tim",   //Steven_ArcMain_Steven3
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);

	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
