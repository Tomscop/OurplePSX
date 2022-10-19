/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "springb.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//SpringB character structure
enum
{
	SpringB_ArcMain_Idle0,
	SpringB_ArcMain_Idle1,
	SpringB_ArcMain_Idle2,
	SpringB_ArcMain_Idle3,
	SpringB_ArcMain_Left,
	SpringB_ArcMain_Down,
	SpringB_ArcMain_Up,
	SpringB_ArcMain_Right,
	
	SpringB_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[SpringB_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_SpringB;

//SpringB character definitions
static const CharFrame char_springb_frame[] = {
	{SpringB_ArcMain_Idle0, {  0,  0, 87,133}, { 42,183}}, //0 idle 1
	{SpringB_ArcMain_Idle0, { 88,  0, 87,133}, { 42,183}}, //1 idle 2
	{SpringB_ArcMain_Idle1, {  0,  0, 87,133}, { 42,183}}, //2 idle 3
	{SpringB_ArcMain_Idle1, { 88,  0, 87,133}, { 42,183}}, //3 idle 4
	{SpringB_ArcMain_Idle2, {  0,  0, 87,133}, { 42,183}}, //4 idle 5
	{SpringB_ArcMain_Idle2, { 88,  0, 87,133}, { 42,183}}, //5 idle 6
	{SpringB_ArcMain_Idle3, {  0,  0, 87,133}, { 42,183}}, //6 idle 7
	
	{SpringB_ArcMain_Left, {  0,  0, 87,133}, { 42,183}}, //7 left 1
	{SpringB_ArcMain_Left, { 88,  0, 87,133}, { 42,183}}, //8 left 2
	
	{SpringB_ArcMain_Down, {  0,  0, 87,133}, { 42,183}}, //9 down 1
	{SpringB_ArcMain_Down, { 88,  0, 87,133}, { 42,183}}, //10 down 2
	
	{SpringB_ArcMain_Up, {  0,  0, 87,133}, { 42,183}}, //11 up 1
	{SpringB_ArcMain_Up, { 88,  0, 87,133}, { 42,183}}, //12 up 2
	
	{SpringB_ArcMain_Right, {  0,  0, 87,133}, { 42,183}}, //13 right 1
	{SpringB_ArcMain_Right, { 88,  0, 87,133}, { 42,183}}, //14 right 2
};

static const Animation char_springb_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 1, 2, 3, 4, 5, 5, 6, ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{2, (const u8[]){ 7, 8, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 9, 10, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 11, 12, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 13, 14, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//SpringB character functions
void Char_SpringB_SetFrame(void *user, u8 frame)
{
	Char_SpringB *this = (Char_SpringB*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_springb_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_SpringB_Tick(Character *character)
{
	Char_SpringB *this = (Char_SpringB*)character;
	
	//Icon change
	if (stage.song_step == 656)
		this->character.health_i = 4;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_SpringB_SetFrame);
	Character_Draw(character, &this->tex, &char_springb_frame[this->frame]);
}

void Char_SpringB_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_SpringB_Free(Character *character)
{
	Char_SpringB *this = (Char_SpringB*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_SpringB_New(fixed_t x, fixed_t y)
{
	//Allocate springb object
	Char_SpringB *this = Mem_Alloc(sizeof(Char_SpringB));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_SpringB_New] Failed to allocate springb object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_SpringB_Tick;
	this->character.set_anim = Char_SpringB_SetAnim;
	this->character.free = Char_SpringB_Free;
	
	Animatable_Init(&this->character.animatable, char_springb_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 3;

	//health bar color
	this->character.health_bar = 0xFFC39B06;
	
	this->character.focus_x = FIXED_DEC(47,1);
	this->character.focus_y = FIXED_DEC(-121,1);
	this->character.focus_zoom = FIXED_DEC(311,256);
	
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\SPRINGB.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //SpringB_ArcMain_Idle0
		"idle1.tim", //SpringB_ArcMain_Idle1
		"idle2.tim", //SpringB_ArcMain_Idle2
		"idle3.tim", //SpringB_ArcMain_Idle3
		"left.tim",  //SpringB_ArcMain_Left
		"down.tim",  //SpringB_ArcMain_Down
		"up.tim",    //SpringB_ArcMain_Up
		"right.tim", //SpringB_ArcMain_Right
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
