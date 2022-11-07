/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "jackass.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Jackass player types
enum
{
	Jackass_ArcMain_JKA0,
	Jackass_ArcMain_JKA1,
	Jackass_ArcMain_JKA2,
	Jackass_ArcMain_JKA3,
	
	Jackass_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Jackass_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
} Char_Jackass;

//Jackass player definitions
static const CharFrame char_jackass_frame[] = 
{
	{Jackass_ArcMain_JKA0, {  0,  0,103,109}, { 53, 92}}, //0 idle 1
	{Jackass_ArcMain_JKA0, {104,  0,103,109}, { 53, 92}}, //1 idle 2
	{Jackass_ArcMain_JKA0, {  0,110,103,109}, { 53, 92}}, //2 idle 3
	{Jackass_ArcMain_JKA0, {104,110,103,109}, { 53, 92}}, //3 idle 3
	
	{Jackass_ArcMain_JKA1, {  0,  0,103,109}, { 53, 92}}, //4 left 1
	{Jackass_ArcMain_JKA1, {104,  0,103,109}, { 53, 92}}, //5 left 2
	
	{Jackass_ArcMain_JKA1, {  0,110,103,109}, { 53, 92}}, //6 down 1
	{Jackass_ArcMain_JKA1, {104,110,103,109}, { 53, 92}}, //7 down 2
	
	{Jackass_ArcMain_JKA2, {  0,  0,103,109}, { 53, 92}}, //8 up 1
	{Jackass_ArcMain_JKA2, {104,  0,103,109}, { 53, 92}}, //9 up 2
	
	{Jackass_ArcMain_JKA2, {  0,110,103,109}, { 53, 92}}, //10 right 1
	{Jackass_ArcMain_JKA2, {104,110,103,109}, { 53, 92}}, //11 right 2
	
	{Jackass_ArcMain_JKA3, {  0,  0,103,109}, { 53, 92}}, //12 miss
};
static const Animation char_jackass_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2, 3, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 4, 5, ASCR_BACK, 1}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 6, 7, ASCR_BACK, 1}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){ 8, 9, ASCR_BACK, 1}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){ 10, 11, ASCR_BACK, 1}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
	
	{1, (const u8[]){ 12, 12, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{1, (const u8[]){ 12, 12, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{1, (const u8[]){ 12, 12, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{1, (const u8[]){ 12, 12, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};

//Jackass player functions
void Char_Jackass_SetFrame(void *user, u8 frame)
{
	Char_Jackass *this = (Char_Jackass*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		CharFrame *cframe;

 		cframe = &char_jackass_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Jackass_Tick(Character *character)
{
	Char_Jackass *this = (Char_Jackass*)character;

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
	Animatable_Animate(&character->animatable, (void*)this, Char_Jackass_SetFrame);

	Character_Draw(character, &this->tex, &char_jackass_frame[this->frame]);
}

void Char_Jackass_SetAnim(Character *character, u8 anim)
{
	Char_Jackass *this = (Char_Jackass*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Jackass_Free(Character *character)
{
	Char_Jackass *this = (Char_Jackass*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Jackass_New(fixed_t x, fixed_t y)
{
	//Allocate jackass object
	Char_Jackass *this = Mem_Alloc(sizeof(Char_Jackass));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Jackass_New] Failed to allocate jackass object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Jackass_Tick;
	this->character.set_anim = Char_Jackass_SetAnim;
	this->character.free = Char_Jackass_Free;
	
	Animatable_Init(&this->character.animatable, char_jackass_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	this->character.health_i = 3;

	//health bar color
	this->character.health_bar = 0xFF075BFF;

	this->character.focus_x = FIXED_DEC(-30,1);
	this->character.focus_y = FIXED_DEC(-59,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR2\\JACKASS.ARC;1");
	
	const char **pathp = (const char *[]){
		"jka0.tim",   //Jackass_ArcMain_JKA0
		"jka1.tim",   //Jackass_ArcMain_JKA1
		"jka2.tim",   //Jackass_ArcMain_JKA2
		"jka3.tim",   //Jackass_ArcMain_JKA3
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);

	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
