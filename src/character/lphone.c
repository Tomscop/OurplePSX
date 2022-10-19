/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "lphone.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Lore Phone character structure
enum
{
	LPhone_ArcMain_Idle0,
	LPhone_ArcMain_Idle1,
	LPhone_ArcMain_Idle2,
	LPhone_ArcMain_Idle3,
	LPhone_ArcMain_Idle4,
	LPhone_ArcMain_Left0,
	LPhone_ArcMain_Left1,
	LPhone_ArcMain_Down0,
	LPhone_ArcMain_Down1,
	LPhone_ArcMain_Up0,
	LPhone_ArcMain_Up1,
	LPhone_ArcMain_Right0,
	LPhone_ArcMain_Right1,
	LPhone_ArcMain_Ring0,
	LPhone_ArcMain_Ring1,
	LPhone_ArcMain_Ring2,
	LPhone_ArcMain_Ring3,
	LPhone_ArcMain_Ring4,
	LPhone_ArcMain_Ring5,
	LPhone_ArcMain_Ring6,
	LPhone_ArcMain_Ring7,
	LPhone_ArcMain_Ring8,
	LPhone_ArcMain_Ring9,
	
	LPhone_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[LPhone_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_LPhone;

//Golden Phone character definitions
static const CharFrame char_lphone_frame[] = {
	{LPhone_ArcMain_Idle0, {  0,  0,141,195}, { 35,52}}, //0 idle 1
	{LPhone_ArcMain_Idle1, {  0,  0,141,195}, { 35,52}}, //1 idle 2
	{LPhone_ArcMain_Idle2, {  0,  0,141,195}, { 35,52}}, //2 idle 3
	{LPhone_ArcMain_Idle3, {  0,  0,141,195}, { 35,52}}, //3 idle 4
	{LPhone_ArcMain_Idle4, {  0,  0,141,195}, { 35,52}}, //4 idle 5
	
	{LPhone_ArcMain_Left0, {  0,  0,141,195}, { 35,52}}, //5 left 1
	{LPhone_ArcMain_Left1, {  0,  0,141,195}, { 35,52}}, //6 left 2
	
	{LPhone_ArcMain_Down0, {  0,  0,141,195}, { 35,52}}, //7 down 1
	{LPhone_ArcMain_Down1, {  0,  0,141,195}, { 35,52}}, //8 down 2
	
	{LPhone_ArcMain_Up0, {  0,  0,141,195}, { 35,52}}, //9 up 1
	{LPhone_ArcMain_Up1, {  0,  0,141,195}, { 35,52}}, //10 up 2
	
	{LPhone_ArcMain_Right0, {  0,  0,141,195}, { 35,52}}, //11 right 1
	{LPhone_ArcMain_Right1, {  0,  0,141,195}, { 35,52}}, //12 right 2
	
	{LPhone_ArcMain_Ring0, {  0,  0,141,195}, { 35,52}}, //13 ring 1
	{LPhone_ArcMain_Ring1, {  0,  0,141,195}, { 35,52}}, //14 ring 2
	{LPhone_ArcMain_Ring2, {  0,  0,141,195}, { 35,52}}, //15 ring 3
	{LPhone_ArcMain_Ring3, {  0,  0,141,195}, { 35,52}}, //16 ring 4
	{LPhone_ArcMain_Ring4, {  0,  0,141,195}, { 35,52}}, //17 ring 5
	{LPhone_ArcMain_Ring5, {  0,  0,141,195}, { 35,52}}, //18 ring 6
	{LPhone_ArcMain_Ring6, {  0,  0,141,195}, { 35,52}}, //19 ring 7
	{LPhone_ArcMain_Ring7, {  0,  0,141,195}, { 35,52}}, //20 ring 8
	{LPhone_ArcMain_Ring8, {  0,  0,141,195}, { 35,52}}, //21 ring 9
	{LPhone_ArcMain_Ring9, {  0,  0,141,195}, { 35,52}}, //22 ring 10
};

static const Animation char_lphone_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}},             //CharAnim_Idle
	{2, (const u8[]){ 5, 6, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 7, 8, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){11,12, ASCR_BACK, 1}},          //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
	
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},     //PlayerAnim_LeftMiss
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},     //PlayerAnim_DownMiss
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},     //PlayerAnim_UpMiss
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},     //PlayerAnim_RightMiss
	
	{2, (const u8[]){13, 14, 15, 16, 15, 16, 15, 16, 15, 16, 15, 16, 15, 16, 15, 16, 15, 16, 15, 16, 15, 16, 15, 16, 15, 16, 15, 16, 15, 17, 18, 19, 20, 21, 22, ASCR_BACK, 1}},     //PlayerAnim_Sweat
	{2, (const u8[]){13, 14, 15, 16, 15, 16, 15, 16, 15, 16, 15, 16, 15, 16, 15, 16, 15, 16, 15, 16, 15, 16, 15, 16, 15, 16, 15, 16, 15, 17, 18, 19, 20, 21, 22, ASCR_BACK, 1}},     //PlayerAnim_Sweat
};

