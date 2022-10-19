/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "matpat.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Matpat character structure
enum
{
	Matpat_ArcMain_Idle0,
	Matpat_ArcMain_Idle1,
	Matpat_ArcMain_Idle2,
	Matpat_ArcMain_Idle3,
	Matpat_ArcMain_Idle4,
	Matpat_ArcMain_Left0,
	Matpat_ArcMain_Left1,
	Matpat_ArcMain_Left2,
	Matpat_ArcMain_Left3,
	Matpat_ArcMain_Left4,
	Matpat_ArcMain_Left5,
	Matpat_ArcMain_Down0,
	Matpat_ArcMain_Down1,
	Matpat_ArcMain_Up0,
	Matpat_ArcMain_Up1,
	Matpat_ArcMain_Right0,
	Matpat_ArcMain_Right1,
	
	Matpat_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Matpat_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Matpat;

//Matpat character definitions
static const CharFrame char_matpat_frame[] = {
	{Matpat_ArcMain_Idle0, {  0,   0, 165, 205}, { 42, 183+4}}, //0 idle 1
	{Matpat_ArcMain_Idle1, {  0,   0, 165, 205}, { 42, 183+4}}, //1 idle 2
	{Matpat_ArcMain_Idle2, {  0,   0, 165, 205}, { 42, 183+4}}, //2 idle 3
	{Matpat_ArcMain_Idle3, {  0,   0, 165, 205}, { 42, 183+4}}, //3 idle 4
	{Matpat_ArcMain_Idle4, {  0,   0, 165, 205}, { 42, 183+4}}, //4 idle 5
	
	{Matpat_ArcMain_Left0, {  0,   0, 165, 205}, { 42, 183+4}}, //5 left 1
	{Matpat_ArcMain_Left1, {  0,   0, 165, 205}, { 42, 183+4}}, //6 left 2
	{Matpat_ArcMain_Left2, {  0,   0, 165, 205}, { 42, 183+4}}, //7 left 3
	{Matpat_ArcMain_Left3, {  0,   0, 165, 205}, { 42, 183+4}}, //8 left 4
	{Matpat_ArcMain_Left4, {  0,   0, 165, 205}, { 42, 183+4}}, //9 left 5
	{Matpat_ArcMain_Left5, {  0,   0, 165, 205}, { 42, 183+4}}, //10 left 6
	
	{Matpat_ArcMain_Down0, {  0,   0, 165, 205}, { 42, 183+4}}, //11 down 1
	{Matpat_ArcMain_Down1, {  0,   0, 165, 205}, { 42, 183+4}}, //12 down 2
	
	{Matpat_ArcMain_Up0, {  0,   0, 165, 205}, { 42, 183+4}}, //13 up 1
	{Matpat_ArcMain_Up1, {  0,   0, 165, 205}, { 42, 183+4}}, //14 up 2
	
	{Matpat_ArcMain_Right0, {  0,   0, 165, 205}, { 42, 183+4}}, //15 right 1
	{Matpat_ArcMain_Right1, {  0,   0, 165, 205}, { 42, 183+4}}, //16 right 2
};

static const Animation char_matpat_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}}, //CharAnim_Idle
	{1, (const u8[]){ 5, 6, 7, 8, 9, 10, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 11, 12, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 13, 14, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){15, 16, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Matpat character functions
void Char_Matpat_SetFrame(void *user, u8 frame)
{
	Char_Matpat *this = (Char_Matpat*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_matpat_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Matpat_Tick(Character *character)
{
	Char_Matpat *this = (Char_Matpat*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Matpat_SetFrame);
	Character_Draw(character, &this->tex, &char_matpat_frame[this->frame]);
}

void Char_Matpat_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Matpat_Free(Character *character)
{
	Char_Matpat *this = (Char_Matpat*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Matpat_New(fixed_t x, fixed_t y)
{
	//Allocate matpat object
	Char_Matpat *this = Mem_Alloc(sizeof(Char_Matpat));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Matpat_New] Failed to allocate matpat object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Matpat_Tick;
	this->character.set_anim = Char_Matpat_SetAnim;
	this->character.free = Char_Matpat_Free;
	
	Animatable_Init(&this->character.animatable, char_matpat_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 4;

	//health bar color
	this->character.health_bar = 0xFF33E47F;
	
	this->character.focus_x = FIXED_DEC(109,1);
	this->character.focus_y = FIXED_DEC(-117,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\MATPAT.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //Matpat_ArcMain_Idle0
		"idle1.tim", //Matpat_ArcMain_Idle1
		"idle2.tim", //Matpat_ArcMain_Idle2
		"idle3.tim", //Matpat_ArcMain_Idle3
		"idle4.tim", //Matpat_ArcMain_Idle4
		"left0.tim", //Matpat_ArcMain_Left0
		"left1.tim", //Matpat_ArcMain_Left1
		"left2.tim", //Matpat_ArcMain_Left2
		"left3.tim", //Matpat_ArcMain_Left3
		"left4.tim", //Matpat_ArcMain_Left4
		"left5.tim", //Matpat_ArcMain_Left5
		"down0.tim", //Matpat_ArcMain_Down0
		"down1.tim", //Matpat_ArcMain_Down1
		"up0.tim", //Matpat_ArcMain_Up0
		"up1.tim", //Matpat_ArcMain_Up1
		"right0.tim", //Matpat_ArcMain_Right0
		"right1.tim", //Matpat_ArcMain_Right1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
