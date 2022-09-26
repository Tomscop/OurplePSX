/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "prange.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Prange character structure
enum
{
	Prange_ArcMain_Prange0,
	Prange_ArcMain_Prange1,
	
	Prange_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Prange_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Prange;

//Prange character definitions
static const CharFrame char_prange_frame[] = {
	{Prange_ArcMain_Prange0, {  0,  0, 78,127}, { 42,183+4}}, //0 idle 1
	{Prange_ArcMain_Prange0, { 79,  0, 78,127}, { 42,183+4}}, //1 idle 2
	{Prange_ArcMain_Prange0, {158,  0, 78,127}, { 42,183+4}}, //2 idle 3
	{Prange_ArcMain_Prange0, {  0,128, 78,127}, { 42,183+4}}, //3 idle 4
	
	{Prange_ArcMain_Prange0, { 79,128, 78,127}, { 42,183+4}}, //4 left 1
	{Prange_ArcMain_Prange0, {158,128, 78,127}, { 42,183+4}}, //5 left 2
	
	{Prange_ArcMain_Prange1, {  0,  0, 78,127}, { 42,183+4}}, //6 down 1
	{Prange_ArcMain_Prange1, { 79,  0, 78,127}, { 42,183+4}}, //7 down 2
	
	{Prange_ArcMain_Prange1, {158,  0, 78,127}, { 42,183+4}}, //8 up 1
	{Prange_ArcMain_Prange1, {  0,128, 78,127}, { 42,183+4}}, //9 up 2
	
	{Prange_ArcMain_Prange1, { 79,128, 78,127}, { 42,183+4}}, //10 right 1
	{Prange_ArcMain_Prange1, {158,128, 78,127}, { 42,183+4}}, //11 right 2
};

static const Animation char_prange_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 4,  5, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 6,  7, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 8,  9, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){10, 11, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Prange character functions
void Char_Prange_SetFrame(void *user, u8 frame)
{
	Char_Prange *this = (Char_Prange*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_prange_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Prange_Tick(Character *character)
{
	Char_Prange *this = (Char_Prange*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Prange_SetFrame);
	Character_Draw(character, &this->tex, &char_prange_frame[this->frame]);
}

void Char_Prange_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Prange_Free(Character *character)
{
	Char_Prange *this = (Char_Prange*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Prange_New(fixed_t x, fixed_t y)
{
	//Allocate prange object
	Char_Prange *this = Mem_Alloc(sizeof(Char_Prange));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Prange_New] Failed to allocate prange object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Prange_Tick;
	this->character.set_anim = Char_Prange_SetAnim;
	this->character.free = Char_Prange_Free;
	
	Animatable_Init(&this->character.animatable, char_prange_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 3;

	//health bar color
	this->character.health_bar = 0xFF915F00;
	
	this->character.focus_x = FIXED_DEC(66,1);
	this->character.focus_y = FIXED_DEC(-129,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\PRANGE.ARC;1");
	
	const char **pathp = (const char *[]){
		"prange0.tim", //Prange_ArcMain_Prange0
		"prange1.tim", //Prange_ArcMain_Prange1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
