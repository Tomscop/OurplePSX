/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "fatjones.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Fat Jones character structure
enum
{
	FatJones_ArcMain_Idle0,
	FatJones_ArcMain_Idle1,
	FatJones_ArcMain_Idle2,
	FatJones_ArcMain_Idle3,
	FatJones_ArcMain_Idle4,
	FatJones_ArcMain_Idle5,
	FatJones_ArcMain_Left,
	FatJones_ArcMain_Down,
	FatJones_ArcMain_Up,
	FatJones_ArcMain_Right,
	
	FatJones_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[FatJones_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_FatJones;

//Fat Jones character definitions
static const CharFrame char_fatjones_frame[] = {
	{FatJones_ArcMain_Idle0, {  0,  0,255,158}, { 42,183+4}}, //0 idle 1
	{FatJones_ArcMain_Idle1, {  0,  0,255,158}, { 42,183+4}}, //1 idle 2
	{FatJones_ArcMain_Idle2, {  0,  0,255,158}, { 42,183+4}}, //2 idle 3
	{FatJones_ArcMain_Idle3, {  0,  0,255,158}, { 42,183+4}}, //3 idle 4
	{FatJones_ArcMain_Idle4, {  0,  0,255,158}, { 42,183+4}}, //4 idle 5
	{FatJones_ArcMain_Idle5, {  0,  0,255,158}, { 42,183+4}}, //5 idle 6
	
	{FatJones_ArcMain_Left, {  0,  0,255,158}, { 42,183+4}}, //6 left 1
	
	{FatJones_ArcMain_Down, {  0,  0,255,158}, { 42,183+4}}, //7 down 1
	
	{FatJones_ArcMain_Up, {  0,  0,255,158}, { 42,183+4}}, //8 up 1
	
	{FatJones_ArcMain_Right, {  0,  0,255,158}, { 42,183+4}}, //9 right 1
};

static const Animation char_fatjones_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3, 4, 5, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 6, 6, 5, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 7, 7, 5, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 8, 8, 5, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){9, 9, 5, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Fat Jones character functions
void Char_FatJones_SetFrame(void *user, u8 frame)
{
	Char_FatJones *this = (Char_FatJones*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_fatjones_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_FatJones_Tick(Character *character)
{
	Char_FatJones *this = (Char_FatJones*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_FatJones_SetFrame);
	Character_Draw(character, &this->tex, &char_fatjones_frame[this->frame]);
}

void Char_FatJones_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_FatJones_Free(Character *character)
{
	Char_FatJones *this = (Char_FatJones*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_FatJones_New(fixed_t x, fixed_t y)
{
	//Allocate fatjones object
	Char_FatJones *this = Mem_Alloc(sizeof(Char_FatJones));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_FatJones_New] Failed to allocate fatjones object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_FatJones_Tick;
	this->character.set_anim = Char_FatJones_SetAnim;
	this->character.free = Char_FatJones_Free;
	
	Animatable_Init(&this->character.animatable, char_fatjones_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 6;

	//health bar color
	this->character.health_bar = 0xFFFFFFFF;
	
	this->character.focus_x = FIXED_DEC(117,1);
	this->character.focus_y = FIXED_DEC(-102,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\FATJONES.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //FatJones_ArcMain_Idle0
		"idle1.tim", //FatJones_ArcMain_Idle1
		"idle2.tim", //FatJones_ArcMain_Idle2
		"idle3.tim", //FatJones_ArcMain_Idle3
		"idle4.tim", //FatJones_ArcMain_Idle4
		"idle5.tim", //FatJones_ArcMain_Idle5
		"left.tim",  //FatJones_ArcMain_Left
		"down.tim",  //FatJones_ArcMain_Down
		"up.tim",    //FatJones_ArcMain_Up
		"right.tim", //FatJones_ArcMain_Right
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
