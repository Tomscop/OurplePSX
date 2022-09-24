/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "henry.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Henry character structure
enum
{
	Henry_ArcMain_Idle0,
	Henry_ArcMain_Idle1,
	Henry_ArcMain_Idle2,
	Henry_ArcMain_Idle3,
	Henry_ArcMain_Left0,
	Henry_ArcMain_Left1,
	Henry_ArcMain_Down0,
	Henry_ArcMain_Down1,
	Henry_ArcMain_Up0,
	Henry_ArcMain_Up1,
	Henry_ArcMain_Right0,
	Henry_ArcMain_Right1,
	Henry_ArcMain_Idiot,
	
	Henry_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Henry_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Henry;

//Henry character definitions
static const CharFrame char_henry_frame[] = {
	{Henry_ArcMain_Idle0, {  0,  0,139,131}, { 42,183+4}}, //0 idle 1
	{Henry_ArcMain_Idle1, {  0,  0,139,131}, { 42,183+4}}, //1 idle 2
	{Henry_ArcMain_Idle2, {  0,  0,139,131}, { 42,183+4}}, //2 idle 3
	{Henry_ArcMain_Idle3, {  0,  0,139,131}, { 42,183+4}}, //3 idle 4
	
	{Henry_ArcMain_Left0, {  0,  0,139,131}, { 42,183+4}}, //4 left 1
	{Henry_ArcMain_Left1, {  0,  0,139,131}, { 42,183+4}}, //5 left 2
	
	{Henry_ArcMain_Down0, {  0,  0,139,131}, { 42,183+4}}, //6 down 1
	{Henry_ArcMain_Down1, {  0,  0,139,131}, { 42,183+4}}, //7 down 2
	
	{Henry_ArcMain_Up0, {  0,  0,139,131}, { 42,183+4}}, //8 up 1
	{Henry_ArcMain_Up1, {  0,  0,139,131}, { 42,183+4}}, //9 up 2
	
	{Henry_ArcMain_Right0, {  0,  0,139,131}, { 42,183+4}}, //10 right 1
	{Henry_ArcMain_Right1, {  0,  0,139,131}, { 42,183+4}}, //11 right 2
	
	{Henry_ArcMain_Idiot, {  0,  0,139,131}, { 42,183+4}}, //12 dead
};

static const Animation char_henry_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 4,  5, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 6,  7, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 8,  9, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){10, 11, ASCR_BACK, 1}},         //CharAnim_Right
	{2, (const u8[]){12, ASCR_CHGANI, CharAnim_RightAlt}},   //CharAnim_RightAlt
};

//Henry character functions
void Char_Henry_SetFrame(void *user, u8 frame)
{
	Char_Henry *this = (Char_Henry*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_henry_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Henry_Tick(Character *character)
{
	Char_Henry *this = (Char_Henry*)character;
	
	//Camera stuff
	if (stage.stage_id == StageId_1_3)
		if (stage.song_step == 257)
		{
			this->character.focus_x = FIXED_DEC(101,1);
			this->character.focus_y = FIXED_DEC(-139,1);	
		}
		if (stage.song_step == 911)
		{
			this->character.focus_x = FIXED_DEC(111,1);
			this->character.focus_y = FIXED_DEC(-503,1);
			this->character.focus_zoom = FIXED_DEC(2295,512);
		}
		if (stage.song_step == 912)
		{
			this->character.focus_y = FIXED_DEC(-175,1);
			this->character.focus_zoom = FIXED_DEC(767,512);
		}
		if (stage.song_step == 1167)
		{
			this->character.focus_x = FIXED_DEC(-223,1);
			this->character.focus_y = FIXED_DEC(-139,1);
			this->character.focus_zoom = FIXED_DEC(1,1024);
		}
		if (stage.song_step == 1168)
		{
			this->character.focus_x = FIXED_DEC(101,1);
			this->character.focus_zoom = FIXED_DEC(1,1);
		}
		if (stage.song_step == 1423)
		{
			this->character.focus_x = FIXED_DEC(111,1);
			this->character.focus_y = FIXED_DEC(-503,1);
			this->character.focus_zoom = FIXED_DEC(2295,512);
		}
		if (stage.song_step == 1424)
		{
			this->character.focus_y = FIXED_DEC(-175,1);
			this->character.focus_zoom = FIXED_DEC(767,512);
		}
		if (stage.song_step == 1680)
		{
			this->character.focus_x = FIXED_DEC(96,1);
			this->character.focus_y = FIXED_DEC(-145,1);
			this->character.focus_zoom = FIXED_DEC(1,1024);
		}
		if (stage.song_step == 1681)
		{
			this->character.focus_zoom = FIXED_DEC(1,1);
		}
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Stage specific animations
		switch (stage.stage_id)
		{
			case StageId_1_3: //Dead
				if (stage.song_step >= 2193)
					character->set_anim(character, CharAnim_RightAlt);
				break;
			default:
				break;
		}
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Henry_SetFrame);
	if (stage.song_step >= -30 && stage.song_step <= 270 || stage.song_step >= 1680)
		Character_Draw(character, &this->tex, &char_henry_frame[this->frame]);
}

void Char_Henry_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Henry_Free(Character *character)
{
	Char_Henry *this = (Char_Henry*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Henry_New(fixed_t x, fixed_t y)
{
	//Allocate henry object
	Char_Henry *this = Mem_Alloc(sizeof(Char_Henry));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Henry_New] Failed to allocate henry object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Henry_Tick;
	this->character.set_anim = Char_Henry_SetAnim;
	this->character.free = Char_Henry_Free;
	
	Animatable_Init(&this->character.animatable, char_henry_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 4;

	//health bar color
	this->character.health_bar = 0xFF383838;
	
	this->character.focus_x = FIXED_DEC(96,1);
	this->character.focus_y = FIXED_DEC(-145,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\HENRY.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //Henry_ArcMain_Idle0
		"idle1.tim", //Henry_ArcMain_Idle1
		"idle2.tim", //Henry_ArcMain_Idle2
		"idle3.tim", //Henry_ArcMain_Idle3
		"left0.tim",  //Henry_ArcMain_Left0
		"left1.tim",  //Henry_ArcMain_Left1
		"down0.tim",  //Henry_ArcMain_Down0
		"down1.tim",  //Henry_ArcMain_Down1
		"up0.tim",    //Henry_ArcMain_Up0
		"up1.tim",    //Henry_ArcMain_Up1
		"right0.tim", //Henry_ArcMain_Right0
		"right1.tim", //Henry_ArcMain_Right1
		"idiot.tim", //Henry_ArcMain_Idiot
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