//Lore Phone character functions
void Char_LPhone_SetFrame(void *user, u8 frame)
{
	Char_LPhone *this = (Char_LPhone*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_lphone_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_LPhone_Tick(Character *character)
{
	Char_LPhone *this = (Char_LPhone*)character;
	
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
			case StageId_2_1: //Ring
				if (stage.song_step == 1534 || stage.song_step == 1598 || stage.song_step == 1662 || stage.song_step == 1726)
					character->set_anim(character, PlayerAnim_Peace);
				break;
			default:
				break;
		}
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_LPhone_SetFrame);
	Character_Draw(character, &this->tex, &char_lphone_frame[this->frame]);
}

void Char_LPhone_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_LPhone_Free(Character *character)
{
	Char_LPhone *this = (Char_LPhone*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_LPhone_New(fixed_t x, fixed_t y)
{
	//Allocate lphone object
	Char_LPhone *this = Mem_Alloc(sizeof(Char_LPhone));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_LPhone_New] Failed to allocate lphone object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_LPhone_Tick;
	this->character.set_anim = Char_LPhone_SetAnim;
	this->character.free = Char_LPhone_Free;

	Animatable_Init(&this->character.animatable, char_lphone_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 7;

	//health bar color
	this->character.health_bar = 0xFFAD63D6;
	
	if (stage.stage_id == StageId_2_1)
	{	
		this->character.focus_x = FIXED_DEC(-1,1);
		this->character.focus_y = FIXED_DEC(22,1);
		this->character.focus_zoom = FIXED_DEC(597,512);
	}
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\LPHONE.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //LPhone_ArcMain_Idle0
		"idle1.tim", //LPhone_ArcMain_Idle1
		"idle2.tim", //LPhone_ArcMain_Idle2
		"idle3.tim", //LPhone_ArcMain_Idle3
		"idle4.tim", //LPhone_ArcMain_Idle4
		"left0.tim", //LPhone_ArcMain_Left0
		"left1.tim", //LPhone_ArcMain_Left1
		"down0.tim", //LPhone_ArcMain_Down0
		"down1.tim", //LPhone_ArcMain_Down1
		"up0.tim", //LPhone_ArcMain_Up0
		"up1.tim", //LPhone_ArcMain_Up1
		"right0.tim", //LPhone_ArcMain_Right0
		"right1.tim", //LPhone_ArcMain_Right1
		"ring0.tim", //LPhone_ArcMain_Ring0
		"ring1.tim", //LPhone_ArcMain_Ring1
		"ring2.tim", //LPhone_ArcMain_Ring2
		"ring3.tim", //LPhone_ArcMain_Ring3
		"ring4.tim", //LPhone_ArcMain_Ring4
		"ring5.tim", //LPhone_ArcMain_Ring5
		"ring6.tim", //LPhone_ArcMain_Ring6
		"ring7.tim", //LPhone_ArcMain_Ring7
		"ring8.tim", //LPhone_ArcMain_Ring8
		"ring9.tim", //LPhone_ArcMain_Ring9
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
