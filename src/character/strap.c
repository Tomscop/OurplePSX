/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "strap.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Springtrap character structure
enum
{
	STrap_ArcMain_Idle,
	STrap_ArcMain_Left,
	STrap_ArcMain_Down,
	STrap_ArcMain_Up,
	STrap_ArcMain_Right,
	
	STrap_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[STrap_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_STrap;

//Springtrap character definitions
static const CharFrame char_strap_frame[] = {
	{STrap_ArcMain_Idle, {  0,  0,204,185}, { 42,183}}, //0 idle 1
	{STrap_ArcMain_Idle, {  0,  0,204,185}, { 42,178}}, //1 idle 2
	{STrap_ArcMain_Idle, {  0,  0,204,185}, { 42,180}}, //2 idle 3
	{STrap_ArcMain_Idle, {  0,  0,204,185}, { 42,182}}, //3 idle 4
	
	{STrap_ArcMain_Left, {  0,  0,204,185}, { 42,183}}, //4 left 1
	{STrap_ArcMain_Left, {  0,  0,204,185}, { 42,180}}, //5 left 2
	
	{STrap_ArcMain_Down, {  0,  0,204,185}, { 42,183}}, //6 down 1
	{STrap_ArcMain_Down, {  0,  0,204,185}, { 42,180}}, //7 down 2
	
	{STrap_ArcMain_Up, {  0,  0,204,185}, { 42,183}}, //8 up 1
	{STrap_ArcMain_Up, {  0,  0,204,185}, { 42,180}}, //9 up 2
	
	{STrap_ArcMain_Right, {  0,  0,204,185}, { 42,183}}, //10 right 1
	{STrap_ArcMain_Right, {  0,  0,204,185}, { 42,180}}, //11 right 2
};

static const Animation char_strap_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0, 0, 0, 1, 1, 2, 2, 3, ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{3, (const u8[]){ 4, 5, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{3, (const u8[]){ 6, 7, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{3, (const u8[]){ 8, 9, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{3, (const u8[]){ 10, 11, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Springtrap character functions
void Char_STrap_SetFrame(void *user, u8 frame)
{
	Char_STrap *this = (Char_STrap*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_strap_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_STrap_Tick(Character *character)
{
	Char_STrap *this = (Char_STrap*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_STrap_SetFrame);
	Character_Draw(character, &this->tex, &char_strap_frame[this->frame]);
}

void Char_STrap_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_STrap_Free(Character *character)
{
	Char_STrap *this = (Char_STrap*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_STrap_New(fixed_t x, fixed_t y)
{
	//Allocate strap object
	Char_STrap *this = Mem_Alloc(sizeof(Char_STrap));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_STrap_New] Failed to allocate strap object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_STrap_Tick;
	this->character.set_anim = Char_STrap_SetAnim;
	this->character.free = Char_STrap_Free;
	
	Animatable_Init(&this->character.animatable, char_strap_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;

	//health bar color
	this->character.health_bar = 0xFF6C7C48;
	
	this->character.focus_x = FIXED_DEC(102,1);
	this->character.focus_y = FIXED_DEC(-103,1);
	this->character.focus_zoom = FIXED_DEC(717,512);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR2\\STRAP.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle.tim", //STrap_ArcMain_Idle
		"left.tim",  //STrap_ArcMain_Left
		"down.tim",  //STrap_ArcMain_Down
		"up.tim",    //STrap_ArcMain_Up
		"right.tim", //STrap_ArcMain_Right
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
