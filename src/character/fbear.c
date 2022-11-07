/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "fbear.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//FBear player types
enum
{
	FBear_ArcMain_Idle0,
	FBear_ArcMain_Idle1,
	FBear_ArcMain_Left,
	FBear_ArcMain_LeftMiss,
	FBear_ArcMain_Down,
	FBear_ArcMain_DownMiss,
	FBear_ArcMain_Up,
	FBear_ArcMain_UpMiss,
	FBear_ArcMain_Right,
	FBear_ArcMain_RightMiss,
	
	FBear_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[FBear_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
} Char_FBear;

//FBear player definitions
static const CharFrame char_fbear_frame[] = 
{
	{FBear_ArcMain_Idle0, {  0,  0, 73,126}, { 53, 92}}, //0 idle 1
	{FBear_ArcMain_Idle0, { 74,  0, 73,126}, { 53, 92}}, //1 idle 2
	{FBear_ArcMain_Idle1, {  0,  0, 73,126}, { 53, 92}}, //2 idle 3
	{FBear_ArcMain_Idle1, { 74,  0, 73,126}, { 53, 92}}, //3 idle 3
	
	{FBear_ArcMain_Left, {  0,  0, 73,126}, { 53, 92}}, //4 left 1
	{FBear_ArcMain_Left, { 74,  0, 73,126}, { 53, 92}}, //5 left 2
	
	{FBear_ArcMain_Down, {  0,  0, 73,126}, { 53, 92}}, //6 down 1
	{FBear_ArcMain_Down, { 74,  0, 73,126}, { 53, 92}}, //7 down 2
	
	{FBear_ArcMain_Up, {  0,  0, 73,126}, { 53, 92}}, //8 up 1
	{FBear_ArcMain_Up, { 74,  0, 73,126}, { 53, 92}}, //9 up 2
	
	{FBear_ArcMain_Right, {  0,  0, 73,126}, { 53, 92}}, //10 right 1
	{FBear_ArcMain_Right, { 74,  0, 73,126}, { 53, 92}}, //11 right 2
	
	{FBear_ArcMain_LeftMiss, {  0,  0, 73,126}, { 53, 92}}, //12 left miss 1
	{FBear_ArcMain_LeftMiss, { 74,  0, 73,126}, { 53, 92}}, //13 left miss 2
	
	{FBear_ArcMain_DownMiss, {  0,  0, 73,126}, { 53, 92}}, //14 down miss 1
	{FBear_ArcMain_DownMiss, { 74,  0, 73,126}, { 53, 92}}, //15 down miss 2
	
	{FBear_ArcMain_UpMiss, {  0,  0, 73,126}, { 53, 92}}, //16 up miss 1
	{FBear_ArcMain_UpMiss, { 74,  0, 73,126}, { 53, 92}}, //17 up miss 2
	
	{FBear_ArcMain_RightMiss, {  0,  0, 73,126}, { 53, 92}}, //18 right miss 1
	{FBear_ArcMain_RightMiss, { 74,  0, 73,126}, { 53, 92}}, //19 right miss 2
};
static const Animation char_fbear_anim[PlayerAnim_Max] = {
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

//FBear player functions
void Char_FBear_SetFrame(void *user, u8 frame)
{
	Char_FBear *this = (Char_FBear*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		CharFrame *cframe;

 		cframe = &char_fbear_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_FBear_Tick(Character *character)
{
	Char_FBear *this = (Char_FBear*)character;
	
	//Camera stuff
	if (stage.stage_id == StageId_4_3)
	{
		if (stage.song_step == -37)
		{
			this->character.focus_zoom = FIXED_DEC(3500,512);
		}
		if (stage.song_step == -36)
		{
			this->character.focus_zoom = FIXED_DEC(1064,1024);
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
	Animatable_Animate(&character->animatable, (void*)this, Char_FBear_SetFrame);

	Character_Draw(character, &this->tex, &char_fbear_frame[this->frame]);
}

void Char_FBear_SetAnim(Character *character, u8 anim)
{
	Char_FBear *this = (Char_FBear*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_FBear_Free(Character *character)
{
	Char_FBear *this = (Char_FBear*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_FBear_New(fixed_t x, fixed_t y)
{
	//Allocate fbear object
	Char_FBear *this = Mem_Alloc(sizeof(Char_FBear));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_FBear_New] Failed to allocate fbear object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_FBear_Tick;
	this->character.set_anim = Char_FBear_SetAnim;
	this->character.free = Char_FBear_Free;
	
	Animatable_Init(&this->character.animatable, char_fbear_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	if (stage.stage_id == StageId_3_1)
		this->character.health_i = 2;
	else
		this->character.health_i = 9;

	//health bar color
	if (stage.stage_id == StageId_3_1)
	{
		this->character.health_bar = 0xFFC39B06;
		
		this->character.focus_x = FIXED_DEC(-16,1);
		this->character.focus_y = FIXED_DEC(-47,1);
		this->character.focus_zoom = FIXED_DEC(311,256);
	}
	else
	{
		this->character.health_bar = 0xFF000000;
		
		this->character.focus_x = FIXED_DEC(354,1);
		this->character.focus_y = FIXED_DEC(-245,1);
		this->character.focus_zoom = FIXED_DEC(1064,1024);
	}
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\FBEAR.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",   //FBear_ArcMain_Idle0
		"idle1.tim",   //FBear_ArcMain_Idle1
		"left.tim",   //FBear_ArcMain_Left
		"leftm.tim",   //FBear_ArcMain_LeftMiss
		"down.tim",   //FBear_ArcMain_Down
		"downm.tim",   //FBear_ArcMain_DownMiss
		"up.tim",   //FBear_ArcMain_Up
		"upm.tim",   //FBear_ArcMain_UpMiss
		"right.tim",   //FBear_ArcMain_Right
		"rightm.tim",   //FBear_ArcMain_RightMiss
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);

	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
