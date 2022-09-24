/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "henrytv.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Henry TV character structure
enum
{
	HenryTV_ArcMain_Idle0,
	HenryTV_ArcMain_Idle1,
	HenryTV_ArcMain_Left0,
	HenryTV_ArcMain_Left1,
	HenryTV_ArcMain_Down0,
	HenryTV_ArcMain_Down1,
	HenryTV_ArcMain_Up0,
	HenryTV_ArcMain_Up1,
	HenryTV_ArcMain_Right0,
	HenryTV_ArcMain_Right1,
	
	HenryTV_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[HenryTV_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_HenryTV;

//Henry TV character definitions
static const CharFrame char_henrytv_frame[] = {
	{HenryTV_ArcMain_Idle0, {  0,  0,191,154}, { 42,183+4}}, //0 idle 1
	{HenryTV_ArcMain_Idle1, {  0,  0,191,154}, { 42,183+4}}, //1 idle 2
	
	{HenryTV_ArcMain_Left0, {  0,  0,191,154}, { 42,183+4}}, //2 left 1
	{HenryTV_ArcMain_Left1, {  0,  0,191,154}, { 42,183+4}}, //3 left 2
	
	{HenryTV_ArcMain_Down0, {  0,  0,191,154}, { 42,183+4}}, //4 down 1
	{HenryTV_ArcMain_Down1, {  0,  0,191,154}, { 42,183+4}}, //5 down 2
	
	{HenryTV_ArcMain_Up0, {  0,  0,191,154}, { 42,183+4}}, //6 up 1
	{HenryTV_ArcMain_Up1, {  0,  0,191,154}, { 42,183+4}}, //7 up 2
	
	{HenryTV_ArcMain_Right0, {  0,  0,191,154}, { 42,183+4}}, //8 right 1
	{HenryTV_ArcMain_Right1, {  0,  0,191,154}, { 42,183+4}}, //9 right 2
};

static const Animation char_henrytv_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 2,  3, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 4,  5, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 6,  7, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){8, 9, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Henry TV character functions
void Char_HenryTV_SetFrame(void *user, u8 frame)
{
	Char_HenryTV *this = (Char_HenryTV*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_henrytv_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_HenryTV_Tick(Character *character)
{
	Char_HenryTV *this = (Char_HenryTV*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_HenryTV_SetFrame);
	if (stage.song_step >= 270 && stage.song_step <= 911 || stage.song_step >= 1167 && stage.song_step <= 1423)
		Character_Draw(character, &this->tex, &char_henrytv_frame[this->frame]);
}

void Char_HenryTV_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_HenryTV_Free(Character *character)
{
	Char_HenryTV *this = (Char_HenryTV*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_HenryTV_New(fixed_t x, fixed_t y)
{
	//Allocate henrytv object
	Char_HenryTV *this = Mem_Alloc(sizeof(Char_HenryTV));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_HenryTV_New] Failed to allocate henrytv object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_HenryTV_Tick;
	this->character.set_anim = Char_HenryTV_SetAnim;
	this->character.free = Char_HenryTV_Free;
	
	Animatable_Init(&this->character.animatable, char_henrytv_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 4;

	//health bar color
	this->character.health_bar = 0xFF383838;
	
	this->character.focus_x = FIXED_DEC(47,1);
	this->character.focus_y = FIXED_DEC(-118,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\HENRYTV.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //HenryTV_ArcMain_Idle0
		"idle1.tim", //HenryTV_ArcMain_Idle1
		"left0.tim",  //HenryTV_ArcMain_Left0
		"left1.tim",  //HenryTV_ArcMain_Left1
		"down0.tim",  //HenryTV_ArcMain_Down0
		"down1.tim",  //HenryTV_ArcMain_Down1
		"up0.tim",    //HenryTV_ArcMain_Up0
		"up1.tim",    //HenryTV_ArcMain_Up1
		"right0.tim", //HenryTV_ArcMain_Right0
		"right1.tim", //HenryTV_ArcMain_Right1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
