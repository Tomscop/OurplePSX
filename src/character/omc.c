/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "omc.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//OMC character structure
enum
{
	OMC_ArcMain_Idle0,
	OMC_ArcMain_Idle1,
	OMC_ArcMain_Left,
	OMC_ArcMain_Down,
	OMC_ArcMain_Up,
	OMC_ArcMain_Right,
	
	OMC_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[OMC_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_OMC;

//OMC character definitions
static const CharFrame char_omc_frame[] = {
	{OMC_ArcMain_Idle0, {  0,  0,156,108}, { 42,183}}, //0 idle 1
	{OMC_ArcMain_Idle0, {  0,109,156,108}, { 42,183}}, //1 idle 2
	{OMC_ArcMain_Idle1, {  0,  0,156,108}, { 42,183}}, //2 idle 3
	{OMC_ArcMain_Idle1, {  0,109,156,108}, { 42,183}}, //3 idle 4
	
	{OMC_ArcMain_Left, {  0,  0,156,108}, { 42,183}}, //4 left 1
	{OMC_ArcMain_Left, {  0,109,156,108}, { 42,183}}, //5 left 2
	
	{OMC_ArcMain_Down, {  0,  0,156,108}, { 42,183}}, //6 down 1
	{OMC_ArcMain_Down, {  0,109,156,108}, { 42,183}}, //7 down 2
	
	{OMC_ArcMain_Up, {  0,  0,156,108}, { 42,183}}, //8 up 1
	{OMC_ArcMain_Up, {  0,109,156,108}, { 42,183}}, //9 up 2
	
	{OMC_ArcMain_Right, {  0,  0,156,108}, { 42,183}}, //10 right 1
	{OMC_ArcMain_Right, {  0,109,156,108}, { 42,183}}, //11 right 2
};

static const Animation char_omc_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 4, 5, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 6, 7, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 8, 9, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 10, 11, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//OMC character functions
void Char_OMC_SetFrame(void *user, u8 frame)
{
	Char_OMC *this = (Char_OMC*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_omc_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_OMC_Tick(Character *character)
{
	Char_OMC *this = (Char_OMC*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_OMC_SetFrame);
	Character_Draw(character, &this->tex, &char_omc_frame[this->frame]);
}

void Char_OMC_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_OMC_Free(Character *character)
{
	Char_OMC *this = (Char_OMC*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_OMC_New(fixed_t x, fixed_t y)
{
	//Allocate omc object
	Char_OMC *this = Mem_Alloc(sizeof(Char_OMC));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_OMC_New] Failed to allocate omc object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_OMC_Tick;
	this->character.set_anim = Char_OMC_SetAnim;
	this->character.free = Char_OMC_Free;
	
	Animatable_Init(&this->character.animatable, char_omc_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 3;

	//health bar color
	this->character.health_bar = 0xFFF94345;
	
	this->character.focus_x = FIXED_DEC(62,1);
	this->character.focus_y = FIXED_DEC(-141,1);
	this->character.focus_zoom = FIXED_DEC(465,512);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR2\\OMC.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //OMC_ArcMain_Idle0
		"idle1.tim", //OMC_ArcMain_Idle1
		"left.tim",  //OMC_ArcMain_Left
		"down.tim",  //OMC_ArcMain_Down
		"up.tim",    //OMC_ArcMain_Up
		"right.tim", //OMC_ArcMain_Right
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
