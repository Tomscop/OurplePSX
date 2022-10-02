/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "plush.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Plush character structure
enum
{
	Plush_ArcMain_Plush,
	
	Plush_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Plush_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Plush;

//Plush character definitions
static const CharFrame char_plush_frame[] = {
	{Plush_ArcMain_Plush, { 87,  8, 22, 30}, { 42, 183+4}}, //0 idle 1
	
	{Plush_ArcMain_Plush, { 11, 49, 22, 30}, { 42, 183+4}}, //1 left 1
	{Plush_ArcMain_Plush, { 49, 49, 22, 30}, { 42, 183+4}}, //2 left 2
	
	{Plush_ArcMain_Plush, { 11,  8, 22, 30}, { 42, 183+4}}, //3 down 1
	{Plush_ArcMain_Plush, { 49,  8, 22, 30}, { 42, 183+4}}, //4 down 2
	
	{Plush_ArcMain_Plush, { 49, 90, 22, 30}, { 42, 183+4}}, //5 up 1
	{Plush_ArcMain_Plush, { 87, 90, 22, 30}, { 42, 183+4}}, //6 up 2
	
	{Plush_ArcMain_Plush, { 87, 49, 22, 30}, { 42, 183+4}}, //7 right 1
	{Plush_ArcMain_Plush, { 11, 90, 22, 30}, { 42, 183+4}}, //8 right 2
};

static const Animation char_plush_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 1,  2, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 3,  4, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 5,  6, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 7,  8, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Plush character functions
void Char_Plush_SetFrame(void *user, u8 frame)
{
	Char_Plush *this = (Char_Plush*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_plush_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Plush_Tick(Character *character)
{
	Char_Plush *this = (Char_Plush*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Plush_SetFrame);
	Character_Draw(character, &this->tex, &char_plush_frame[this->frame]);
}

void Char_Plush_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Plush_Free(Character *character)
{
	Char_Plush *this = (Char_Plush*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Plush_New(fixed_t x, fixed_t y)
{
	//Allocate plush object
	Char_Plush *this = Mem_Alloc(sizeof(Char_Plush));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Plush_New] Failed to allocate plush object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Plush_Tick;
	this->character.set_anim = Char_Plush_SetAnim;
	this->character.free = Char_Plush_Free;
	
	Animatable_Init(&this->character.animatable, char_plush_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 4;

	//health bar color
	this->character.health_bar = 0xFF6F1F73;
	
	this->character.focus_x = FIXED_DEC(-31,1);
	this->character.focus_y = FIXED_DEC(-168,1);
	this->character.focus_zoom = FIXED_DEC(151,100);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR2\\PLUSH.ARC;1");
	
	const char **pathp = (const char *[]){
		"plush.tim", //Plush_ArcMain_Plush
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
