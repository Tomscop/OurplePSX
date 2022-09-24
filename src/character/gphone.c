/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "gphone.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Golden Phone character structure
enum
{
	GPhone_ArcMain_GPhone0,
	GPhone_ArcMain_GPhone1,
	GPhone_ArcMain_GPhone2,
	GPhone_ArcMain_GPhone3,
	
	GPhone_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[GPhone_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_GPhone;

//Golden Phone character definitions
static const CharFrame char_gphone_frame[] = {
	{GPhone_ArcMain_GPhone0, {  0,  0,117, 79}, { 35,52}}, //0 idle 1
	{GPhone_ArcMain_GPhone0, {118,  0,117, 79}, { 35,52}}, //1 idle 1
	{GPhone_ArcMain_GPhone0, {  0, 80,117, 79}, { 35,52}}, //2 idle 1
	{GPhone_ArcMain_GPhone0, {118, 80,117, 79}, { 35,52}}, //3 idle 1
	
	{GPhone_ArcMain_GPhone0, {  0,160,117, 79}, { 35,52}}, //4 left 1
	{GPhone_ArcMain_GPhone0, {118,160,117, 79}, { 35,52}}, //5 left 2
	
	{GPhone_ArcMain_GPhone1, {  0,  0,117, 79}, { 35,52}}, //6 down 1
	{GPhone_ArcMain_GPhone1, {118,  0,117, 79}, { 35,52}}, //7 down 2
	
	{GPhone_ArcMain_GPhone1, {  0, 80,117, 79}, { 35,52}}, //8 up 1
	{GPhone_ArcMain_GPhone1, {118, 80,117, 79}, { 35,52}}, //9 up 2
	
	{GPhone_ArcMain_GPhone1, {  0,160,117, 79}, { 35,52}}, //10 right 1
	{GPhone_ArcMain_GPhone1, {118,160,117, 79}, { 35,52}}, //11 right 2
	
	{GPhone_ArcMain_GPhone2, {  0,  0,117, 79}, { 35,52}}, //12 ring 1
	{GPhone_ArcMain_GPhone2, {118,  0,117, 79}, { 35,52}}, //13 ring 2
	{GPhone_ArcMain_GPhone2, {  0, 80,117, 79}, { 35,52}}, //14 ring 3
	{GPhone_ArcMain_GPhone2, {118, 80,117, 79}, { 35,52}}, //15 ring 4
	{GPhone_ArcMain_GPhone2, {  0,160,117, 79}, { 35,52}}, //16 ring 5
	{GPhone_ArcMain_GPhone2, {118,160,117, 79}, { 35,52}}, //17 ring 6
	{GPhone_ArcMain_GPhone3, {  0,  0,117, 79}, { 35,52}}, //18 ring 7
	{GPhone_ArcMain_GPhone3, {118,  0,117, 79}, { 35,52}}, //19 ring 8
	{GPhone_ArcMain_GPhone3, {  0, 80,117, 79}, { 35,52}}, //20 ring 9
	{GPhone_ArcMain_GPhone3, {118, 80,117, 79}, { 35,52}}, //21 ring 10
};

static const Animation char_gphone_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, ASCR_BACK, 1}},             //CharAnim_Idle
	{2, (const u8[]){ 4, 5, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 6, 7, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 8, 9, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){10,11, ASCR_BACK, 1}},          //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
	
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},     //PlayerAnim_LeftMiss
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},     //PlayerAnim_DownMiss
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},     //PlayerAnim_UpMiss
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},     //PlayerAnim_RightMiss
	
	{1, (const u8[]){12, 13, 14, 15, 16, 17, 18, 19, 20, 21, ASCR_BACK, 1}}, //PlayerAnim_Peace
	{0, (const u8[]){12, 13, 14, 15, 16, 17, 18, 19, 20, 21, ASCR_BACK, 1}},     //PlayerAnim_Sweat
};

//Golden Phone character functions
void Char_GPhone_SetFrame(void *user, u8 frame)
{
	Char_GPhone *this = (Char_GPhone*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_gphone_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_GPhone_Tick(Character *character)
{
	Char_GPhone *this = (Char_GPhone*)character;
	
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
	
	//Stage specific animations
		switch (stage.stage_id)
		{
			case StageId_2_3: //Ring
				if (stage.song_step == 824 || stage.song_step == 2104)
					character->set_anim(character, PlayerAnim_Peace);
				break;
			default:
				break;
		}
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_GPhone_SetFrame);
	if (stage.stage_id == StageId_2_3 && stage.song_step <= 893 || stage.stage_id == StageId_2_3 && stage.song_step >= 1401)
		Character_Draw(character, &this->tex, &char_gphone_frame[this->frame]);
}

void Char_GPhone_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_GPhone_Free(Character *character)
{
	Char_GPhone *this = (Char_GPhone*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_GPhone_New(fixed_t x, fixed_t y)
{
	//Allocate gphone object
	Char_GPhone *this = Mem_Alloc(sizeof(Char_GPhone));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_GPhone_New] Failed to allocate gphone object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_GPhone_Tick;
	this->character.set_anim = Char_GPhone_SetAnim;
	this->character.free = Char_GPhone_Free;

	Animatable_Init(&this->character.animatable, char_gphone_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 7;

	//health bar color
	this->character.health_bar = 0xFFAD63D6;
	
	if (stage.stage_id == StageId_2_3)
	{	
		this->character.focus_x = FIXED_DEC(7,1);
		this->character.focus_y = FIXED_DEC(22,1);
		this->character.focus_zoom = FIXED_DEC(597,512);
	}
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\GPHONE.ARC;1");
	
	const char **pathp = (const char *[]){
		"gphone0.tim", //GPhone_ArcMain_GPhone0
		"gphone1.tim", //GPhone_ArcMain_GPhone1
		"gphone2.tim", //GPhone_ArcMain_GPhone2
		"gphone3.tim", //GPhone_ArcMain_GPhone3
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
