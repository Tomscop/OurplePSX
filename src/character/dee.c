/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "dee.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Dee player types
enum
{
	Dee_ArcMain_Dee0,
	Dee_ArcMain_Dee1,
	Dee_ArcMain_Dee2,
	Dee_ArcMain_Dee3,
	Dee_ArcMain_Dee4,
	
	Dee_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Dee_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
} Char_Dee;

//Dee player definitions
static const CharFrame char_dee_frame[] = 
{
	{Dee_ArcMain_Dee0, {  0,  0, 85,116}, {  0,  0}}, //0 idle 1
	{Dee_ArcMain_Dee0, { 86,  0, 87,115}, {  0,  0}}, //1 idle 2
	{Dee_ArcMain_Dee0, {  0,117, 83,118}, {  0,  0}}, //2 idle 3
	{Dee_ArcMain_Dee0, { 84,117, 84,117}, {  0,  0}}, //3 idle 4
	
	{Dee_ArcMain_Dee1, {  0,  0,104,114}, {  0,  0}}, //4 left 1
	{Dee_ArcMain_Dee1, {105,  0,103,114}, {  0,  0}}, //5 left 2
	
	{Dee_ArcMain_Dee1, {  0,115, 52, 77}, {  0,  0}}, //6 down 1
	{Dee_ArcMain_Dee1, { 53,115, 53, 78}, {  0,  0}}, //7 down 2
	
	{Dee_ArcMain_Dee1, {107,115, 62,113}, {  0,  0}}, //8 up 1
	{Dee_ArcMain_Dee1, {170,115, 61,113}, {  0,  0}}, //9 up 2
	
	{Dee_ArcMain_Dee2, {  0,  0, 48,119}, {  0,  0}}, //10 right 1
	{Dee_ArcMain_Dee2, { 49,  0, 49,118}, {  0,  0}}, //11 right 2
	
	{Dee_ArcMain_Dee3, { 88,  0,104,114}, {  0,  0}}, //12 left miss 1
	{Dee_ArcMain_Dee3, {  0, 83,103,114}, {  0,  0}}, //13 left miss 2
	
	{Dee_ArcMain_Dee3, {104,115, 52, 77}, {  0,  0}}, //14 down miss 1
	{Dee_ArcMain_Dee3, {157,115, 53, 77}, {  0,  0}}, //15 down miss 2

	{Dee_ArcMain_Dee3, {193,  0, 62,113}, {  0,  0}}, //16 up miss 1
	{Dee_ArcMain_Dee4, {  0,  0, 61,113}, {  0,  0}}, //17 up miss 2
	
	{Dee_ArcMain_Dee4, { 62,  0, 47,119}, {  0,  0}}, //18 right miss 1
	{Dee_ArcMain_Dee4, {110,  0, 49,119}, {  0,  0}}, //19 right miss 2
	
	{Dee_ArcMain_Dee2, { 99,  0, 86, 82}, {  0,  0}}, //20 dead 1
	{Dee_ArcMain_Dee2, {  0,120, 86, 82}, {  0,  0}}, //21 dead 2
	{Dee_ArcMain_Dee2, { 87,120, 86, 82}, {  0,  0}}, //22 dead 3
	{Dee_ArcMain_Dee3, {  0,  0, 87, 82}, {  0,  0}}, //23 dead 4
};
static const Animation char_dee_anim[PlayerAnim_Max] = {
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
static const Animation char_dee_anim2[PlayerAnim_Max] = {
	{3, (const u8[]){ 20, 21, 22, 23, ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
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

//Dee player functions
void Char_Dee_SetFrame(void *user, u8 frame)
{
	Char_Dee *this = (Char_Dee*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		CharFrame *cframe;

 		cframe = &char_dee_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Dee_Tick(Character *character)
{
	Char_Dee *this = (Char_Dee*)character;

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
	Animatable_Animate(&character->animatable, (void*)this, Char_Dee_SetFrame);
	
	if (stage.song_step == 2462)
		Animatable_Init(&this->character.animatable, char_dee_anim2);
	if (stage.song_step == 4032)
		Animatable_Init(&this->character.animatable, char_dee_anim);
	
	Character_Draw(character, &this->tex, &char_dee_frame[this->frame]);
}

void Char_Dee_SetAnim(Character *character, u8 anim)
{
	Char_Dee *this = (Char_Dee*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Dee_Free(Character *character)
{
	Char_Dee *this = (Char_Dee*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Dee_New(fixed_t x, fixed_t y)
{
	//Allocate dee object
	Char_Dee *this = Mem_Alloc(sizeof(Char_Dee));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Dee_New] Failed to allocate dee object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Dee_Tick;
	this->character.set_anim = Char_Dee_SetAnim;
	this->character.free = Char_Dee_Free;
	
	Animatable_Init(&this->character.animatable, char_dee_anim);
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
	this->arc_main = IO_Read("\\CHAR2\\DEE.ARC;1");
	
	const char **pathp = (const char *[]){
		"dee0.tim",   //Dee_ArcMain_Dee0
		"dee1.tim",   //Dee_ArcMain_Dee1
		"dee2.tim",   //Dee_ArcMain_Dee2
		"dee3.tim",   //Dee_ArcMain_Dee3
		"dee4.tim",   //Dee_ArcMain_Dee4
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);

	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
